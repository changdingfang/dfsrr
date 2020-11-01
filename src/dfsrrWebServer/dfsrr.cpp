// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.cpp
// Author:       dingfang
// CreateDate:   2020-10-31 12:31:56
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 10:47:33
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrrWebServer/dfsrr.h"
#include "dfsrrWebServer/route.h"
#include "event2/buffer.h"
#include <event2/keyvalq_struct.h>

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

        string response = "error!";
        int code = 500;
        DfsrrDataSelect *pDs = static_cast<DfsrrDataSelect *>(arg);
        if (pDs != nullptr)
        {
            code = pDs->doRun(paramMap, response);
        }

        struct evbuffer *evb = evbuffer_new();
        evbuffer_add_printf(evb, response.c_str());
        evhttp_send_reply(req, code, "ok", evb);

        if (evb)
        {
            evbuffer_free(evb);
        }
    }


    int DfsrrDataSelect::doRun(const std::map<std::string, std::string> &paramMap, std::string &response)
    {
        json res, data = json::array();
        int code = 200;
        string msg("successful");
        auto modIt = paramMap.find("module");
        if (modIt == paramMap.end() || moduleSet_.find(modIt->second) == moduleSet_.end())
        {
            LOG(WARN, "param error! not found module.");
            res["code"] = 403;
            res["msg"]  = "not found module param!";
            response = res.dump();
            return 403;
        }
        mod_ = modIt->second;

        this->parseCommonParam(paramMap);
        code = this->selectData(data);

        if (code == SERVER_ERROR)
        {
            msg = SERVER_ERROR_MSG;
        }

        res["code"] = code;
        res["data"] = data;
        res["msg"]  = msg;
        response = res.dump();

        return 200;
    }


    void DfsrrDataSelect::parseCommonParam(const std::map<std::string, std::string> &paramMap)
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
                watch_ = "10";
                LOG(WARN, "param watch is not number!, watch: [{}]", watchIt->second);
            }
        }

        metric_ = "*";
        auto metricIt = paramMap.find("metric");
        if (metricIt != paramMap.end())
        {
            metric_ = metricIt->second;
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
            return SERVER_ERROR;
        }

        return 200;
    }


}; /* dfsrrWebServer namespace end */

