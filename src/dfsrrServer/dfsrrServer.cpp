// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrServer.cpp
// Author:       dingfang
// CreateDate:   2020-10-26 21:52:34
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-30 21:23:20
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
            struct SockConf_T sc = 
            {
                config_.addr,
                config_.port,
                DfsrrServer::recvData,
                (void *)this,
                config_.timeout,
                DfsrrServer::timeout
            };

            netPtr_ = unique_ptr<Network>(new Network(sc));
            if (netPtr_->server())
            {
                LOG(WARN, "init server failed!");
                throw("connect server failed!");
            }

            if (config_.dbUse)
            {
                LOG(INFO, "use database...");
                s2mPtr_ = unique_ptr<Store2Mysql>(new Store2Mysql(config_.dbinfo));
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
        catch (...)
        {
            LOG(CRITICAL, "parse server config failed!");
            return false;
        }



        auto dbIt = confJson.find("database");
        if (dbIt == confJson.end())
        {
            LOG(WARN, "not found database config, not use database!");
            return true;
        }

        try
        {
            config_.dbUse = true;
            config_.dbinfo.ip       = dbIt->at("addr");
            config_.dbinfo.port     = dbIt->at("port");
            config_.dbinfo.dbName   = dbIt->at("dbname");
            config_.dbinfo.dbUser   = dbIt->at("username");
        }
        catch (json::exception &e)
        {
            LOG(ERROR, "error: [{}]", e.what());
            return false;
        }
        catch (...)
        {
            LOG(ERROR, "database config failed...");
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
            auto resIt = pDfss->cliInfoMap_.insert(make_pair(key, ci));
            if (!resIt.second)
            {
                LOG(ERROR, "insert client info failed!");
                return ;
            }
            it = resIt.first;
        }

        ClientInfo_T &ci = it->second;
        const char *currDataPtr = data.data();
        UINT32 currRemainder    = data.size();

        do
        {
            if (ci.currLen < dfsrrProtocol::PkgLen)
            {
                ci.head.append(currDataPtr, dfsrrProtocol::PkgLen - ci.currLen);
                if (currRemainder + ci.currLen < dfsrrProtocol::PkgLen)
                {
                    ci.currLen += currRemainder;
                    LOG(INFO, "not found head");
                    break;
                }
                currRemainder -= (dfsrrProtocol::PkgLen - ci.currLen);
                currDataPtr = currDataPtr + dfsrrProtocol::PkgLen - ci.currLen;
                ci.currLen     = dfsrrProtocol::PkgLen;
                ci.tcpPkg.type = *(UINT32 *)ci.head.data();
                ci.tcpPkg.size = *(UINT32 *)(ci.head.data() + 4);
                ci.head.clear();
            }

            UINT32 length = ci.tcpPkg.size - ci.currLen + dfsrrProtocol::PkgLen;
            UINT32 availableLen = currRemainder >= length ? length : currRemainder;

            ci.tcpPkg.msg += string(currDataPtr, availableLen);
            currDataPtr     += availableLen;
            currRemainder   -= availableLen;
            ci.currLen         = (ci.currLen + availableLen) % (ci.tcpPkg.size + dfsrrProtocol::PkgLen);

            LOG(DEBUG, "data size: [{}], curr len: [{}], msg size: [{}], remainder: [{}]", data.size(), ci.currLen, ci.tcpPkg.msg.size(), currRemainder);

            if (ci.tcpPkg.msg.size() == ci.tcpPkg.size)
            {
                /* */
                LOG(DEBUG, "msg: [{}]", ci.tcpPkg.msg);
                LOG(DEBUG, "type: [{}], size: [{}]", ci.tcpPkg.type, ci.tcpPkg.size);
                string msg;
                if (pDfss->s2mPtr_)
                {
                    pDfss->s2mPtr_->convert(ci.tcpPkg.msg, msg);
                    pDfss->s2mPtr_->addData(msg);
                }
                ci.tcpPkg.msg.clear();
                ci.tcpPkg.size = ci.tcpPkg.type = 0;
            }
        } while (currRemainder > 0);
    }


    bool DfsrrServer::timeout(string key, void *arg)
    {
        DfsrrServer *pDfss = static_cast<DfsrrServer *>(arg);
        if (pDfss == nullptr)
        {
            LOG(WARN, "dfsrrServer ptr is null!");
            return false;
        }

        auto it = pDfss->cliInfoMap_.find(key);
        if (it == pDfss->cliInfoMap_.end())
        {
            LOG(WARN, "not found in client info map");
            return false;
        }
        if (it->second.timeoutCount++ >= 2)
        {
            LOG(WARN, "time out count: [{}]", it->second.timeoutCount);
            pDfss->cliInfoMap_.erase(it);
            return false;
        }

        LOG(DEBUG, "----time out count: [{}]", it->second.timeoutCount);
        it->second.timeoutCount = 0;

        return true;
    }


}; /* dfsrrServer namespace end */
