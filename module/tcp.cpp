// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     tcp.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 18:53:10
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 20:05:43
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "tcp.h"
#include "fileHelper.h"

#include <stdio.h>

using namespace std;

namespace mod
{


    CollectData_T Tcp::collect()
    {
        CollectData_T cd;
        this->readSnmp();
        this->calculate(cd);

        lastTcp_ = currTcp_;

        return std::move(cd);
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


    int Tcp::calculate(CollectData_T &cd)
    {
        if (lastTcp_.timestamp == 0)
        {
            return 0;
        }

        UINT64 ts       = Delta(currTcp_.timestamp, lastTcp_.timestamp);
        UINT64 outseg   = Delta(currTcp_.outSegs, lastTcp_.outSegs);
        double retran = Percent(Delta(currTcp_.retransSegs, lastTcp_.retransSegs), outseg);
        Data_T data;
        data.modStatVec.push_back({ "active", Ratio(Delta(currTcp_.activeOpens, lastTcp_.activeOpens), ts) });
        data.modStatVec.push_back({ "pasive", Ratio(Delta(currTcp_.passiveOpens, lastTcp_.passiveOpens), ts) });
        data.modStatVec.push_back({ "iseg", Ratio(Delta(currTcp_.inSegs, lastTcp_.inSegs), ts) });
        data.modStatVec.push_back({ "outseg", Ratio(outseg, ts) });
        data.modStatVec.push_back({ "EstRes", Ratio(Delta(currTcp_.estabResets, lastTcp_.estabResets), ts) });
        data.modStatVec.push_back({ "AtmpFa", Ratio(Delta(currTcp_.attemptFails, lastTcp_.attemptFails), ts) });
        data.modStatVec.push_back({ "CurrEs", Ratio(Delta(currTcp_.currEstab, lastTcp_.currEstab), ts) });
        data.modStatVec.push_back({ "retran", retran > 100.0 ? 100.0 : retran });

        cd.dataVec.push_back(data);

        return 0;
    }


}; /* mod namespace end */
