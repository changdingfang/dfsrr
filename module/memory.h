// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     memory.h
// Author:       dingfang
// CreateDate:   2020-10-15 19:00:26
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 19:32:42
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "module.h"


namespace mod
{


    struct MemoryStat_T
    {
        UINT64 total;
        UINT64 free;
        UINT64 available; /* 高版本系统内核拥有该字段 */
        UINT64 buffers;
        UINT64 cached;
        UINT64 active;
        UINT64 inactive;
        UINT64 slab;
        UINT64 swapCached;
        UINT64 swapTotal;
        UINT64 swapFree;
        UINT64 committedAS;

        void clear()
        {
            total       = 0;
            free        = 0;
            available   = 0;
            buffers     = 0;
            cached      = 0;
            active      = 0;
            inactive    = 0;
            slab        = 0;
            swapCached  = 0;
            swapTotal   = 0;
            swapFree    = 0;
            committedAS = 0;
        }
    };


    class Memory final
        : public Module
    {
    public:
        Memory() = default;
        virtual ~Memory() { };
        virtual std::map<std::string, double> collect() override;

    private:
        int readMemoryInfo();
        int calculate();

    private:
        MemoryStat_T currMem_;
        std::map<std::string, double> memMetric_;
    };


}; /* mod namespace end */



#endif /* __MEMORY_H__ */
