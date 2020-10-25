// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputTcp.cpp
// Author:       dingfang
// CreateDate:   2020-10-24 10:53:41
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-25 22:38:51
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrr/outputTcp.h"
#include "nlohmann/json.hpp"


using namespace std;
using namespace common;
using json = nlohmann::json;

namespace dfsrr
{


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
        // string data(jdata.dump(4));
        string data(jdata.dump());

        if (netPtr_->send(data.c_str(), data.size()) <= 0)
        {
            LOG(WARN, "send data failed!");
            return -1;
        }
                
        return 0;
    }


}; /* dfsrr namespace end */
