#include "outputSls.h"
#include "sls/dfsrr.h"
#include "dflog/dflog.h"
#include "nlohmann/json.hpp"

#include <stdio.h>

#include <vector>


using json = nlohmann::json;
using namespace std;
using namespace sls;


void sparse(list<map<string, string>> &dataList, time_t &startTime, time_t span)
{
    auto lit = dataList.begin(); 
    if (lit == dataList.end())
    {
        LOG(WARN, "data list is empty!");
        return ;
    }

    while (lit != dataList.end())
    {
        if (std::stol((*lit)["timestamp"]) > startTime)
        {
            startTime = stol((*lit)["timestamp"]) + span;
            ++lit;
        }
        else
        {
            dataList.erase(lit++);
        }
    }
}


void getLastTime(const string &mod, string &t)
{
    FILE *fp;
    string filename(mod);
    filename.append("_lasttime.conf");
    if ((fp = fopen(filename.c_str(), "r")) == nullptr)
    {
        LOG(WARN, "open latttime file failed!");
        t = "-1";
        return ;
    }

    char numStr[32] = { 0 };
    fread(numStr, 32, 1, fp);

    t = numStr;
    if (t.empty())
    {
        LOG(WARN, "read data is empty!");
        t = "-1";
    }
    fclose(fp);
}


void setLastTime(const string &mod, string t)
{
    FILE *fp;
    string filename(mod);
    filename.append("_lasttime.conf");
    if ((fp = fopen(filename.c_str(), "w")) == nullptr)
    {
        LOG(WARN, "open latttime file failed!");
        return ;
    }

    fwrite(t.c_str(), t.size(), 1, fp);

    fclose(fp);
}


void post_logs_with_http_cont_lz4_log_option()
{
    const string id         = "LTAI4G1ZY8UjcYSaPLmHBUoC";
    const string secret     = "a2qyspdeccoka7GhjEx73TTAGbnd7q";
    const string endpoint   = "cn-shanghai.log.aliyuncs.com";
    const string project    = "dfsrr-test";
    const std::string topic = "dfsrr_test";
    std::string source      = "dingfang";

    json database, conf;
    database["addr"]        = "127.0.0.1";
    database["port"]        = 3306;
    database["dbname"]      = "dfsrr_test";
    database["username"]    = "dfsrr";
    database["userpwd"]     = "1";
    conf["database"]        = database;

    DfsrrDataSelect dds(conf);
    vector<string> mods = { "cpu", "memory", "load" };

    std::string error;
    Sls *pSls = new Sls();

    while (true)
    {
        for (const auto &mod : mods)
        {
            string logstore("dfsrr-");
            logstore += mod;
            list<map<string, string>> dataList;
            string lastTime;
            getLastTime(mod, lastTime);
            LOG(INFO, "last time: {}", lastTime);
            dds.getDfsrrData(mod, lastTime, dataList);
            LOG(INFO, "data list size: {}", dataList.size());
            time_t startTime = stoul(lastTime);
            sparse(dataList, startTime, 300);
            if (lastTime == "-1")
            {
                startTime = time(nullptr);
            }
            setLastTime(mod, to_string(startTime));
            LOG(INFO, "sparse data list size: {}", dataList.size());
            if (dataList.empty())
            {
                continue;
            }
            if (pSls->send(id, secret, endpoint, project, logstore, dataList, topic, source, error) != 0)
            {
                LOG(ERROR, "send data to store failed!, error msg: {}", error);
            }
        }
        /* 360s 查询一次数据库，输出到sls */
        ::usleep(360 * 1000 * 1000);
    }
}


int main(void)
{
    dflog::InitLog("sls-dfsrr.log", dflog::loggerOption::FILELOG);
    /* dflog::InitLog("sls-dfsrr.log", dflog::loggerOption::FILELOG | dflog::loggerOption::CONSOLE); */
    post_logs_with_http_cont_lz4_log_option();
    return 0;
}
