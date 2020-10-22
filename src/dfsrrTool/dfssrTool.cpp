// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfssrTool.cpp
// Author:       dingfang
// CreateDate:   2020-10-14 19:51:06
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-22 20:51:52
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "runModule.h"
#include "common/argh.h"

#include <stdio.h>

#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;
using namespace dfssrTool;


void help()
{
    printf("Usage: dfsrrTool [Module] [options]\n");

    printf("Options:\n");
    printf("\t-w / --watch\t打印最近的n条数据:\tdfsrrTool --cpu -w 10\n");
    printf("\t-l / --live\t实时打印:\t\tdfsrrTool --cpu -l\n");
    printf("\t-i / --interval\t指定时间间隔:\t\tdfsrrTool --cpu -l -i 5\n");
    printf("\t-t / --time\t打印指定时间数据:\tdfsrrTool --cpu -t 20201001\n");
    printf("\t\t\t\t\t\tdfsrrTool --cpu -t 2020100108\n");
    printf("\t\t\t\t\t\tdfsrrTool --cpu -t 202010010800\n");
    printf("\t-h / --help\t帮助\n");

    printf("Modules:\n");
    printf("\t--cpu\t\tCPU\n");
    printf("\t--memory\t内存\n");
    printf("\t--load\t\t系统运行队列和平均负载\n");
    printf("\t--partition\t磁盘分区\n");
    printf("\t--tcp\t\ttcp流量\n");
    printf("\t--udp\t\tudp流量\n");
    printf("\t--traffic\t流量\n");

    exit(0);
}


int parseParam(int argc, char **argv, struct ModuleConfig_T &cfg)
{
    auto cmdl = argh::parser(argc, argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION);

    // 通过字符串名称访问flag的值: true或者false
    // 无视 “-” 的数量
    // cmdl["v"] 或 cmdl[{"v", "version", "..."}]

    if (cmdl[{"h", "help"}] || argc < 2)
    {
        help();
        return  0;
    }

    for (const auto &flag : cmdl.flags())
    {
        LOG(DEBUG, "flag: {}", flag);
        if (flag == "l" || flag == "list")
        {
            cfg.printMode = PRINT_LIVE;
        }
        else if (flag == "cpu" || flag == "memory" || flag == "mem" || flag == "load"
                 || flag == "partition" || flag == "tcp" || flag == "udp" || flag == "traffic")
        {
            cfg.name = flag;
        }
    }

    if (cfg.name.empty())
    {
        printf("请带上正确的module参数\n");
        exit(-1);
    }

    for (const auto &param : cmdl.params())
    {
        LOG(DEBUG, "param: {} : {}", param.first, param.second);
        if (param.first == "i" || param.first == "interval")
        {
            cfg.interval = stoi(param.second);
        }
        else if (param.first == "t" || param.first == "time")
        {
            cfg.printMode = PRINT_SPECIL_TIME;
            cfg.specifieTime = param.second;
        }
        else if (param.first== "w" || param.first == "watch")
        {
            cfg.printMode = PRINT_LAST_N_DATA;
            cfg.lastN = stoul(param.second);
        }
    }

    return 0;
}


int init(int argc, char **argv)
{
    // FILELOG | CONSOLE
    dflog::InitLog("", dflog::loggerOption::CONSOLE);
    dflog::SetLevel(INFO);
    // dflog::SetLevel(DEBUG);

    LOG(DEBUG, "dfssrtool is running...");

    struct ModuleConfig_T config;
    parseParam(argc, argv, config);

    RunMode runMode(config);
    runMode.run();

    return 0;
}


int main(int argc, char **argv)
{
    init(argc, argv);

    return 0;
}
