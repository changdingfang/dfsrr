// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     network.h
// Author:       dingfang
// CreateDate:   2020-10-23 18:49:31
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-27 21:33:02
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "common/type.h"
#include "event2/bufferevent.h"

#include <string>
#include <thread>
#include <memory>


namespace common
{

    typedef void (* pFn)(std::string);

    struct SockConf_T
    {
        std::string         addr;
        unsigned short int  port;
        int                 timeout;
        pFn                 recvData;
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
        int recv(char *buff, int len);

    private:
        static void recvCB(struct bufferevent *bev, void *arg);
        // static void sendCB();
        static void eventCB(struct bufferevent *bev, short event, void *arg);

    private:
        SockConf_T          sc_;
        bufferevent         *bev_;
        struct event_base   *base_ ;
        std::unique_ptr<std::thread> loopPtr_;
    };


}; /* common namespace end */


#endif /* __NETWORK_H__ */
