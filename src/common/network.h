// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     network.h
// Author:       dingfang
// CreateDate:   2020-10-23 18:49:31
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 14:29:19
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "common/type.h"

#include <string>


namespace common
{


    struct SockConf_T
    {
        std::string         addr;
        unsigned short int  port;
        int                 timeout;
    };


    class Network
    {
    public:
        Network();
        ~Network();

        Socket_t socket() const { return socket_; }

        int server(SockConf_T &sc);
        int connect(SockConf_T &sc);

        int send(const char *buff, int len);
        int recv(char *buff, int len);

        int setSockOpt();
        int setScokTimeout(int timeout);

    private:
        int createSocket();
        int bind(SockConf_T &sc);
        int listen(int backlog);
        int accept(Network net);
        int close();

    private:
        Socket_t socket_;
    };


}; /* common namespace end */


#endif /* __NETWORK_H__ */
