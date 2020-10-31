// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     route.cpp
// Author:       dingfang
// CreateDate:   2020-10-31 10:59:19
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-31 16:05:18
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrrWebServer/route.h"
#include "event2/buffer.h"
#include <event2/keyvalq_struct.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

using namespace std;
using namespace common;

namespace dfsrrWebServer
{


    Route::Route(std::shared_ptr<common::HttpServer> &hsp, const nlohmann::json &conf)
        : httpServerPtr_(hsp)
    { 
        try
        {
            dsPtr_ = make_shared<DfsrrDataSelect>(conf);
        }
        catch (...)
        {
            LOG(ERROR, "init DfsrrDataSelect failed!");
            throw;
        }
    }


    bool Route::route()
    {
        if (!httpServerPtr_)
        {
            LOG(ERROR, "http server ptr is null!");
            return false;
        }
        httpServerPtr_->setCallback("/helloworld", Route::helloWorld, nullptr);
        httpServerPtr_->setCallback("/dfsrr"
                , DfsrrDataSelect::dfsrr
                , static_cast<void *>(dsPtr_.get()));
        
        /* httpServerPtr_->setGenCallback(nullptr, nullptr); */

        return true;
    }


    void Route::getInput(struct evhttp_request *req, string &data)
    {
        struct evbuffer *buf = ::evhttp_request_get_input_buffer(req);
        LOG(DEBUG, "parse input"); 
        while (::evbuffer_get_length(buf))
        {
            int n;
            char cbuf[128];
            n = ::evbuffer_remove(buf, cbuf, sizeof(cbuf));
            if (n > 0)
            {
                data += cbuf;
            }
        }
    }


    void Route::getParam(const struct evkeyvalq *headers, const char *key, string &value)
    {
        const char *p = evhttp_find_header(headers, key);
        if (!p)
        {
            value = "";
            return ;
        }
        value = p;
    }


    void Route::getCmdType(const struct evhttp_request *req, string &cmdtype)
    {
        switch (evhttp_request_get_command(req))
        {
            case EVHTTP_REQ_GET:
                cmdtype = "GET";
                break;
            case EVHTTP_REQ_POST:
                cmdtype = "POST";
                break;
            case EVHTTP_REQ_HEAD:
                cmdtype = "HEAD";
                break;
            case EVHTTP_REQ_PUT:
                cmdtype = "PUT";
                break;
            case EVHTTP_REQ_DELETE:
                cmdtype = "DELETE";
                break;
            case EVHTTP_REQ_OPTIONS:
                cmdtype = "OPTIONS";
                break;
            case EVHTTP_REQ_TRACE:
                cmdtype = "TRACE";
                break;
            case EVHTTP_REQ_CONNECT:
                break;
            case EVHTTP_REQ_PATCH:
                cmdtype = "PATCH";
                break;
            default:
                cmdtype = "unknown";
                break;
        }
    }


    void Route::helloWorld(struct evhttp_request *req, void *arg)
    {
        string cmdtype;
        struct evkeyvalq *headers;
        struct evkeyval *header;

        Route::getCmdType(req, cmdtype);

        LOG(INFO, "Received a {} request for {}\nHeader: ", cmdtype.c_str(), evhttp_request_get_uri(req));

        headers = evhttp_request_get_input_headers(req);
        for (header = headers->tqh_first; header; header = header->next.tqe_next)
        {
            LOG(DEBUG, " {}: {}", header->key, header->value);
        }

        LOG(DEBUG, "=============================="); 
        string data;
        Route::getInput(req, data);
        LOG(DEBUG, "==============================");

        struct evbuffer *evb = nullptr;
        evb = evbuffer_new();
        evbuffer_add_printf(evb, "hello world\n");
        evhttp_send_reply(req, 200, "ok", evb);
        if (evb)
        {
            evbuffer_free(evb);
        }
    }


}; /* dfsrrWebServer namespace end */
