// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     load.cpp
// Author:       dingfang
// CreateDate:   2020-10-15 21:17:08
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 21:34:32
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "load.h"
#include "fileHelper.h"

using namespace std;

namespace mod
{


    CollectData_T Load::collect()
    {
        CollectData_T cd;
        this->readLoad();

        Data_T data;
        data.modStatVec.push_back({ "load1", currLoad_.load1 });
        data.modStatVec.push_back({ "load5", currLoad_.load5 });
        data.modStatVec.push_back({ "load15", currLoad_.load15 });
        data.modStatVec.push_back({ "runq", currLoad_.nrRuning });
        data.modStatVec.push_back({ "plit", currLoad_.nrThreads});
        cd.dataVec.push_back(data);

        return std::move(cd);
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

        return 0;
    }


}; /* mod namespace end */
