// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     route.cpp
// Author:       dingfang
// CreateDate:   2020-10-31 10:59:19
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-02 20:18:33
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
        httpServerPtr_->setCallback("/favicon.ico", Route::favicon, nullptr);
        httpServerPtr_->setCallback("/helloworld", Route::helloWorld, nullptr);
        httpServerPtr_->setCallback("/dfsrr"
                , DfsrrDataSelect::dfsrr
                , static_cast<void *>(dsPtr_.get()));
        
        httpServerPtr_->setGenCallback(Route::helloWorld, nullptr);

        return true;
    }


    std::string Route::getResponseMsg(ResponseCode_E code)
    {
        string msg;
        switch (code)
        {
            case SuccessCode:       msg = "successful";         break;
            case ParamErrorCode:    msg = "param error!";       break;
            case ServerErrorCode:   msg = "server internal error!"; break;
            default: msg = "server internal error!"; break;
        }

        return std::move(msg);
    }


    int Route::getHeaderOfUri(const string &uri, std::map<std::string, std::string> &headerMap)
    {
        struct evkeyvalq headers;
        if (::evhttp_parse_query(uri.c_str(), &headers) != 0)
        {
            LOG(WARN, "parse param failed!");
            return -1;
        }
        Route::getHeader(&headers, headerMap);
        evhttp_clear_headers(&headers);

        return 0;
    }


    int Route::getHeaderOfReq(struct evhttp_request *req, std::map<string, string> &headerMap)
    {
        const struct evkeyvalq *headers = ::evhttp_request_get_input_headers(req);
        return Route::getHeader(headers, headerMap);
    }


    int Route::getHeader(const struct evkeyvalq *headers, std::map<std::string, std::string> &headerMap)
    {
        struct evkeyval *header = headers->tqh_first; 
        while (header)
        {
            headerMap[header->key] = header->value;
            LOG(DEBUG, " {}: {}", header->key, header->value);
            header = header->next.tqe_next;
        }

        return 0;
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
        switch (::evhttp_request_get_command(req))
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


    void Route::favicon(struct evhttp_request *req, void *arg)
    {
        LOG(CRITICAL, "FAVICON......");
        string cmdtype;
        Route::getCmdType(req, cmdtype);
        LOG(INFO, "Received a {} request for {}", cmdtype.c_str(), ::evhttp_request_get_uri(req));

        map<string, string> headerMap;
        Route::getHeaderOfReq(req, headerMap);

        LOG(DEBUG, "=============================="); 
        string data;
        Route::getInput(req, data);
        LOG(DEBUG, "==============================");

        ::evhttp_add_header(::evhttp_request_get_output_headers(req), "Content-Type", "image/png");

        int fd = -1;
        if ((fd = ::open("../imgs/favicon.jpg", O_RDONLY)) < 0)
        {
            LOG(ERROR, "open favicon file failed!");
            ::evhttp_send_error(req, 404, "favicon file was not found!");
            return ;
        }

        struct stat st;
        if (::fstat(fd, &st) < 0)
        {
            LOG(ERROR, "get file stat info failed!");
            ::evhttp_send_error(req, 404, "get file stat info failed!");
            return ;
        }

        struct evbuffer *evb = nullptr;
        evb = ::evbuffer_new();
        ::evbuffer_add_file(evb, fd, 0, st.st_size);
        ::evhttp_send_reply(req, SuccessCode, "ok", evb);
        if (evb)
        {
            ::evbuffer_free(evb);
        }
    }


    void Route::helloWorld(struct evhttp_request *req, void *arg)
    {
        string cmdtype;
        Route::getCmdType(req, cmdtype);
        LOG(INFO, "Received a {} request for {}", cmdtype.c_str(), ::evhttp_request_get_uri(req));

        map<string, string> headerMap;
        Route::getHeaderOfReq(req, headerMap);

        LOG(DEBUG, "=============================="); 
        string data;
        Route::getInput(req, data);
        LOG(DEBUG, "==============================");

        struct evbuffer *evb = nullptr;
        evb = ::evbuffer_new();
        ::evbuffer_add_printf(evb, "hello world\n");
        ::evhttp_send_reply(req, SuccessCode, "ok", evb);
        if (evb)
        {
            ::evbuffer_free(evb);
        }
    }


}; /* dfsrrWebServer namespace end */
