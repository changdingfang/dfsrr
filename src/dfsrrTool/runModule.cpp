// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     runModule.cpp
// Author:       dingfang
// CreateDate:   2020-10-14 18:41:02
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 21:30:57
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrrModule.h"
#include "runModule.h"

#include <unistd.h>

#include <string>

using namespace std;
using namespace mod;

namespace dfssrTool
{


    RunMode::RunMode(ModuleConfig_T &config)
        : config_(config)
    { }


    int RunMode::run()
    {
        LOG(DEBUG, "Run Mode run...");
        switch (config_.printMode)
        {
            case PRINT_LAST_N_DATA: this->printLastNData(); break;
            case PRINT_SPECIL_TIME: this->printDateDate();  break;
            case PRINT_LIVE:        this->printLiveData();  break;
            default: printf("not found mod\n"); exit(-1);
        }

        return 0;
    }


    void RunMode::printLastNData()
    {
        LOG(DEBUG, "print last n data");
    }


    void RunMode::printDateDate()
    {
        LOG(DEBUG, "print date data");
    }


    void RunMode::printLiveData()
    {
        LOG(DEBUG, "print live data...");
        DfSrrModule dfsrr(config_.name);
        dfsrr.collect();
        ::sleep(config_.interval);

        string head, optLine;
        CollectData_T cd;
        cd = std::move(dfsrr.collect());
        for (const auto &data : cd.dataVec)
        {
            for (const auto &modStat : data.modStatVec)
            {
                this->formatHead(modStat.key, optLine, head);
            }

            for (const auto &tag : data.modStatTagVec)
            {
                this->formatHead(tag.key, optLine, head);
            }
            break;
        }
        optLine += "----";

        string value;
        int count = 0;

        while (true)
        {
            cd = std::move(dfsrr.collect());
            for (const auto &data : cd.dataVec)
            {
                value.clear();
                for (const auto &modStat : data.modStatVec)
                {
                    this->formatValue(modStat.value, value);
                }

                for (const auto &tag : data.modStatTagVec)
                {
                    this->formatTag(tag.value, value);
                }


                if (count % 20 == 0)
                {
                    printf("%s%s%s\n", optLine.c_str(), config_.name.c_str(), optLine.c_str());
                    printf("Time\t\t%s\n", head.c_str());
                }

                if (count % cd.dataVec.size() == 0)
                {
                    time_t t1;
                    ::time(&t1);
                    char t[32] { 0 };
                    ::strftime(t, sizeof(t), "%d/%m %H:%M:%S", ::localtime(&t1));
                    printf("%s\t%s\n", t, value.c_str());
                }
                else
                {
                    printf("              \t%s\n", value.c_str());
                }
                ++count;
            }

            ::usleep(config_.interval * 1000 * 1000);
        }
        LOG(DEBUG, "print live  data");
    }


    void RunMode::formatHead(const string &metric, string &optLine, string &headStr)
    {
        optLine += "----";
        std::string space("");
        if (metric.size() < 6)
        {
            space.assign(6 - metric.size(), ' ');
        }
        headStr += space + metric;
        headStr += "\t";
    }


    void RunMode::formatTag(const std::string &value, std::string &valueStr)
    {
        valueStr += value;
        valueStr += "\t";
    }


    void RunMode::formatValue(double value, std::string &valueStr)
    {
        int count = 0;
        while (value - 1000 > 0.1)
        {
            value /= 1024;
            ++count;
        }

        char d2s[32] { 0 };
        if (count == 0)
        {
            std::sprintf(d2s, "%6.2f%s\t", value, Uint[count].c_str());
        }
        else
        {
            std::sprintf(d2s, "%5.1f%s\t", value, Uint[count].c_str());
        }
        valueStr += d2s;
    }


}; /* dfssrTool namespace end */
