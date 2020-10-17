// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     module.h
// Author:       dingfang
// CreateDate:   2020-10-13 19:40:48
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-17 11:52:44
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __MODULE_H__
#define __MODULE_H__


#include "common/type.h"

#include <string>
#include <map>

namespace mod
{


    static const std::string CpuStatFile     = "/proc/stat";
    static const std::string CpuInfoFile     = "/proc/cpuinfo";
    static const std::string MemoryInfoFIle  = "/proc/meminfo";
    static const std::string LoadFile        = "/proc/loadavg";
    static const std::string NetDev          = "/proc/net/dev";
    static const std::string NetSnmp         = "/proc/net/snmp";
    static const std::string TcpFile         = "/proc/net/tcp";
    static const std::string NetStatFile     = "/proc/net/netstat";
    static const std::string MtabFile        = "/etc/mtab";

    static const std::string Uint[]          = {"", "K", "M", "G", "T", "P"};


    class Module
    {
    public:
        Module() = default;
        virtual ~Module() { };
        virtual std::map<std::string, double>collect() = 0;
    };

    inline UINT64 Delta(UINT64 t1, UINT64 t2)
    {
        return t1 > t2 ? t1 - t2 : 0;
    }

    inline double Percent(UINT64 t1, UINT64 t2)
    {
        return t2 != 0 && t1 > 0 ? 100.0 * t1 / t2 : 0.0;
    }

    inline double Ratio(UINT64 t1, UINT64 t2)
    {
        return t2 != 0 && t1 > 0 ? 1.0 * t1 / t2 : 0.0;
    }


}; /* mod namespace end */


#endif /* __MODULE_H__ */
