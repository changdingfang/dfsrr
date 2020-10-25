// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     network.cpp
// Author:       dingfang
// CreateDate:   2020-10-23 18:54:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-25 22:24:07
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "network.h"

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

namespace common
{


    Network::Network()
    {
        if (this->createSocket() != 0 || this->setSockOpt() != 0)
        {
            throw("create net failed!");
        }
    }


    Network::~Network()
    {
        this->close();
    }


    int Network::server(SockConf_T &sc)
    {
        this->bind(sc);
        this->listen(10);

        return 0;
    }


    int Network::connect(SockConf_T &sc)
    {
        if (sc.timeout) 
        {
            // this->setScokTimeout(sc.timeout);
            struct timeval timeset;
            timeset.tv_sec = 1;
            timeset.tv_usec = 0;
            if (::setsockopt(this->socket()
                        , SOL_SOCKET
                        , SO_SNDTIMEO
                        , (char *)(&timeset)
                        , sizeof(timeset)) != 0) 
            {
                LOG(ERROR, "setsockopt error: [{}], [{}]", errno, strerror(errno));
                return -1;
            } 
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family       = AF_INET;
        serverAddr.sin_addr.s_addr  = ::inet_addr(sc.addr.c_str());
        serverAddr.sin_port         = ::htons(sc.port);
        if (::connect(this->socket()
                    , (struct sockaddr *)(&serverAddr)
                    , sizeof(serverAddr)) != 0) 
        {
            if (errno == EINPROGRESS) 
            {
                LOG(ERROR, "server addr: [{}], port: [{}] is connecting", sc.addr, sc.port);
                return -1;
            } 
            LOG(ERROR, "error: [{}], [{}], addr: [{}], port: [{}]", errno, strerror(errno), sc.addr, sc.port);
            return -1;
        } 

        return 0;
    }


    int Network::send(const char *buff, int len)
    {
        int sendLen = 0, size = 0;

        do
        {
            // size = ::send(this->socket(), buff + sendLen, len - sendLen, 0);
            size = ::send(this->socket(), buff + sendLen, len - sendLen, MSG_NOSIGNAL);
            if (size > 0)
            {
                sendLen += size;
            }
        } while (size > 0 && sendLen < len);

        if (size < 0)
        {
            LOG(ERROR, "send data error: [{}], [{}]", errno, strerror(errno));
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
            {
                return size;
            }
        }

        return sendLen;
    }


    int Network::recv(char *buff, int len)
    {
        int size = ::recv(this->socket(), buff, len, 0);

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


    int Network::setSockOpt()
    {
        return 0;
    }


    int Network::setScokTimeout(int timeout)
    {
        return 0;
    }


    int Network::createSocket()
    {
        socket_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (socket_ == INVALID_SOCKET) 
        {
            LOG(ERROR, "create socket error: [{}], [{}]", errno, strerror(errno));
            return -1;
        } 
        return 0;
    }


    int Network::bind(SockConf_T &sc)
    {
        return 0;
    }


    int Network::listen(int backlog)
    {
        return 0;
    }


    int Network::accept(Network net)
    {
        return 0;
    }


    int Network::close()
    {
        return 0;
    }


}; /* common namespace end */
