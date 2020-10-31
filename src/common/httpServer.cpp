// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     httpServer.cpp
// Author:       dingfang
// CreateDate:   2020-10-31 08:54:13
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-31 11:41:11
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/type.h"
#include "common/httpServer.h"

#include <signal.h>

using namespace std;

namespace common
{


    HttpServer::HttpServer(UINT16 port, string addr)
        : addr_(addr)
          , port_(port)
          , http_(nullptr)
          , base_(nullptr)
    {
        if (this->init() != 0)
        {
            this->uninit();
            LOG(CRITICAL, "init http server failed!");
            throw("init http server failed!");
        }
    }


    HttpServer::~HttpServer()
    {
        this->uninit();
    }


    int HttpServer::init()
    {
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        {
            LOG(ERROR, "signal error, errno[{}], error[{}]", errno, strerror(errno));
            return -1;
        }

        base_ = event_base_new();
        if (!base_)
        {
            LOG(ERROR, "Couldn't create an event_base");
            return -1;
        }

        /* Create a new http oject to handle request */
        http_ = ::evhttp_new(base_);
        if (!http_)
        {
            LOG(ERROR, "Couldn't create evhttp.");
            return -1;
        }

        /* handle_ = ::evhttp_bind_socket_with_handle(http_, addr_.c_str(), port_); */
        /* if (!handle_) */
        if (::evhttp_bind_socket(http_, addr_.c_str(), port_) != 0)
        {
            LOG(ERROR, "Couldn't bind to port[{}], addr: [{}]", port_, addr_);
            return -1;
        }

        return 0;
    }


    int HttpServer::uninit()
    {
        if(http_)
        {
            evhttp_free(http_);
            http_ = nullptr;
        }

        if (base_)
        {
            event_base_free(base_);
            base_ = nullptr;
        }

        return 0;
    }


    int HttpServer::setCallback(string path, httpCallback cb, void *arg)
    {
        /* 特定api回调函数设置 */
        return http_ ? ::evhttp_set_cb(http_, path.c_str(), cb, arg) : -1;
    }


    int HttpServer::setGenCallback(httpCallback cb, void *arg)
    {
        /* 通用请求回调函数设置 */
        if (!http_)
        {
            LOG(ERROR, "set gen callback failed!");
            return -1;
        }

        ::evhttp_set_gencb(http_, cb, arg);

        return 0;
    }


    int HttpServer::run()
    {
        return event_base_dispatch(base_);
    }


}; /* common namespace end */
