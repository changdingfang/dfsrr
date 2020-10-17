// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     partition.cpp
// Author:       dingfang
// CreateDate:   2020-10-17 11:22:23
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-17 18:56:39
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "partition.h"
#include "fileHelper.h"

#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>
#include <mntent.h>

using namespace std;

namespace mod
{


    std::map<std::string, double> Partition::collect()
    {
        this->readStat();
        this->calculate();

        return std::move(partitionMetric_);
    }


    int Partition::readStat()
    {
        FILE *mntfile;
        struct mntent *mnt { nullptr };

        mntfile = ::setmntent(MtabFile.c_str(), "r");

        currPartition_.clear();
        while ((mnt = ::getmntent(mntfile)) != nullptr)
        {
            if (!::strncmp(mnt->mnt_fsname, "/", 1))
            {
                this->readPartitionStat(mnt->mnt_dir, currPartition_);
            }
        }

        ::endmntent(mntfile);
    }


    int Partition::readPartitionStat(const char *fsname, struct PartitionStat_T &ps)
    {
        struct statfs fsbuf;
        if (!statfs(fsname, &fsbuf))
        {
            ps.bszie    += fsbuf.f_bsize;
            ps.bfree    += fsbuf.f_bfree;
            ps.blocks   += fsbuf.f_blocks;
            ps.bavail   += fsbuf.f_bavail;
            ps.ifree    += fsbuf.f_ffree;
            ps.itotal   += fsbuf.f_files;
        }

        return 0;
    }


    int Partition::calculate()
    {
        UINT64 used = Delta(currPartition_.blocks, currPartition_.bfree);
        partitionMetric_["bfree"] = currPartition_.bavail * currPartition_.bszie;
        partitionMetric_["bused"] = used * currPartition_.bszie;
        partitionMetric_["btotl"] = currPartition_.blocks * currPartition_.bszie;
        UINT64 nonrootTital = used + currPartition_.bavail;
        partitionMetric_["util"]  = Percent(used, nonrootTital) + (used * 100 % nonrootTital != 0);
        partitionMetric_["ifree"] = currPartition_.ifree;
        partitionMetric_["itotl"] = currPartition_.itotal;
        partitionMetric_["iutil"] = Percent(Delta(currPartition_.itotal, currPartition_.ifree), currPartition_.itotal);
    }


}; /* mod namespace end */

