// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrServer.h
// Author:       dingfang
// CreateDate:   2020-10-26 21:51:40
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 17:00:58
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_SERVER_H__
#define __DFSRR_SERVER_H__

#include "common/type.h"
#include "common/network.h"
#include "nlohmann/json.hpp"
#include "dfsrr/protocol.h"

#include <string>
#include <memory>
#include <map>

namespace dfsrrServer
{


    struct ClientInfo_T
    {
        std::string hostname;
        std::string ip;
        UINT32 lastTime { 0 };

        char head[dfsrrProtocol::PkgLen];
        UINT32 currLen  { 0 };
        struct dfsrrProtocol::TcpPackage_T tcpPkg;
    };

    
    struct DfsrrServerConfig_T
    {
        std::string         addr;
        unsigned short      port;
        UINT32  intv { 10 * 1000 };    /* us */
        UINT32 checkTimeout { 10 };    /* s */ 
    };


    class DfsrrServer
    {
    public:
        DfsrrServer(nlohmann::json conf);
        ~DfsrrServer();

        inline void stop() { stop_ = true; }
        void run();

        static void recvData(std::string data, std::string key);

    private:
        bool parseConfig(const nlohmann::json &);
        inline bool isStop() { return stop_; }


    private:
        bool stop_;
        DfsrrServerConfig_T  config_;
        std::unique_ptr<common::Network> netPtr_;
        static std::map<std::string, ClientInfo_T> cliInfoMap_;
    };


}; /* dfsrrServer namespace end */


#endif /* __DFSRR_SERVER_H__ */
