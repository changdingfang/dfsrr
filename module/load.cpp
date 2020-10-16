// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     load.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 21:17:08
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 19:33:43
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "load.h"
#include "fileHelper.h"

using namespace std;

namespace mod
{


    std::map<std::string, double> Load::collect()
    {
        this->readLoad();

        return std::move(loadMetric_);
    }


    int Load::readLoad()
    {
        FileHelper f(LoadFile);
        if (f.ifstream().fail())
        {
            LOG(WARN, "open load file failed!");
            return -1;
        }

        f.ifstream() >> currLoad_.load1 
            >> currLoad_.load5 
            >> currLoad_.load15 
            >> currLoad_.nrRuning;
        f.ifstream().get();
        f.ifstream() >> currLoad_.nrThreads;

        loadMetric_["load1"]    = currLoad_.load1;
        loadMetric_["load5"]    = currLoad_.load5;
        loadMetric_["load15"]   = currLoad_.load15;
        loadMetric_["runq"]     = currLoad_.nrRuning;
        loadMetric_["plit"]     = currLoad_.nrThreads;

        return 0;
    }


}; /* mod namespace end */
