// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     memory.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 19:20:12
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 18:50:47
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "memory.h"
#include "fileHelper.h"

#include <stdio.h>

using namespace std;

namespace mod
{

    std::map<std::string, double> Memory::collect()
    {
        this->readMemoryInfo();
        this->calculate();

        lastMem_ = currMem_;

        return std::move(memMetric_);
    }


    int Memory::readMemoryInfo()
    {
        MemoryStat_T ms;
        FileHelper f(MemoryInfoFIle);
        if (f.ifstream().fail())
        {
            LOG(WARN, "open memory info file failed!");
            return -1;
        }

        string line;
        while (getline(f.ifstream(), line))
        {
            if (!line.compare(0, 9, "MemTotal:"))
            {
                ::sscanf(line.c_str() + 9, "%llu", &currMem_.total);
            }
            else if (!line.compare(0, 8, "MemFree:"))
            {
                ::sscanf(line.c_str() + 8, "%llu", &currMem_.free);
            }
            else if (!line.compare(0, 8, "Buffers:"))
            {
                ::sscanf(line.c_str() + 8, "%llu", &currMem_.buffers);
            }
            else if (!line.compare(0, 7, "Cached:"))
            {
                ::sscanf(line.c_str() + 7, "%llu", &currMem_.cached);
            }
            else if (!line.compare(0, 7, "Active:"))
            {
                ::sscanf(line.c_str() + 7, "%llu", &currMem_.active);
            }
            else if (!line.compare(0, 9, "Inactive:"))
            {
                ::sscanf(line.c_str() + 9, "%llu", &currMem_.inactive);
            }
            else if (!line.compare(0, 5, "Slab:"))
            {
                ::sscanf(line.c_str() + 5, "%llu", &currMem_.slab);
            }
            else if (!line.compare(0, 11, "SwapCached:"))
            {
                ::sscanf(line.c_str() + 11, "%llu", &currMem_.swapCached);
            }
            else if (!line.compare(0, 10, "SwapTotal:"))
            {
                ::sscanf(line.c_str() + 10, "%llu", &currMem_.swapTotal);
            }
            else if (!line.compare(0, 9, "SwapFree:"))
            {
                ::sscanf(line.c_str() + 9, "%llu", &currMem_.swapFree);
            }
            else if (!line.compare(0, 13, "Committed_AS:"))
            {
                ::sscanf(line.c_str() + 13, "%llu", &currMem_.committedAS);
            }
        }

        return 0;
    }


    int Memory::calculate()
    {
        memMetric_.clear();
        memMetric_["free"]      = currMem_.free << 10;
        auto usedIt = memMetric_.insert(make_pair("used", (currMem_.total - currMem_.free - currMem_.buffers - currMem_.cached) << 10));
        memMetric_["buff"]      = currMem_.buffers << 10;
        memMetric_["cache"]     = currMem_.cached << 10;
        memMetric_["total"]     = currMem_.total << 10;
        if (currMem_.total != 0)
        {
            memMetric_["util"]      = usedIt.first->second * 100.0 / (currMem_.total << 10);
        }
        else
        {
            memMetric_["util"]      = 0.0;
        }


        // memMetric_["active"]    = currMem_.inactive;
        // memMetric_["inactive"]  = currMem_.inactive;
        // memMetric_["sTotal"]    = currMem_.swapTotal
        // memMetric_["sCache"]    = currMem_.swapCached;
        // memMetric_["sFree"]     = currMem_.swapFree;
        // memMetric_["slab"]      = currMem_.slab;
        // memMetric_["com"]       = currMem_.committedAS;
    }



}; /* mod namespace end */
