// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     memory.h
// Author:       dingfang
// CreateDate:   2020-10-15 19:00:26
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 18:50:50
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common/type.h"
#include "module.h"

#include <map>
#include <string>

namespace mod
{


    struct MemoryStat_T
    {
        UINT64 total;
        UINT64 free;
        UINT64 buffers;
        UINT64 cached;
        UINT64 active;
        UINT64 inactive;
        UINT64 slab;
        UINT64 swapCached;
        UINT64 swapTotal;
        UINT64 swapFree;
        UINT64 committedAS;
    };


    class Memory final
        : public Module
    {
    public:
        virtual std::map<std::string, double> collect() override;

    private:
        int readMemoryInfo();
        int calculate();

    private:
        MemoryStat_T currMem_;
        MemoryStat_T lastMem_;
        std::map<std::string, double> memMetric_;
    };


}; /* mod namespace end */



#endif /* __MEMORY_H__ */
