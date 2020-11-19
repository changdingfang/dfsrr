// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrWebServer.h
// Author:       dingfang
// CreateDate:   2020-10-30 21:27:59
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-19 20:41:03
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_WEB_SERVER_H__
#define __DFSRR_WEB_SERVER_H__

#include "nlohmann/json.hpp"
#include "common/httpServer.h"
#include "dfsrrWebServer/route.h"

#include <memory>

namespace dfsrrWebServer
{


    class WebServer final
    {
    public:
        explicit WebServer(const nlohmann::json &json);
        ~WebServer();

        bool route();
        int run();

    private:
        bool parseConfig(const nlohmann::json &confJson);

    private:
        std::shared_ptr<common::HttpServer>  httpServerPtr_;
        std::unique_ptr<Route> routePtr_;
        std::string rootDir_;
    };


}; /* dfsrrWebServer namespace end */


#endif /* __DFSRR_WEB_SERVER_H__ */
