// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.cpp
// Author:       dingfang
// CreateDate:   2020-10-31 12:31:56
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 13:25:07
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/stringUtil.h"
#include "dfsrrWebServer/dfsrr.h"
#include "dfsrrWebServer/route.h"
#include "event2/buffer.h"
#include <event2/keyvalq_struct.h>

#include <vector>

using namespace std;
using namespace common;
using json = nlohmann::json;

namespace dfsrrWebServer
{


    std::set<std::string> DfsrrDataSelect::moduleSet_ 
    {
        "cpu"
            , "memory"
            , "load"
            , "partition"
            , "traffic"
            , "tcp"
            , "udp"
    };

    std::map< std::string, std::set<std::string> > DfsrrDataSelect::metricSet_
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


    DfsrrDataSelect::DfsrrDataSelect(const nlohmann::json &conf)
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


    bool DfsrrDataSelect::parseConfig(const json &confJson)
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


    /* static void helloWorld(struct evhttp_request *req, void *arg); */
    void DfsrrDataSelect::dfsrr(struct evhttp_request *req, void *arg)
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

        struct evkeyvalq *headers = evhttp_request_get_input_headers(req);
        for (struct evkeyval *header = headers->tqh_first; header; header = header->next.tqe_next)
        {
            LOG(DEBUG, " {}: {}", header->key, header->value);
        }

        struct evkeyvalq params;
        if (::evhttp_parse_query(uri.c_str(), &params) != 0)
        {
            LOG(WARN, "parse param failed!");
            return ;
        }

        map<string, string> paramMap;
        for (struct evkeyval *header = params.tqh_first; header; header = header->next.tqe_next)
        {
            // Route::getParam(&params, "module", mod);
            paramMap[header->key] = header->value;
        }
        evhttp_clear_headers(&params);

        LOG(DEBUG, "=============================="); 
        string data;
        Route::getInput(req, data);
        LOG(DEBUG, "data: [{}]", data);
        LOG(DEBUG, "==============================");

        string response;
        DfsrrDataSelect *pDs = static_cast<DfsrrDataSelect *>(arg);
        if (pDs != nullptr)
        {
            pDs->doRun(paramMap, response);
        }

        struct evbuffer *evb = evbuffer_new();
        evbuffer_add_printf(evb, response.c_str());
        evhttp_send_reply(req, 200, "ok", evb);

        if (evb)
        {
            evbuffer_free(evb);
        }
    }


    int DfsrrDataSelect::doRun(const std::map<std::string, std::string> &paramMap, std::string &response)
    {
        json res, data = json::array();
        int code = SuccessCode;
        auto modIt = paramMap.find("module");
        if (modIt == paramMap.end() || moduleSet_.find(modIt->second) == moduleSet_.end())
        {
            LOG(WARN, "param error! not found module.");
            code = ParamErrorCode;
        }
        else
        {
            mod_ = modIt->second;
            if ((code = this->parseCommonParam(paramMap)) == SuccessCode)
            {
                code = this->selectData(data);
            }
        }

        res["code"] = code;
        res["data"] = data;
        res["msg"]  = Route::getResponseMsg(static_cast<ResponseCode_E>(code));
        response = res.dump();

        return 0;
    }


    int DfsrrDataSelect::parseCommonParam(const std::map<std::string, std::string> &paramMap)
    {
        watch_ = "10";
        auto watchIt = paramMap.find("watch");
        if (watchIt != paramMap.end())
        {
            /* 需要检查是否为数字 */
            try
            {
                int w   = stoi(watchIt->second);
                watch_  = w > 50 ? "50" : to_string(w);
            }
            catch (...) 
            { 
                LOG(WARN, "param watch is not number!, watch: [{}]", watchIt->second);
                return ParamErrorCode;
            }
        }

        metric_ = "*";
        auto metricIt = paramMap.find("metric");
        if (metricIt != paramMap.end())
        {
            metric_ = metricIt->second;
            vector<string> metricVec = common::split(metric_, ",");
            for (auto &val : metricVec)
            {
                // metric_ += common::strip(val);
                auto modSet = metricSet_[mod_];
                if (modSet.find(val) == modSet.end())
                {
                    LOG(WARN, "parse metric error! metric: [{}], value: [{}]", metric_, val);
                    return ParamErrorCode;
                }
            }
        }

        startTime_.clear();
        auto startIt = paramMap.find("startTime");
        if (startIt != paramMap.end())
        {
            startTime_ = startIt->second;
        }

        endTime_.clear();
        auto endIt = paramMap.find("endTime");
        if (endIt != paramMap.end())
        {
            endTime_ = startIt->second;
        }

        return SuccessCode;
    }


    int DfsrrDataSelect::selectData(nlohmann::json &data)
    {
        string sql("select ");
        sql += metric_ + " from `" + mod_ + "` where idx > 0 ";

        if (!startTime_.empty())
        {
            sql += " and timestamp >= " + startTime_;
        }

        if (!endTime_.empty())
        {
            sql += " and timestamp <= " + endTime_;
        }

        sql += " order by timestamp desc limit " + watch_;
        if (!dbPtr_->execSql(sql, data))
        {
            LOG(ERROR, "exec sql failed!");
            return ServerErrorCode;
        }

        return SuccessCode;
    }


}; /* dfsrrWebServer namespace end */

