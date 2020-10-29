// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrServer.h
// Author:       dingfang
// CreateDate:   2020-10-26 21:51:40
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 20:02:54
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_SERVER_H__
#define __DFSRR_SERVER_H__

#include "common/type.h"
#include "common/network.h"
#include "nlohmann/json.hpp"
#include "dfsrrServer/store2Mysql.h"
#include "dfsrr/protocol.h"

#include <string>
#include <memory>
#include <map>

namespace dfsrrServer
{


    struct ClientInfo_T
    {
        std::string     hostname { "" };
        std::string     ip { "" };
        int             timeoutCount { 0 };

        std::string     head { "" };
        UINT32          currLen  { 0 };
        struct          dfsrrProtocol::TcpPackage_T tcpPkg;
    };

    
    struct DfsrrServerConfig_T
    {
        std::string         addr;
        unsigned short      port;
        UINT32              intv { 10 * 1000 };    /* us */
        int                 timeout { 30 };    /* s */ 

        bool                dbUse { false };
        common::DBInfo_T    dbinfo;
    };


    class DfsrrServer
    {
    public:
        DfsrrServer(nlohmann::json conf);
        ~DfsrrServer();

        inline void stop() { stop_ = true; }
        void run();

        static void recvData(std::string data, std::string key, void *arg);
        static bool timeout(std::string key, void *arg);

    private:
        bool parseConfig(const nlohmann::json &);
        inline bool isStop() { return stop_; }


    private:
        bool stop_;
        DfsrrServerConfig_T  config_;
        std::unique_ptr<common::Network>    netPtr_;
        std::map<std::string, ClientInfo_T> cliInfoMap_;
        std::unique_ptr<Store2Mysql>        s2mPtr_;
    };


}; /* dfsrrServer namespace end */


#endif /* __DFSRR_SERVER_H__ */
