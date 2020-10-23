// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     runModule.cpp
// Author:       dingfang
// CreateDate:   2020-10-14 18:41:02
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-23 18:53:13
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "runModule.h"
#include "dfsrr/outputLocal.h"
#include "common/database.h"

#include <unistd.h>
#include <time.h>
#include <string.h>

#include <string>

using namespace std;
using namespace mod;
using namespace common;

namespace dfssrTool
{


    static int getDataCB(void *data, int argc, char **argv, char **colName)
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


    RunMode::RunMode(ModuleConfig_T &config)
        : config_(config)
    { }


    int RunMode::run()
    {
        this->dealConfig();

        LOG(DEBUG, "Run Mode...");
        switch (config_.printMode)
        {
            case PRINT_LAST_N_DATA: this->printLastNData(); break;
            case PRINT_SPECIL_TIME: this->printDateDate();  break;
            case PRINT_LIVE:        this->printLiveData();  break;
            default: this->defaultPrint(); break;
        }

        return 0;
    }


    int RunMode::dealConfig()
    {
        if (config_.filename.empty())
        {
            config_.filename = DataDir + "/";
            config_.filename += config_.name + ".db";
        }
        
        if (!config_.specifieTime.empty())
        {
            /* "20201023"  8 bytes */
            /* "20201023200000" 14 bytes */
            int range = 60;
            switch (config_.specifieTime.size())
            {
                case 8: range = 86400;  break; /* 日级 */
                case 10: range = 3600;  break; /* 时级 */
                case 12: range = 60;    break; /* 分级 */
                default:
                    printf("请输入正确的时间格式\n");
                    exit(-1);
            }
            config_.specifieTime.resize(14, '0');

            struct tm t;
            ::strptime(config_.specifieTime.c_str(), "%Y%m%d%H%M%S", &t);
            time_t st = ::mktime(&t);
            time_t et = st + range;

            config_.specifieTime = to_string(st);
            config_.specifieTime += "," + to_string(et);
        }

        return 0;
    }


    void RunMode::printLastNData()
    {
        LOG(DEBUG, "print last n data");
        string datafile(config_.filename);
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

        this->printModData(modData);
    }


    void RunMode::printDateDate()
    {
        LOG(DEBUG, "print date data");
        string datafile(config_.filename);

        string::size_type pos = config_.specifieTime.find(",");
        if (pos == string::npos)
        {
            printf("time error! time: [%s]\n", config_.specifieTime.c_str());
            exit(-1);
        }
        string startTime = config_.specifieTime.substr(0, pos);
        string endTime   = config_.specifieTime.substr(pos + 1, string::npos);

        ModData_t modData;
        try
        {
            Database db(datafile);
            string sql("select * from ");
            sql += config_.name + dfsrr::tableSuffix;
            sql += " where timestamp >= " + startTime;
            sql += " and timestamp < " + endTime;
            sql += " order by timestamp desc";

            if (db.execSql(sql, getDataCB, &modData))
            {
                LOG(ERROR, "exec sql failed! sql: [{}]", sql);
                exit(-1);
            }
        }
        catch (...)
        {
            LOG(ERROR, "open data file failed! file: [{}]", datafile);
            exit(-1);
        }

        this->printModData(modData);
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
            if (count % DLine == 0)
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


    void RunMode::defaultPrint()
    {
        printf("请确保输入了正确的参数\n");
        exit(-1);
    }


    void RunMode::printModData(const ModData_t &modData)
    {
        int count = 0;
        for (const auto &md : modData)
        {
            if (count++ % DLine == 0)
            {
                this->printHead(md.second);
            }
            char t[32] { 0 };
            ::strftime(t, sizeof(t), "%d/%m %H:%M:%S", ::localtime(&md.first));
            this->printValue(md.second, t);
        }
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
                if (this->checkFilter(modStat.key))
                {
                    this->formatValue(modStat.value, value);
                }
            }

            for (const auto &tag : data.modStatTagVec)
            {
                if (this->checkFilter(tag.key))
                {
                    this->formatTag(tag.value, value);
                }
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


    void RunMode::formatHead(const string &metric, string &optLine, string &headStr)
    {
        if (!this->checkFilter(metric))
        {
            return ;
        }

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
