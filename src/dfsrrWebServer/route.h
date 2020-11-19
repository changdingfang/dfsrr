// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     route.h
// Author:       dingfang
// CreateDate:   2020-10-31 10:55:56
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-19 20:41:21
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
        static void favicon(struct evhttp_request *req, void *arg);
        static void helloWorld(struct evhttp_request *req, void *arg);
        static void document(struct evhttp_request *req, void *arg);

    public:
        Route(std::shared_ptr<common::HttpServer> &hsp, const nlohmann::json &conf);
        ~Route() = default;
        bool route(const std::string rootdir = "");

        static std::string getResponseMsg(ResponseCode_E code);
        static int getHeaderOfUri(const std::string &uri, std::map<std::string, std::string> &headerMap);
        static int getHeaderOfReq(struct evhttp_request *req, std::map<std::string, std::string> &headerMap);
        static int getHeader(const struct evkeyvalq *headers, std::map<std::string, std::string> &headerMap);
        static void getInput(struct evhttp_request *req, std::string &data);
        static void getParam(const struct evkeyvalq *headers, const char *key, std::string &value);
        static void getCmdType(const struct evhttp_request *req, std::string &cmdtype);

    private:
        std::shared_ptr<common::HttpServer> httpServerPtr_;
        std::shared_ptr<DfsrrDataSelect>    dsPtr_;
        std::string rootdir_;
    };


}; /* dfsrrWebServer namespace end */


#endif /* __ROUTE_H__ */
