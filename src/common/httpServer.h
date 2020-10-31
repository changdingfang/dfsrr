// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     httpServer.h
// Author:       dingfang
// CreateDate:   2020-10-31 08:51:50
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-31 11:05:46
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include "common/type.h"
#include "event2/event.h"
#include "event2/http.h"

#include <string>

namespace common
{


    typedef void (* httpCallback)(struct evhttp_request *, void *);

    class HttpServer final
    {
    public:
        HttpServer(UINT16 port, std::string addr = "0.0.0.0");
        ~HttpServer();
        
        int setCallback(std::string path, httpCallback cb, void *arg);
        int setGenCallback(httpCallback cb, void *arg);

        int run();

    private:
        int init();
        int uninit();

    private:
        std::string         addr_;
        UINT16              port_;
        struct evhttp       *http_;
        struct event_base   *base_;
    };


}; /* common namespace end */


#endif /* __HTTP_SERVER_H__ */
