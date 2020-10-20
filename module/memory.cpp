// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     memory.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 19:20:12
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 20:12:21
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "memory.h"
#include "fileHelper.h"

#include <stdio.h>

using namespace std;

namespace mod
{

    CollectData_T Memory::collect()
    {
        CollectData_T cd;
        cd.moduleName = "memory";
        this->readMemoryInfo();
        this->calculate(cd);

        return std::move(cd);
    }


    int Memory::readMemoryInfo()
    {
        FileHelper f(MemoryInfoFIle);
        if (f.ifstream().fail())
        {
            LOG(WARN, "open memory info file failed!");
            return -1;
        }

        string line;
        currMem_.clear();
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
            else if (!line.compare(0, 13, "MemAvailable:"))
            {
                ::sscanf(line.c_str() + 13, "%llu", &currMem_.available);
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


    int Memory::calculate(CollectData_T &cd)
    {
        
        UINT64 used = 0;
        if (currMem_.available == 0)
        {
            used = currMem_.total - currMem_.free - currMem_.buffers - currMem_.cached;
        }
        else
        {
            used = currMem_.total - currMem_.available;
        }

        Data_T data;
        data.modStatVec.push_back({ "free", static_cast<double>(currMem_.free << 10) });
        data.modStatVec.push_back({ "used", static_cast<double>(used << 10) });
        data.modStatVec.push_back({ "buff", static_cast<double>(currMem_.buffers << 10) });
        data.modStatVec.push_back({ "cache", static_cast<double>(currMem_.cached << 10) });
        data.modStatVec.push_back({ "total", static_cast<double>(currMem_.total << 10) });
        data.modStatVec.push_back({ "util", static_cast<double>(Percent(used, currMem_.total)) });

        cd.dataVec.push_back(data);

        return 0;
    }


}; /* mod namespace end */
