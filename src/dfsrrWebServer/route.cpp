// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     route.cpp
// Author:       dingfang
// CreateDate:   2020-10-31 10:59:19
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-27 19:16:45
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

    static const struct table_entry 
    {
        const char *extension;
        const char *content_type;
    } content_type_table[] = {
        { "txt", "text/plain" },
        { "c", "text/plain" },
        { "h", "text/plain" },
        { "html", "text/html" },
        { "htm", "text/html" },
        { "css", "text/css" },
        { "gif", "image/gif" },
        { "jpg", "image/jpg" },
        { "jpeg", "image/jpeg" },
        { "png", "image/png" },
        { "pdf", "application/pdf" },
        { "ps", "application/postscript" },
        { NULL, NULL },
    };

    static const char* guess_content_type(const char *path)
    {
        const char *last_period, *extension;
        const struct table_entry *ent;
        last_period = strrchr(path, '.');
        if (!last_period || strchr(last_period, '/'))
        {
            goto not_found;
        }

        extension = last_period + 1;
        for (ent = &content_type_table[0]; ent->extension; ++ent)
        {
            if (!evutil_ascii_strcasecmp(ent->extension, extension))
            {
                return ent->content_type;
            }
        }

not_found:
        return "application/misc";
    }


    Route::Route(std::shared_ptr<common::HttpServer> &hsp, const nlohmann::json &conf)
        : httpServerPtr_(hsp)
          , rootdir_("")
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


    bool Route::route(const string rootdir)
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

        if (rootdir.empty())
        {
            LOG(WARN, "not found root dir!");
            return true;
        }

        rootdir_ = rootdir;
        LOG(INFO, "root dir: [{}]", rootdir_);
        httpServerPtr_->setGenCallback(Route::document, 
                static_cast<void *>(const_cast<char *>(rootdir_.c_str())));

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


    void Route::document(struct evhttp_request *req, void *arg)
    {
        const char *docroot = static_cast<const char *>(arg);
        string cmdtype;
        Route::getCmdType(req, cmdtype);
        if (cmdtype != "GET")
        {
            LOG(WARN, "request not GET, type: [{}]", cmdtype);
            return;
        }

        string uri = ::evhttp_request_get_uri(req);
        LOG(INFO, "Got a GET request for <{}>", uri);

        /* Decode the URI */
        shared_ptr<struct evhttp_uri> decoded(::evhttp_uri_parse(uri.c_str())
                , [](struct evhttp_uri *p) { if (p) ::evhttp_uri_free(p); });
        if (!decoded)
        {
            LOG(ERROR, "It's not a good URI, Sneding BADREQUEST");
            ::evhttp_send_error(req, HTTP_BADREQUEST, 0);
            return;
        }

        const char *path = ::evhttp_uri_get_path(decoded.get());
        LOG(DEBUG, "path: [{}]", path);
        if (!path || !strcmp("/", path))
        {
            path = "/index.html";
        }

        /* We need to decode it, to see what path the user really wanted */
        char *decodedPath = ::evhttp_uridecode(path, 0, nullptr);
        if (decodedPath == nullptr || ::strstr(decodedPath, ".."))
        {
            ::evhttp_send_error(req, 404, "Document was not found");
            LOG(ERROR, "decoded path error! path: [{}]", decodedPath);
            return ;
        }

        string wholePath;
        wholePath.append(docroot).append("/").append(decodedPath);

        struct stat st;
        if (::stat(wholePath.c_str(), &st) < 0)
        {
            ::evhttp_send_error(req, 404, "Document was not found");
            LOG(ERROR, "whole path stat error! wholePath: [{}]", wholePath);
            return ;
        }

        shared_ptr<struct evbuffer> evb(::evbuffer_new()
                , [](struct evbuffer *p) { if (p) ::evbuffer_free(p); });

        if (S_ISREG(st.st_mode))
        {
            /* Otherwise it's a file; and it to the buffer to get send via sendfile */
            shared_ptr<int> fd(new int(-1), [](int *p) { if (p && *p > 0) { ::close(*p); }});
            if ((*fd = ::open(wholePath.c_str(), O_RDONLY)) < 0)
            {
                LOG(ERROR, "open file error! file: [{}]", wholePath);
                ::evhttp_send_error(req, 404, "Document was not found");
                return ;
            }

            if (::fstat(*fd, &st) < 0)
            {
                /* Make sure the length still matches, now that we opened the file :/ */
                LOG(ERROR, "fstat error! fd: [{}]", *fd);
                ::evhttp_send_error(req, 404, "Document was not found");
                return ;
            }
            const char *type = guess_content_type(decodedPath);
            ::evhttp_add_header(::evhttp_request_get_output_headers(req)
                    , "Content-Type", type);
            ::evbuffer_add_file(evb.get(), *fd, 0, st.st_size);
            ::evhttp_send_reply(req, 200, "OK", evb.get());
            return ;
        }
        ::evhttp_send_error(req, 404, "not found");
    }


}; /* dfsrrWebServer namespace end */
