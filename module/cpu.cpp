// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     cpu.cpp
// Author:       dingfang
// CreateDate:   2020-10-13 19:13:37
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 18:50:08
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#include "dflog/dflog.h"
#include "cpu.h"
#include "fileHelper.h"

using namespace std;

namespace mod
{


    map<string, double> Cpu::collect()
    {
        this->readStats(currCpuStat_);
        this->calculate(currCpuStat_, lastCpuStat_);

        lastCpuStat_ = currCpuStat_;

        return std::move(cpuMetric_);
    }


    int Cpu::readStats(struct CpuStat_T &cpuStat)
    {
        FileHelper f(CpuStatFile);
        if (f.ifstream().fail())
        {
            LOG(WARN, "open cpu stat file failed!");
            return -1;
        }

        string name;
        while (f.ifstream().peek() != EOF)
        {
            f.ifstream() >> name;
            if (name == "cpu")
            {
                f.ifstream() >> cpuStat.user;
                f.ifstream() >> cpuStat.nice;
                f.ifstream() >> cpuStat.system;
                f.ifstream() >> cpuStat.idle;
                f.ifstream() >> cpuStat.ioWait;
                f.ifstream() >> cpuStat.irq;
                f.ifstream() >> cpuStat.softIrq;
                f.ifstream() >> cpuStat.steal;
            }
        }

        FileHelper cpuInfo(CpuInfoFile);
        if (cpuInfo.ifstream().fail())
        {
            LOG(WARN, "open cpu info file failed!");
            return -1;
        }

        string line;
        cpuStat.cpuNumber = 0;
        while (getline(cpuInfo.ifstream(), line))
        {
            if (line.compare(0, 9, "processor") == 0)
            {
                ++cpuStat.cpuNumber;
            }
        }

        return 0;
    }


    int Cpu::calculate(const struct CpuStat_T &currCpuStat, const struct CpuStat_T &lastCpuStat)
    {
        if (lastCpuStat.user == 0)
        {
            return 0;
        }

        UINT64 user = Delta(currCpuStat.user, lastCpuStat.user);
        UINT64 nice = Delta(currCpuStat.nice, lastCpuStat.nice);
        UINT64 system = Delta(currCpuStat.system, lastCpuStat.system);
        UINT64 idle = Delta(currCpuStat.idle, lastCpuStat.idle);
        UINT64 ioWait = Delta(currCpuStat.ioWait, lastCpuStat.ioWait);
        UINT64 irq = Delta(currCpuStat.irq, lastCpuStat.irq);
        UINT64 softIrq = Delta(currCpuStat.softIrq, lastCpuStat.softIrq);
        UINT64 steal = Delta(currCpuStat.steal, lastCpuStat.steal);
        UINT64 total = user + nice + system + idle + ioWait + irq + softIrq + steal;

        cpuMetric_.clear();
        // cpuMetric_["ncpu"]     = currCpuStat.cpuNumber;
        auto userIt   = cpuMetric_.insert(make_pair("user", Percent(user, total)));
        // cpuMetric_.insert(make_pair("nice", Percent(nice, total)));
        auto systemIt = cpuMetric_.insert(make_pair("sys", Percent(system, total)));
        auto idleIt   = cpuMetric_.insert(make_pair("idle", Percent(idle, total)));
        auto ioWaitIt = cpuMetric_.insert(make_pair("wait", Percent(ioWait, total)));
        cpuMetric_.insert(make_pair("hirq", Percent(irq, total)));
        cpuMetric_.insert(make_pair("sirq", Percent(softIrq, total)));
        auto stealIt  = cpuMetric_.insert(make_pair("steal", Percent(steal, total)));
        if (systemIt.second == true && userIt.second == true)
        {
            // cpuMetric_.insert(make_pair("systemUser", systemIt.first->second + userIt.first->second));
        }

        if (idleIt.second == true && ioWaitIt.second == true && stealIt.second == true)
        {
            cpuMetric_.insert(make_pair("util", 100.0 - idleIt.first->second - ioWaitIt.first->second - stealIt.first->second));
        }

        return 0;
    }


}; /* mod namespace end */


