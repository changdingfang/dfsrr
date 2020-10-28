// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     network.cpp
// Author:       dingfang
// CreateDate:   2020-10-23 18:54:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 20:32:55
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "network.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "event.h"
#include "event2/buffer.h"
#include "event2/util.h"
#include "event2/thread.h"

using namespace std;

namespace common
{


    Network::Network(const SockConf_T &sc)
        : sc_(sc)
          , listener_(nullptr)
          , base_(nullptr)
    {
        LOG(DEBUG, "Network init....");
        if (::evthread_use_pthreads() != 0)
        {
            LOG(WARN, "use pthread failed!");
            throw("use pthread failed!");
        }

        struct event_base *base = ::event_base_new();
        if (base == nullptr)
        {
            LOG(WARN, "network init failed!");
            throw( "network init failed!");
        }
        base_ = base;
    }


    Network::~Network()
    {
        this->loopExit();
        LOG(DEBUG, "~Network");

        if (listener_)
        {
            evconnlistener_free(listener_);
            listener_ = nullptr;
        }

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


    int Network::server()
    {
        struct event_base *base = base_;

        int backlog = 10;

        struct sockaddr_in si;
        ::memset(&si, 0x00, sizeof(struct sockaddr_in));
        si.sin_family   = AF_INET;
        si.sin_port     = htons(sc_.port);

        string method = event_base_get_method(base);
        LOG(INFO, "method: [{}]", method);

        evconnlistener *listener = ::evconnlistener_new_bind(base
                , Network::listenerCB
                , (void *)this
                , LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_THREADSAFE
                , backlog
                , (struct sockaddr *)&si
                , sizeof(struct sockaddr_in));

        if (listener == nullptr)
        {
            LOG(ERROR, "evconnlistener new bind failed!");
            return -1;
        }

        ::evconnlistener_set_error_cb(listener, Network::acceptErrorCB);

        listener_ = listener;

        LOG(DEBUG, "server init successful");

        loopPtr_ = unique_ptr<std::thread>(new thread(Network::loop, this));

        return 0;
    }


    int Network::connect()
    {
        struct event_base *base = base_;

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

        this->insertBev(bev);
        base_ = base;

        LOG(DEBUG, "connect successful!");

        loopPtr_ = unique_ptr<std::thread>(new thread(Network::loop, this));

        return 0;
    }


    int Network::send(const char *buff, int len)
    {
        if (bevSet_.size() == 0)
        {
            LOG(ERROR, "send:: buffer event is null!");
            return -1;
        }

        for (auto bev : bevSet_)
        {
            if (::bufferevent_write(bev, buff, len) != 0)
            {
                LOG(WARN, "buffer event write buff failed!");
            }
        }

        return 0;
    }


    void Network::listenerCB(evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg)
    {
        LOG(DEBUG, "accept a client [{}]", fd);

        Network *pNet = reinterpret_cast<Network *>(arg);
        event_base *base = pNet->base_;
        // event_base *base = evconnlistener_get_base(listener);

        bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        bufferevent_setcb(bev, Network::recvCB, nullptr, eventCB, arg);
        bufferevent_enable(bev, EV_READ | EV_PERSIST | EV_ET);

        pNet->insertBev(bev);

        return ;
    }


    void Network::acceptErrorCB(struct evconnlistener *listener, void *ctx)
    {
        struct event_base *base = ::evconnlistener_get_base(listener);
        int err = EVUTIL_SOCKET_ERROR();

        LOG(ERROR, "error: [{}], [{}], shutdown!", err, evutil_socket_error_to_string(err));

        ::event_base_loopexit(base, nullptr);
    }


    void Network::recvCB(struct bufferevent *bev, void *arg)
    {
        LOG(DEBUG, "recv....!");

        Network *pNet = reinterpret_cast<Network *>(arg);
        if (pNet && pNet->sc_.recvData != nullptr)
        {
            evbuffer *inputBuffer = ::bufferevent_get_input(bev);
            if (inputBuffer == nullptr)
            {
                LOG(WARN, "input buffer is null");
                return ;
            }

            char data[2048] = { 0 };
            do
            {
                size_t len = bufferevent_read(bev, data, sizeof(data));
                pNet->sc_.recvData(string(data, len), to_string(std::hash<bufferevent *>{}(bev)));
            } while (::evbuffer_get_length(inputBuffer) > 0);
        }
        else
        {
            LOG(WARN, "recvCB: arg is null");
        }

        return ;
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
        ::bufferevent_free(bev);
        pNet->eraseBev(bev);
        LOG(DEBUG, "event callback!");
    }


    bool Network::insertBev(bufferevent *bev)
    {
        if (bevSet_.find(bev) != bevSet_.end())
        {
            return false;
        }
        bevSet_.insert(bev);

        return true;
    }


    bool Network::eraseBev(bufferevent *bev)
    {
        auto it = bevSet_.find(bev);
        if (it == bevSet_.end())
        {
            LOG(INFO, "bev already erase!");
            return false;
        }
        LOG(INFO, "bevSet find the bev.");
        bevSet_.erase(it);

        return true;
    }


}; /* common namespace end */
