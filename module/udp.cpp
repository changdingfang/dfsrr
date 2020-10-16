// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     udp.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 21:17:41
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 19:34:42
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "udp.h"
#include "fileHelper.h"

using namespace std;

namespace mod
{


    std::map<std::string, double> Udp::collect()
    {
        this->readSnmp();
        this->calculate();

        lastUdp_ = currUdp_;

        return std::move(udpMetric_);
    }


    int Udp::readSnmp()
    {
        FileHelper f(NetSnmp);
        if (f.ifstream().fail())
        {
            LOG(WARN, "open snmp file failed!");
            return -1;
        }

        bool isKey = true;
        string line;
        while (getline(f.ifstream(), line))
        {
            if (!line.compare(0, 4, "Udp:"))
            {
                if (isKey)
                {
                    isKey = false;
                }
                else
                {
                    ::sscanf(line.c_str() + 4, "%llu %llu %llu %llu",
                            &currUdp_.inDatagrams,
                            &currUdp_.noPorts,
                            &currUdp_.inErrors,
                            &currUdp_.outDatagrams);
                    currUdp_.timestamp = time(nullptr);
                    break;
                }
            }
        }
        LOG(DEBUG, "read snmp end");

        return 0;
    }


    int Udp::calculate()
    {
        if (lastUdp_.timestamp == 0)
        {
            return 0;
        }

        UINT64 ts = Delta(currUdp_.timestamp, lastUdp_.timestamp);
        udpMetric_["idgm"] = Ratio(Delta(currUdp_.inDatagrams, lastUdp_.inDatagrams), ts);
        udpMetric_["odgm"] = Ratio(Delta(currUdp_.outDatagrams, lastUdp_.outDatagrams), ts);
        udpMetric_["noport"] = Ratio(Delta(currUdp_.noPorts, lastUdp_.noPorts), ts);
        udpMetric_["idmerr"] = Ratio(Delta(currUdp_.inErrors, lastUdp_.inErrors), ts);

        return 0;
    }


}; /* mod namespace end */
