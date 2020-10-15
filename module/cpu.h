// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     cpu.h
// Author:       dingfang
// CreateDate:   2020-10-13 19:08:25
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 18:50:20
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __CPU_H__
#define __CPU_H__

#include "common/type.h"
#include "module.h"

#include <map>
#include <string>


namespace mod
{


    struct CpuStat_T
    {
        UINT64 user     { 0 };
        UINT64 nice     { 0 };
        UINT64 system   { 0 };
        UINT64 idle     { 0 };
        UINT64 ioWait   { 0 };
        UINT64 irq      { 0 };
        UINT64 softIrq  { 0 };
        UINT64 steal    { 0 };
        UINT64 guest    { 0 };
        UINT64 guestNice{ 0 };
        UINT64 cpuNumber{ 0 };
    };

    class Cpu final
        : public Module
    {
    public:
        virtual std::map<std::string, double> collect() override;

    private:
        int readStats(struct CpuStat_T &cpuStat);
        int calculate(const struct CpuStat_T &currCpuStat, const struct CpuStat_T &lastCpuStat);

    private:
        CpuStat_T currCpuStat_;
        CpuStat_T lastCpuStat_;
        std::map<std::string, double> cpuMetric_;
    };


}; /* mod namespace end */


#endif /* __CPU_H__ */
