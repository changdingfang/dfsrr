// =======================================================================
//  FileName:     dfsrrExporter.cpp
//  Author:       dingfang
//  CreateDate:   2021-07-17 15:12:39
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-07-18 10:44:25
// =======================================================================

#include "dflog/dflog.h"
#include "common/stringUtil.h"
#include "dfsrrWebServer/dfsrrExporter.h"
#include "dfsrrWebServer/route.h"
#include "event2/buffer.h"
#include <event2/keyvalq_struct.h>

#include <map>
#include <vector>

using namespace std;
using namespace common;
using json = nlohmann::json;
using namespace dfsrrWebServer;

static const string Wrap = "\n";

vector<string> Exporter::mods_
{
    "cpu",
    "memory",
    "load",
    "partition",
    "traffic",
    "tcp",
    "udp"
};

std::set<std::string> Exporter::moduleSet_ 
{
    "cpu"
        , "memory"
        , "load"
        , "partition"
        , "traffic"
        , "tcp"
        , "udp"
};

std::map< std::string, std::set<std::string> > Exporter::metricSet_
{
    { 
        "cpu", 
            {
                "idx", "ip", "timestamp"
                    , "hirq", "idle", "sirq"
                    , "sys", "user", "util", "wait"
            }
    },
        {
            "memory",
            { 
                "idx", "ip", "timestamp"
                    , "buff", "cache", "free"
                    , "total", "used", "util" 
            }
        },
        {
            "load",
            {
                "idx", "ip", "timestamp"
                    , "load1", "load5", "load15", "plit", "runq"
            }
        },
        {
            "partition",
            {
                "idx", "ip", "timestamp"
                    , "bfree", "btotl", "bused", "device"
                    , "ifree", "itotl", "iutil", "mount", "util"
            }
        },
        {
            "traffic",
            {
                "idx", "ip", "timestamp"
                    , "bytin", "bytout", "device"
                    , "pktdrp" , "pkterr", "pktin", "pktout",
            }
        },
        {
            "tcp",
            {
                "idx", "ip", "timestamp"
                    , "AtmpFa", "CurrEs", "EstRes", "active"
                    , "iseg", "outseg", "pasive", "retran"
            }
        },
        {
            "udp",
            {
                "idx", "ip", "timestamp"
                    , "idgm", "idmerr", "noport", "odgm"
            }
        }
};


Exporter::Exporter(const nlohmann::json &conf)
{
    if (!this->parseConfig(conf))
    {
        throw ("parse config error!");
    }

    try
    {
        dbPtr_ = unique_ptr<MysqlDB>(new MysqlDB(dbinfo_));
    }
    catch (...)
    {
        LOG(ERROR, "init mysql database failed!");
        throw;
    }
}

bool Exporter::parseConfig(const json &confJson)
{
    auto dbIt = confJson.find("database");
    if (dbIt == confJson.end())
    {
        LOG(WARN, "not found database config!");
        return false;
    }

    try
    {
        dbinfo_.ip       = dbIt->at("addr");
        dbinfo_.port     = dbIt->at("port");
        dbinfo_.dbName   = dbIt->at("dbname");
        dbinfo_.dbUser   = dbIt->at("username");
        dbinfo_.dbUserPwd= dbIt->at("userpwd");
    }
    catch (json::exception &e)
    {
        LOG(ERROR, "error: [{}]", e.what());
        return false;
    }
    catch (...)
    {
        LOG(ERROR, "database config failed...");
        return false;
    }

    return true;
}

void Exporter::exporter(struct evhttp_request *req, void *arg)
{
    string cmdtype;
    Route::getCmdType(req, cmdtype);
    // if (::evhttp_request_get_command(req) != EVHTTP_REQ_GET)
    if (cmdtype != "GET")
    {
        LOG(WARN, "request not GET!, type: [{}]", cmdtype);
        return ;
    }

    string uri = ::evhttp_request_get_uri(req);
    LOG(INFO, "Received a {} request for {}\nHeader: ", cmdtype, uri);

    map<string, string> headerMap;
    Route::getHeaderOfReq(req, headerMap);

    LOG(DEBUG, "=============================="); 
    string data;
    Route::getInput(req, data);
    LOG(DEBUG, "data: [{}]", data);
    LOG(DEBUG, "==============================");

    ::evhttp_add_header(::evhttp_request_get_output_headers(req), "Access-Control-Allow-Origin", "*");
    ::evhttp_add_header(::evhttp_request_get_output_headers(req) , "Content-Type", "text/plain; version=0.0.4; charset=utf-8");

    string response;
    Exporter *pEt = static_cast<Exporter*>(arg);
    if (pEt != nullptr)
    {
        pEt->doRun(response);
    }

    struct evbuffer *evb = evbuffer_new();
    evbuffer_add_printf(evb, response.c_str());
    evhttp_send_reply(req, 200, "ok", evb);

    if (evb)
    {
        evbuffer_free(evb);
    }
}

int Exporter::doRun(std::string &response)
{
    string data;
    int code = SuccessCode;
    code = this->getMetgrics(data);

    string errData  = Route::getResponseMsg(static_cast<ResponseCode_E>(code));
    response = code == SuccessCode ? data : errData;

    return 0;
}

int Exporter::getMetgrics(string &metrics)
{
    for (const auto &mod : mods_)
    {
        string sql("select * from `");
        sql.append(mod);
        if (mod == "traffic")
        {
            sql.append("` where `device` = \"total\" order by idx desc limit 1");
        }
        else {
            sql.append("` order by idx desc limit 1");
        }

        json data = json::array();
        if (!dbPtr_->execSql(sql, data))
        {
            LOG(WARN, "exec sql failed!, mod: {}", mod);
            continue;
        }
        LOG(INFO, "data: {}", data.dump(4));
        json metricsJson = json::object();
        if (data.is_array() && data.size() >= 1 && data[0].is_object())
        {
            metricsJson = data[0];
        }
        this->WrapPrometheus(metrics, metricsJson, mod);
    }

    return SuccessCode;
}

int Exporter::WrapPrometheus(string &metrics, const json &metricsJson, const string &mod)
{
    for (const auto &it : metricsJson.items())
    {
        if (it.key() == "ip" || it.key() == "device" || it.key() == "mount" || it.key() == "idx")
        {
            continue;
        }
        string metric = mod + "_" + it.key();
        string value = it.value();
        string prometheusData;
        prometheusData.append("# HELP ");
        prometheusData.append(metric);
        prometheusData.append(" ...");
        prometheusData.append(Wrap);
        prometheusData.append("# TYPE ");
        prometheusData.append(metric);
        prometheusData.append(" gauge");
        prometheusData.append(Wrap);

        prometheusData.append(metric);
        try
        {
            string ip = metricsJson.at("ip");
            prometheusData.append(" {addr=\"");
            prometheusData.append(ip);
            prometheusData.append("\"");
            auto devIt   = metricsJson.find("device");
            auto mountIt = metricsJson.find("mount");

            if (devIt != metricsJson.end())
            {
                prometheusData.append(",device=\"");
                prometheusData.append(*devIt);
                prometheusData.append("\"");
            }

            if (mountIt != metricsJson.end())
            {
                prometheusData.append(",mount=\"");
                prometheusData.append(*mountIt);
                prometheusData.append("\"");
            }
            prometheusData.append("}");
        }
        catch (json::exception &e)
        {
            LOG(WARN, "get ip failed: [{}]", e.what());
        }
        prometheusData.append(" ");
        prometheusData.append(value);
        prometheusData.append(Wrap);
        metrics.append(prometheusData);
    }

    return 0;
}

