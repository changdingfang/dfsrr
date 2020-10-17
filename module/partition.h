// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     partition.h
// Author:       dingfang
// CreateDate:   2020-10-16 19:07:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-17 18:57:28
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __PARTITION_H__
#define __PARTITION_H__

#include "module.h"

namespace mod
{


    struct PartitionStat_T
    {
        UINT64 bszie;
        UINT64 blocks;
        UINT64 bfree;
        UINT64 bavail;
        UINT64 itotal;
        UINT64 ifree;

        void clear()
        {
        bszie   = 0;
        blocks  = 0;
        bfree   = 0;
        bavail  = 0;
        itotal  = 0;
        ifree   = 0;
        }
    };


    class Partition
        : public Module
    {
    public:
        Partition() = default;
        virtual ~Partition() { };
        virtual std::map<std::string, double> collect() override;

    private:
        int readStat();
        int readPartitionStat(const char *fsname, struct PartitionStat_T &ps);
        int calculate();

    private:
        PartitionStat_T currPartition_;
        std::map<std::string, double> partitionMetric_;
    };


}; /* mod namespace end */


#endif /* __PARTITION_H__ */
