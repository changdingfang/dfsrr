// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     partition.h
// Author:       dingfang
// CreateDate:   2020-10-16 19:07:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 19:15:01
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __PARTITION_H__
#define __PARTITION_H__

#include "module.h"

namespace mod
{


    struct PartitionStat_T
    {
        UINT64 bszie    { 0 };
        UINT64 blocks   { 0 };
        UINT64 bfree    { 0 };
        UINT64 bavail   { 0 };
        UINT64 itotal   { 0 };
        UINT64 ifree    { 0 };
        std::string device { "" };
        std::string mount  { "" };
    };


    class Partition final
        : public Module
    {
    public:
        Partition() = default;
        virtual ~Partition() { };
        virtual CollectData_T collect() override;

    private:
        int readStat();
        int calculate(CollectData_T &cd);

    private:
        std::vector<PartitionStat_T> currPartitionVec_;
    };


}; /* mod namespace end */


#endif /* __PARTITION_H__ */
