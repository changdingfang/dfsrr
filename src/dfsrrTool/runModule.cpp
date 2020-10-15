// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     runModule.cpp
// Author:       dingfang
// CreateDate:   2020-10-14 18:41:02
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 21:04:18
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
        switch (config_.printMode)
        {
            case PRINT_LAST_N_DATA: this->printLastNData(); break;
            case PRINT_SPECIL_TIME: this->printDateDate();  break;
            case PRINT_LIVE:        this->printLiveData();  break;
            default: printf("not found mod\n"); exit(-1);
        }
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
        DfSrrModule dfsrr(config_.name);
        dfsrr.collect();
        ::sleep(config_.interval);

        map<string, double> m;

        string head, optLine;
        m = std::move(dfsrr.collect());
        for (const auto &it : m)
        {
            this->formatHead(it.first, optLine, head);
        }
        optLine += "----";

        string value;
        int count = 0;

        while (true)
        {
            m = std::move(dfsrr.collect());
            value.clear();
            for (const auto &it : m)
            {
                this->formatValue(it.second, value);
            }

            if (count++ % 12 == 0)
            {
                printf("%s%s%s\n", optLine.c_str(), config_.name.c_str(), optLine.c_str());
                printf("Time\t\t%s\n", head.c_str());
            }

            time_t t1;
            ::time(&t1);
            char t[32] { 0 };
            ::strftime(t, sizeof(t), "%d/%m %H:%M:%S", ::localtime(&t1));
            printf("%s\t%s\n", t, value.c_str());

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
