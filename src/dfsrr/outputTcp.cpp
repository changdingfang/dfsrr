// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputTcp.cpp
// Author:       dingfang
// CreateDate:   2020-10-24 10:53:41
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 20:16:37
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/common.h"
#include "dfsrr/outputTcp.h"
#include "nlohmann/json.hpp"


using namespace std;
using namespace common;
using namespace dfsrrProtocol;
using json = nlohmann::json;

namespace dfsrr
{


    OutputTcp::OutputTcp(const std::string &addr, unsigned short port)
    {
        struct SockConf_T sc;
        sc.addr     = addr;
        sc.port     = port;
        sc.recvData = nullptr;
        sc.arg      = this;
        sc.timeout  = 20;
        sc.timeoutFunc = nullptr;
        try
        {
            LOG(DEBUG, "OutputTcp ...");
            netPtr_ = unique_ptr<Network>(new Network(sc));
            if (netPtr_->connect())
            {
                LOG(WARN, "connect failed!");
                throw("connect server failed!");
            }
            common::gethostname(li_.hostname);
            common::getip(li_.ipVec);
        }
        catch(...)
        {
            LOG(WARN, "OutputTcp: init network failed!");
            throw;
        }
    }


    OutputTcp::~OutputTcp()
    {
        LOG(DEBUG, "~OutputTcp");
    }


    int OutputTcp::convert(const mod::CollectData_T &cd, UINT64 timestamp,  std::string &data)
    {
        json jdataArr;
        for (const auto &data : cd.dataVec)
        {
            json jdata;
            for (const auto &modStat : data.modStatVec)
            {
                jdata[modStat.key] = modStat.value;
            }

            for (const auto &tag : data.modStatTagVec)
            {
                jdata[tag.key] = tag.value;
            }
            jdataArr.push_back(jdata);
        }

        // data = std::move(jdataArr.dump(4)); /* 4个空格的格式化 */
        data = std::move(jdataArr.dump()); /* 紧凑型格式化 */

        return 0;
    }


    int OutputTcp::send(const OutputData_T &od)
    {
        LOG(DEBUG, "output tcp send");
        json jdata;
        jdata["module"]     = od.name;
        jdata["data"]       = json::parse(od.data);
        jdata["timestamp"]  = od.timestamp;
        jdata["hostname"]   = li_.hostname;
        jdata["ip"]         = li_.ipVec;
        string data(jdata.dump(4));
        // string data(jdata.dump());

        TcpPackage_T th { Protocol_E::MODULE_DATA, static_cast<UINT32>(data.size()), data };
        string msg(std::move(th.serializa()));

        if (netPtr_->send(msg.c_str(), msg.size()) < 0)
        {
            LOG(WARN, "send data failed!");
            return -1;
        }
                
        return 0;
    }


}; /* dfsrr namespace end */
