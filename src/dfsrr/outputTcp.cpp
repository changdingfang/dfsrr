// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputTcp.cpp
// Author:       dingfang
// CreateDate:   2020-10-24 10:53:41
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-26 21:07:22
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrr/outputTcp.h"
#include "nlohmann/json.hpp"


using namespace std;
using namespace common;
using json = nlohmann::json;

namespace dfsrr
{


    std::string TcpPackage_T::serializa()
    {
        char buff[PkgLen] = { 0 };
        ::memcpy(buff, &type, 4);
        ::memcpy(buff + 4, &size, 4);

        std::string s;
        s.assign(buff, PkgLen);
        s += msg;

        return std::move(s);
    }


    bool TcpPackage_T::deserializa(const std::string &data)
    {
        if (data.size() < PkgLen)
        {
            LOG(WARN, "deserializa data failed!");
            return false;
        }
        ::memcpy(&type, data.data(), 4);
        ::memcpy(&size, data.data() + 4, 4);
        msg = data.substr(PkgLen, size);
        
        return true;
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


    OutputTcp::OutputTcp(const std::string &addr, unsigned short port)
    {
        struct SockConf_T sc = { addr, port, 1 };
        try
        {
            netPtr_ = make_shared<Network>(Network());
        }
        catch(...)
        {
            throw;
        }

        if (netPtr_->connect(sc))
        {
            throw("connect server failed!");
        }
    }


    OutputTcp::~OutputTcp()
    {
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
        jdata["module"]    = od.name;
        jdata["data"]      = json::parse(od.data);
        jdata["timestamp"] = od.timestamp;
        string data(jdata.dump(4));
        // string data(jdata.dump());

        TcpPackage_T th { Protocol_E::MODULE_DATA, static_cast<UINT32>(data.size()), data };
        string msg(std::move(th.serializa()));

        if (netPtr_->send(msg.c_str(), msg.size()) <= 0)
        {
            LOG(WARN, "send data failed!");
            return -1;
        }
                
        return 0;
    }


}; /* dfsrr namespace end */
