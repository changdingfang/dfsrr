// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     test_httpServer.cpp
// Author:       dingfang
// CreateDate:   2020-11-02 21:14:39
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-02 21:36:14
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/httpServer.h"
#include "event2/buffer.h"
#include "event2/keyvalq_struct.h"

#include <signal.h>

#include <string>

using namespace std;
using namespace common;

void handle(int flag)
{
    exit(0);
}


void helloWorld(struct evhttp_request *req, void *arg)
{
    struct evbuffer *evb = nullptr;
    evb = ::evbuffer_new();
    ::evbuffer_add_printf(evb, "hello world\n");
    ::evhttp_send_reply(req, 200, "ok", evb);
    if (evb)
    {
        ::evbuffer_free(evb);
    }
}


void testHttpServer()
{
    HttpServer httpServer(50000, "0.0.0.0");
    httpServer.setGenCallback(helloWorld, nullptr);
    httpServer.run();
}


int main(void)
{
    dflog::InitLog("./mysqldbtest.log", dflog::loggerOption::CONSOLE);
    dflog::SetLevel(DEBUG);

    ::signal(SIGTERM, handle);
    testHttpServer();

    return 0;
}
