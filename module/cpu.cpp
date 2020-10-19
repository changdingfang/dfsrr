// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     cpu.cpp
// Author:       dingfang
// CreateDate:   2020-10-13 19:13:37
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 21:34:22
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


#include "dflog/dflog.h"
#include "cpu.h"
#include "fileHelper.h"

using namespace std;

namespace mod
{


    CollectData_T Cpu::collect()
    {
        CollectData_T cd;
        this->readStats(currCpuStat_);
        this->calculate(cd);

        lastCpuStat_ = currCpuStat_;

        return std::move(cd);
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


    int Cpu::calculate(CollectData_T &cd)
    {
        if (lastCpuStat_.user == 0)
        {
            return 0;
        }

        UINT64 user = Delta(currCpuStat_.user, lastCpuStat_.user);
        UINT64 nice = Delta(currCpuStat_.nice, lastCpuStat_.nice);
        UINT64 system = Delta(currCpuStat_.system, lastCpuStat_.system);
        UINT64 idle = Delta(currCpuStat_.idle, lastCpuStat_.idle);
        UINT64 ioWait = Delta(currCpuStat_.ioWait, lastCpuStat_.ioWait);
        UINT64 irq = Delta(currCpuStat_.irq, lastCpuStat_.irq);
        UINT64 softIrq = Delta(currCpuStat_.softIrq, lastCpuStat_.softIrq);
        UINT64 steal = Delta(currCpuStat_.steal, lastCpuStat_.steal);
        UINT64 total = user + nice + system + idle + ioWait + irq + softIrq + steal;

        double duser    = Percent(user, total);
        double dsys     = Percent(system, total);
        double didle    = Percent(idle, total);
        double dwait    = Percent(ioWait, total);
        double dsteal  = Percent(steal, total);
        Data_T data;
        data.modStatVec.push_back({ "user", duser });
        data.modStatVec.push_back({ "sys", dsys });
        data.modStatVec.push_back({ "wait", dwait });
        data.modStatVec.push_back({ "hirq", Percent(irq, total) });
        data.modStatVec.push_back({ "sirq", Percent(softIrq, total) });
        // data.modStatVec.push_back({ "steal", dsteal });
        // data.modStatVec.push_back({ "sysUser", dsys + duser});
        data.modStatVec.push_back({ "util", 100.0 - didle - dwait - dsteal });
        data.modStatVec.push_back({ "idle", didle });

        // data.modStatVec.push_back({ "nice", Percent(nice, total) });
        // data.modStatVec.push_back({ "ncpu", currCpuStat_.cpuNumber });
        cd.dataVec.push_back(data);

        return 0;
    }


}; /* mod namespace end */


