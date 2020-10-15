// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     tcp.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 18:53:10
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 21:14:40
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "tcp.h"
#include "fileHelper.h"

#include <stdio.h>

using namespace std;

namespace mod
{


    std::map<std::string, double> Tcp::collect()
    {
        this->readSnmp();
        this->calculate();

        lastTcp_ = currTcp_;

        return std::move(tcpMertic_);
    }


    int Tcp::readSnmp()
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
            if (!line.compare(0, 4, "Tcp:"))
            {
                if (isKey)
                {
                    isKey = false;
                }
                else
                {
                    ::sscanf(line.c_str() + 4, "%*u %*u %*u %*d %llu %llu "
                            "%llu %llu %llu %llu %llu %llu %llu %llu",
                            &currTcp_.activeOpens,
                            &currTcp_.passiveOpens,
                            &currTcp_.attemptFails,
                            &currTcp_.estabResets,
                            &currTcp_.currEstab,
                            &currTcp_.inSegs,
                            &currTcp_.outSegs,
                            &currTcp_.retransSegs,
                            &currTcp_.inErrs,
                            &currTcp_.outRsts);
                    currTcp_.timestamp = time(nullptr);
                    break;
                }
            }
        }

        return 0;
    }


    int Tcp::calculate()
    {
        if (lastTcp_.timestamp == 0)
        {
            return 0;
        }

        UINT64 ts = Delta(currTcp_.timestamp, lastTcp_.timestamp);
        tcpMertic_["active"] = Ratio(Delta(currTcp_.activeOpens, lastTcp_.activeOpens), ts);
        tcpMertic_["pasive"] = Ratio(Delta(currTcp_.passiveOpens, lastTcp_.passiveOpens), ts);
        tcpMertic_["iseg"]   = Ratio(Delta(currTcp_.inSegs, lastTcp_.inSegs), ts);
        double outseg = Delta(currTcp_.outSegs, lastTcp_.outSegs);
        tcpMertic_["outseg"] = Ratio(outseg, ts);
        tcpMertic_["EstRes"] = Ratio(Delta(currTcp_.estabResets, lastTcp_.estabResets), ts);
        tcpMertic_["AtmpFa"] = Ratio(Delta(currTcp_.attemptFails, lastTcp_.attemptFails), ts);
        tcpMertic_["CurrEs"] = Ratio(Delta(currTcp_.currEstab, lastTcp_.currEstab), ts);
        double retran = Percent(Delta(currTcp_.retransSegs, lastTcp_.retransSegs), outseg);
        tcpMertic_["retran"] = retran > 100.0 ? 100.0 : retran;

        return 0;
    }


}; /* mod namespace end */
