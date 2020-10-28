// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     main.cpp
// Author:       dingfang
// CreateDate:   2020-10-26 21:48:01
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 08:21:06
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrrServer/dfsrrServer.h"
#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <fstream>

using namespace dfsrrServer;
using namespace std;
using json = nlohmann::json;


bool parseConfig(const string &confFile, json &confJson)
{
    if (confFile.empty())
    {
        printf("config file is empty\n");
        return false;
    }

    ifstream fin;
    fin.open(confFile, std::ios_base::in);
    if (fin.fail())
    {
        printf("open config file failed! file: [%s]\n", confFile.c_str());
        return false;
    }

    string configContent(""), line;

    while (getline(fin, line))
    {
        configContent += line;
    }

    fin.close();

    try
    {
        confJson = json::parse(configContent);
    }
    catch (json::exception &e)
    {
        // printf("parse json config file failed! error: [%s], id: [%d]\n", e.what(), e.id);
        printf("parse json config file failed!\n");
        return false;
    }

    return true;
}


bool initLog(const json &conf)
{
    const json &logConf = conf["log"];

    if (!logConf.is_object())
    {
        printf("not found log conf!\n");
        return false;
    }

    string logfile = logConf["file"];

    dflog::loggerOption::Option_t logOpt;
    json logOptJson = {
        { "console", dflog::loggerOption::CONSOLE },
        { "file", dflog::loggerOption::FILELOG }
    };

    const json &output = logConf["output"];
    if (output.is_array())
    {
        vector<string> outputVec = output;
        for (auto &op : outputVec)
        {
            if (!logOptJson[op].is_null())
            {
                dflog::loggerOption::Option_t lop = logOptJson[op];
                logOpt |= lop;
            }
        }
    }

    dflog::InitLog(logfile, logOpt);

    string logLevel(logConf["level"]);
    json logLevelJson = {
        { "debug", DEBUG },
        { "info", INFO },
        { "warn", WARN },
        { "error", ERROR },
        { "critical", CRITICAL }
    };

    if (logLevelJson[logLevel] != nullptr)
    {
        LOG(CRITICAL, "log level update: {}", logLevel);
        dflog::SetLevel(logLevelJson[logLevel]);
    }

    return true;
}


int init(int argc, char **argv)
{
    json conf;
    string logfile;
    logfile = argc == 2 ? argv[1] :  "../conf/dfsrr-server.json";

    if (!parseConfig(logfile, conf))
    {
        printf("parse config error\n");
        exit(-1);
    }

    if (!initLog(conf))
    {
        printf("init log error!\n");
        exit(-1);
    }

    LOG(INFO, "dfssr is running...");

    DfsrrServer dfss(conf);
    dfss.run();

    return 0;
}


int main(int argc, char **argv)
{
    init(argc, argv);

    return 0;
}
