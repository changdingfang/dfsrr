// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     cpu.h
// Author:       dingfang
// CreateDate:   2020-10-13 19:08:25
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 21:34:18
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __CPU_H__
#define __CPU_H__

#include "module.h"


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
        Cpu() = default;
        virtual ~Cpu() { };
        virtual CollectData_T collect() override;

    private:
        int readStats(struct CpuStat_T &cpuStat);
        int calculate(CollectData_T &cd);

    private:
        CpuStat_T currCpuStat_;
        CpuStat_T lastCpuStat_;
    };


}; /* mod namespace end */


#endif /* __CPU_H__ */
