// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     partition.cpp
// Author:       dingfang
// CreateDate:   2020-10-17 11:22:23
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 20:12:42
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


    CollectData_T Partition::collect()
    {
        CollectData_T cd;
        cd.moduleName = "partition";
        this->readStat();
        this->calculate(cd);

        return std::move(cd);
    }


    int Partition::readStat()
    {
        FILE *mntfile;
        struct mntent *mnt { nullptr };

        mntfile = ::setmntent(MtabFile.c_str(), "r");

        currPartitionVec_.clear();
        while ((mnt = ::getmntent(mntfile)) != nullptr)
        {
            if (!::strncmp(mnt->mnt_fsname, "/", 1))
            {
                struct PartitionStat_T ps;
                ps.device   = mnt->mnt_fsname;
                ps.mount    = mnt->mnt_dir;
                struct statfs fsbuf;
                if (!statfs(mnt->mnt_dir, &fsbuf))
                {
                    ps.bszie    = fsbuf.f_bsize;
                    ps.bfree    = fsbuf.f_bfree;
                    ps.blocks   = fsbuf.f_blocks;
                    ps.bavail   = fsbuf.f_bavail;
                    ps.ifree    = fsbuf.f_ffree;
                    ps.itotal   = fsbuf.f_files;
                }
                currPartitionVec_.push_back(ps);
            }
        }

        ::endmntent(mntfile);

        return 0;
    }


    int Partition::calculate(CollectData_T &cd)
    {
        for (auto &ps : currPartitionVec_)
        {
            Data_T data;
            UINT64 used = Delta(ps.blocks, ps.bfree);
            UINT64 nonrootTital = used + ps.bavail;
            data.modStatTagVec.push_back({ "device", ps.device });
            data.modStatTagVec.push_back({ "mount", ps.mount });
            data.modStatVec.push_back({ "bfree", static_cast<double>(ps.bavail * ps.bszie) });
            data.modStatVec.push_back({ "bused", static_cast<double>(used * ps.bszie) });
            data.modStatVec.push_back({ "btotl", static_cast<double>(ps.blocks * ps.bszie) });
            data.modStatVec.push_back({ "util", Percent(used, nonrootTital) + (used * 100 % nonrootTital != 0) });
            data.modStatVec.push_back({ "ifree", static_cast<double>(ps.ifree) });
            data.modStatVec.push_back({ "itotl", static_cast<double>(ps.itotal) });
            data.modStatVec.push_back({ "iutil", Percent(Delta(ps.itotal, ps.ifree), ps.itotal) });
            cd.dataVec.push_back(data);
        }

        return 0;
    }


}; /* mod namespace end */

