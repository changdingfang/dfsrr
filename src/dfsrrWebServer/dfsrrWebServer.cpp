// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrWebServer.cpp
// Author:       dingfang
// CreateDate:   2020-10-30 21:52:05
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-19 20:41:16
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/type.h"
#include "dfsrrWebServer/dfsrrWebServer.h"

using namespace std;
using namespace common;
using json = nlohmann::json;

namespace dfsrrWebServer
{


    WebServer::WebServer(const nlohmann::json &json)
        : rootDir_("")
    {
        if (!this->parseConfig(json))
        {
            throw ("parse config error!");
        }

        try
        {
            routePtr_ = unique_ptr<Route>(new Route(httpServerPtr_, json));
        }
        catch (...)
        {
            LOG(ERROR, "init route failed!");
            throw;
        }
    }


    WebServer::~WebServer()
    {
    }
        

    bool WebServer::parseConfig(const json &confJson)
    {
        auto serverIt = confJson.find("server");
        if (serverIt == confJson.end())
        {
            LOG(CRITICAL, "not found server config");
            return false;
        }

        try
        {
            string addr("0.0.0.0");
            if (serverIt->find("addr") != serverIt->end())
            {
                addr = serverIt->at("addr");
            }
            UINT16 port = serverIt->at("port");
            httpServerPtr_ = make_shared<HttpServer>(port, addr);

            if (serverIt->find("root") != serverIt->end())
            {
                rootDir_ = serverIt->at("root");
            }
        }
        catch (...)
        {
            LOG(CRITICAL, "parse http server config failed!");
            return false;
        }

        return true;
    }


    int WebServer::run()
    {
        if (!routePtr_->route(rootDir_))
        {
            LOG(WARN, "set route failed!");
        }
        return httpServerPtr_->run();
    }


}; /* dfsrrWebServer namespace end */
