// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     network.h
// Author:       dingfang
// CreateDate:   2020-10-23 18:49:31
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 08:21:17
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "common/type.h"
#include "event2/bufferevent.h"
#include <event2/listener.h>

#include <string>
#include <thread>
#include <memory>
#include <set>


namespace common
{

    typedef void (* pFn)(std::string, std::string, void *);

    struct SockConf_T
    {
        std::string         addr;
        unsigned short int  port;
        int                 timeout;

        pFn                 recvData;
        void                *arg;
    };


    class Network
    {
    public:
        Network(const SockConf_T &sc);
        ~Network();

        static int loop(Network *pNet);
        void loopExit();

        int server();
        int connect();

        int send(const char *buff, int len);

    private:
        static void listenerCB(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg);
        static void acceptErrorCB(struct evconnlistener *listener, void *ctx);
        static void recvCB(struct bufferevent *bev, void *arg);
        // static void sendCB();
        static void eventCB(struct bufferevent *bev, short event, void *arg);

        bool insertBev(bufferevent *bev);
        bool eraseBev(bufferevent *bev);

    private:
        SockConf_T          sc_;
        std::set<bufferevent *> bevSet_;
        evconnlistener      *listener_;
        struct event_base   *base_ ;
        std::unique_ptr<std::thread> loopPtr_;
    };


}; /* common namespace end */


#endif /* __NETWORK_H__ */
