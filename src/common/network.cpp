// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     network.cpp
// Author:       dingfang
// CreateDate:   2020-10-23 18:54:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-27 21:32:37
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "network.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/util.h"
#include "event2/thread.h"

using namespace std;

namespace common
{


    Network::Network(const SockConf_T &sc)
        : sc_(sc)
          , bev_(nullptr)
          , base_(nullptr)
    {
        LOG(DEBUG, "Network init....");
    }


    Network::~Network()
    {
        this->loopExit();
        LOG(DEBUG, "~Network");

        if (base_)
        {
            LOG(DEBUG, "~Network::free base");
            event_base_free(base_);
            base_ = nullptr;
        }

        if (loopPtr_)
        {
            LOG(DEBUG, "~Network::loop thread join");
            loopPtr_->join();
        }
    }


    int Network::server()
    {
        return 0;
    }


    int Network::loop(Network *pNet)
    {
        LOG(DEBUG, "loop start!");
        if (!pNet || pNet->base_ == nullptr)
        {
            LOG(ERROR, "base is null");
            return -1;
        }
        event_base_dispatch(pNet->base_);
        /// event_base_loop(pNet->base_, EVLOOP_NO_EXIT_ON_EMPTY);
        LOG(DEBUG, "loop end!");

        return 0;
    }


    void Network::loopExit()
    {
        // ::event_base_loopexit(base_, nullptr);
        ::event_base_loopbreak(base_);
    }


    int Network::connect()
    {
        if (::evthread_use_pthreads() != 0)
        {
            LOG(WARN, "use pthread failed!");
            return -1;
        }

        struct event_base *base = ::event_base_new();
        if (base == nullptr)
        {
            LOG(WARN, "network init failed!");
            return -1;
        }

        struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);
        if (bev == nullptr)
        {
            LOG(ERROR, "buffer event is null!");
            return -1;
        }

        struct sockaddr_in si;
        si.sin_family = AF_INET;
        si.sin_port = htons(sc_.port);
        ::inet_aton(sc_.addr.c_str(), &si.sin_addr);

        if (::bufferevent_socket_connect(bev, (struct sockaddr *)&si, sizeof(si)) != 0)
        {
            LOG(ERROR, "connect server failed!");
            return -1;
        }

        ::bufferevent_setcb(bev, Network::recvCB, nullptr, Network::eventCB, (void *)this);
        if (::bufferevent_enable(bev, EV_READ | EV_PERSIST) != 0)
        {
            LOG(ERROR, "enable event failed!");
            return -1;
        }

        bev_ = bev;
        base_ = base;

        LOG(DEBUG, "connect successful!");

        loopPtr_ = unique_ptr<std::thread>(new thread(Network::loop, this));

        return 0;
    }


    int Network::send(const char *buff, int len)
    {
        if (bev_ == nullptr)
        {
            LOG(ERROR, "send:: buffer event is null!");
            return -1;
        }
        return ::bufferevent_write(bev_, buff, len);
    }


    int Network::recv(char *buff, int len)
    {
        int size = 0;
        // int size = ::recv(this->socket(), buff, len, 0);

        if (size > 0)
        {
            LOG(DEBUG, "recv data size: [{}]", size);
        }
        else if (size == 0) 
        {
            LOG(INFO, "peer maybe has been closed gracefully");
        } 
        else if (size < 0) 
        {
            if (errno == EAGAIN || errno == EINTR)
                return 0;
            LOG(ERROR, "recv data error: [{}], [{}]", errno, strerror(errno));
        } 

        return size;
    }


    void Network::eventCB(struct bufferevent *bev, short event, void *arg)
    {
        if (event & BEV_EVENT_EOF)
        {
            LOG(WARN, "connection closed");
        }
        else if (event & BEV_EVENT_ERROR)
        {
            LOG(ERROR, "some other error!");
        }
        else if (event & BEV_EVENT_CONNECTED)
        {
            LOG(WARN, "the client has connected to server");
            return ;
        }
        else
        {
            LOG(INFO, "event callback other....");
        }

        Network *pNet = reinterpret_cast<Network *>(arg);
        ::bufferevent_free(pNet->bev_);
        pNet->bev_ = nullptr;
        LOG(DEBUG, "event callback!");
    }


    void Network::recvCB(struct bufferevent *bev, void *arg)
    {
        LOG(INFO, "recv....!");
        char data[2048] = { 0 };
        size_t len = bufferevent_read(bev, data, sizeof(data));

        Network *pNet = reinterpret_cast<Network *>(arg);
        if (pNet && pNet->sc_.recvData != nullptr)
        {
            pNet->sc_.recvData(string(data, len));
        }
        else
        {
            LOG(WARN, "recvCB: arg is null");
        }

        return ;
    }


}; /* common namespace end */
