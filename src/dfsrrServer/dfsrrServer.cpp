// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrServer.cpp
// Author:       dingfang
// CreateDate:   2020-10-26 21:52:34
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 08:31:44
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#include "dfsrrServer/dfsrrServer.h"
#include "common/common.h"

#include <unistd.h>

using namespace std;
using namespace common;
using json = nlohmann::json;

namespace dfsrrServer
{


    DfsrrServer::DfsrrServer(json conf)
        : stop_(false)
          , config_()
    {
        if (!this->parseConfig(conf))
        {
            throw ("parse config error!");
        }

        try
        {
            struct SockConf_T sc = { config_.addr, config_.port, 1, DfsrrServer::recvData, (void *)this };
            netPtr_ = unique_ptr<Network>(new Network(sc));
            if (netPtr_->server())
            {
                LOG(WARN, "init server failed!");
                throw("connect server failed!");
            }
        }
        catch (...)
        {
            LOG(WARN, "init server failed!");
            throw;
        }
    }


    DfsrrServer::~DfsrrServer()
    {
        this->stop();
    }


    void DfsrrServer::run()
    {
        while (!this->isStop())
        {
            ::usleep(10 * 1000);
        }
    }


    bool DfsrrServer::parseConfig(const json &confJson)
    {
        auto serverIt = confJson.find("server");
        if (serverIt == confJson.end())
        {
            LOG(CRITICAL, "not found server config");
            return false;
        }

        try
        {
            config_.addr = (*serverIt).at("addr");
            config_.port = (*serverIt).at("port");
        }
        catch(...)
        {
            LOG(CRITICAL, "parse server config failed!");
            return false;
        }

        return true;
    }


    void DfsrrServer::recvData(string data, string key, void *arg)
    {
        DfsrrServer *pDfss = static_cast<DfsrrServer *>(arg);
        if (pDfss == nullptr)
        {
            LOG(WARN, "DfsrrServer ptr is null");
            return ;
        }

        auto it = pDfss->cliInfoMap_.find(key);
        if (it == pDfss->cliInfoMap_.end())
        {
            ClientInfo_T ci;
            ci.lastTime = ::time(nullptr);
            auto resIt = pDfss->cliInfoMap_.insert(make_pair(key, ci));
            if (!resIt.second)
            {
                LOG(ERROR, "insert client info failed!");
                return ;
            }
            it = resIt.first;
        }

        auto &tcpPkg    = it->second.tcpPkg;
        UINT32 &currLen = it->second.currLen;
        const char *currDataPtr = data.data();
        UINT32 currRemainder    = data.size();
        char *head = it->second.head;

        do
        {
            if (currLen < dfsrrProtocol::PkgLen)
            {
                tcpPkg.msg.clear();
                tcpPkg.size = tcpPkg.type = 0;
                ::memcpy(head + currLen, currDataPtr, dfsrrProtocol::PkgLen - currLen);
                if (currRemainder + currLen < dfsrrProtocol::PkgLen)
                {
                    currLen += currRemainder;
                    LOG(INFO, "not found head");
                    break;
                }
                currRemainder -= (dfsrrProtocol::PkgLen - currLen);
                currDataPtr = currDataPtr + dfsrrProtocol::PkgLen - currLen;
                currLen     = dfsrrProtocol::PkgLen;
                tcpPkg.type = *(UINT32 *)head;
                tcpPkg.size = *(UINT32 *)(head + 4);
            }

            UINT32 length = tcpPkg.size - currLen + dfsrrProtocol::PkgLen;
            UINT32 availableLen = currRemainder >= length ? length : currRemainder;

            tcpPkg.msg += string(currDataPtr, availableLen);
            currDataPtr     += availableLen;
            currRemainder   -= availableLen;
            currLen         = (currLen + availableLen) % (tcpPkg.size + dfsrrProtocol::PkgLen);

            LOG(DEBUG, "data size: [{}], curr len: [{}], msg size: [{}]", data.size(), currLen, tcpPkg.msg.size());

            if (tcpPkg.msg.size() == tcpPkg.size)
            {
                /* */
                LOG(DEBUG, "msg: [{}]", tcpPkg.msg);
                LOG(DEBUG, "type: [{}], size: [{}]", tcpPkg.type, tcpPkg.size);
            }
        } while (currRemainder > 0);
    }


}; /* dfsrrServer namespace end */
