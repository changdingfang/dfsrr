// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     route.h
// Author:       dingfang
// CreateDate:   2020-10-31 10:55:56
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 13:01:59
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "common/httpServer.h"
#include "dfsrrWebServer/dfsrr.h"
#include "event2/http.h"
#include "nlohmann/json.hpp"

#include <string>
#include <memory>

namespace dfsrrWebServer
{


    enum ResponseCode_E
    {
        SuccessCode        = 200,
        ParamErrorCode     = 401,
        ServerErrorCode    = 503,
    };


    class Route final
    {
    public:
        static void helloWorld(struct evhttp_request *req, void *arg);

    public:
        Route(std::shared_ptr<common::HttpServer> &hsp, const nlohmann::json &conf);
        ~Route() = default;
        bool route();
        static std::string getResponseMsg(ResponseCode_E code);
        static void getInput(struct evhttp_request *req, std::string &data);
        static void getParam(const struct evkeyvalq *headers, const char *key, std::string &value);
        static void getCmdType(const struct evhttp_request *req, std::string &cmdtype);

    private:
        std::shared_ptr<common::HttpServer> httpServerPtr_;
        std::shared_ptr<DfsrrDataSelect>    dsPtr_;
    };


}; /* dfsrrWebServer namespace end */


#endif /* __ROUTE_H__ */
