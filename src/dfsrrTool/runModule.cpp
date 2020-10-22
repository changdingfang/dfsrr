// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     runModule.cpp
// Author:       dingfang
// CreateDate:   2020-10-14 18:41:02
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-22 20:47:55
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "runModule.h"
#include "dfsrr/outputLocal.h"
#include "common/database.h"

#include <unistd.h>
#include <string.h>

#include <string>

using namespace std;
using namespace mod;
using namespace common;

namespace dfssrTool
{


    typedef map<time_t, CollectData_T, std::less<time_t>> ModData_t;


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


    int getDataCB(void *data, int argc, char **argv, char **colName)
    {
        ModData_t *pModData = static_cast<ModData_t *>(data);
        if (pModData == nullptr)
        {
            LOG(WARN, "data ptr is null!");
            return -1;
        }

        time_t ts = 0;
        Data_T modData;
        for (int i = 0; i < argc; ++i)
        {
            if (!::strcmp(colName[i], "timestamp"))
            {
                ts = stol(argv[i]);
            }
            else if (!::strcmp(colName[i], "mount") || !::strcmp(colName[i], "device"))
            {
                modData.modStatTagVec.push_back(ModStat_T<string, string>({colName[i], argv[i]}));
            }
            else
            {
                modData.modStatVec.push_back(ModStat_T<string, double>({colName[i], stod(argv[i])}));
            }
        }

        (*pModData)[ts].dataVec.push_back(modData);

        return 0;
    }


    void RunMode::printLastNData()
    {
        LOG(DEBUG, "print last n data");
        string datafile("/home/dfsrr/dfsrr/data/");
        datafile += config_.name + ".db";
        ModData_t modData;
        try
        {
            Database db(datafile);
            string sql("select * from ");
            sql += config_.name + dfsrr::tableSuffix;
            sql += " order by timestamp desc limit " + to_string(config_.lastN);

            bool finish = true;
            do
            {
                if (db.execSql(sql, getDataCB, &modData))
                {
                    LOG(ERROR, "exec sql failed! sql: [{}]", sql);
                    exit(-1);
                }
                const auto &it  = modData.cbegin();
                const auto &rit = modData.crbegin();
                UINT32 itSize  = it->second.dataVec.size();
                UINT32 ritSize = rit->second.dataVec.size();
                if (itSize != ritSize && finish)
                {
                    sql = "select * from " + config_.name + dfsrr::tableSuffix;
                    sql += " where timestamp <= " + to_string(it->first);
                    sql += " order by timestamp desc limit ";
                    sql += " " + to_string(itSize) + ",";
                    sql += " " + to_string(config_.lastN * (ritSize - 1));
                    finish = false;
                    continue;
                }
                finish = true;
            } while (!finish);
        }
        catch (...)
        {
            LOG(ERROR, "open data file failed! file: [{}]", datafile);
            exit(-1);
        }

        int count = 0;
        for (auto &md : modData)
        {
            if (count++ % 15 == 0)
            {
                this->printHead(md.second);
            }
            char t[32] { 0 };
            ::strftime(t, sizeof(t), "%d/%m %H:%M:%S", ::localtime(&md.first));
            this->printValue(md.second, t);
        }
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

        for (int count = 0; ; ++count)
        {
            CollectData_T cd = std::move(dfsrr.collect());
            if (count % 15 == 0)
            {
                this->printHead(cd);
            }

            time_t t1 = ::time(nullptr);
            char t[32] { 0 };
            ::strftime(t, sizeof(t), "%d/%m %H:%M:%S", ::localtime(&t1));
            this->printValue(cd, t);

            ::usleep(config_.interval * 1000 * 1000);
        }
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


    void RunMode::printHead(const CollectData_T &cd)
    {
        string head, optLine;
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

        printf("%s%s%s\n", optLine.c_str(), config_.name.c_str(), optLine.c_str());
        printf("Time\t\t%s\n", head.c_str());
    }


    void RunMode::printValue(const CollectData_T &cd, const string &ts)
    {
        bool firstLine = true;
        for (const auto &data : cd.dataVec)
        {
            string value;
            for (const auto &modStat : data.modStatVec)
            {
                this->formatValue(modStat.value, value);
            }

            for (const auto &tag : data.modStatTagVec)
            {
                this->formatTag(tag.value, value);
            }

            if (firstLine)
            {
                printf("%s\t%s\n", ts.c_str(), value.c_str());
                firstLine = false;
            }
            else
            {
                printf("              \t%s\n", value.c_str());
            }
        }
    }


}; /* dfssrTool namespace end */
