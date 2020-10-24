// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     output.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 20:15:22
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 15:24:36
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dfsrr/output.h"

using namespace std;

namespace dfsrr
{


    Output::Output()
        : maxSize_(2048)
    {
    }


    int Output::convert(const mod::CollectData_T &cd, UINT64, std::string &data)
    {
        string d("");
        for (const auto &data : cd.dataVec)
        {
            for (const auto &modStat : data.modStatVec)
            {
                d += modStat.key;
                d += " ";
                d += to_string(modStat.value);
                d += " ";
            }

            for (const auto &tag : data.modStatTagVec)
            {
                d += tag.key;
                d += " ";
                d += tag.value;
                d += " ";
            }
            d[d.size() - 1] = ';';
        }

        data = std::move(d);

        return 0;
    }


    int Output::addData(const std::string &name, UINT64 ts, const std::string &data)
    {
        OutputData_T od;
        od.name         = name;
        od.timestamp    = ts;
        od.data         = data;

        std::unique_lock<std::mutex> lock(mutex_);
        if (dataQueue_.size() > maxSize_)
        {
            LOG(WARN, "queue full, discard data!");
            return -1;
        }
        dataQueue_.push(od);

        return 0;
    }


    int Output::sendData()
    {
        OutputData_T od;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (dataQueue_.empty())
            {
                // LOG(DEBUG, "data queue is empty...");
                return 0;
            }
            od = dataQueue_.front();
            dataQueue_.pop();
        }

        return this->send(od);
    }



}; /* dfsrr namespace end */
