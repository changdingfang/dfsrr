// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     tcp.h
// Author:       dingfang
// CreateDate:   2020-10-15 18:43:51
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-15 19:50:28
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __TCP_H__
#define __TCP_H__


#include "common/type.h"
#include "module.h"

#include <string>
#include <map>

namespace mod
{


    struct TcpStat_T
    {
        UINT64 activeOpens;
        UINT64 passiveOpens;
        UINT64 inSegs;
        UINT64 outSegs;
        UINT64 attemptFails;
        UINT64 estabResets;
        UINT64 currEstab;
        UINT64 retransSegs;
        UINT64 inErrs;
        UINT64 outRsts;
        UINT64 timestamp;
    };


    class Tcp
        : public Module
    {
    public:
        virtual std::map<std::string, double> collect() override;

    private:
        int readSnmp();
        int calculate();

    private:
        TcpStat_T currTcp_;
        TcpStat_T lastTcp_;
        std::map<std::string, double> tcpMertic_;
    };


}; /* mod namespace end */


#endif /* __TCP_H__ */
