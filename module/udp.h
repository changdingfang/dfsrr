// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     udp.h
// Author:       dingfang
// CreateDate:   2020-10-15 18:57:52
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 19:34:37
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __UDP_H__
#define __UDP_H__

#include "module.h"


namespace mod
{


    struct UdpStat_T
    {
        UINT64 inDatagrams;
        UINT64 outDatagrams;
        UINT64 noPorts;
        UINT64 inErrors;
        UINT64 timestamp;

        void clear()
        {
            inDatagrams     = 0;
            outDatagrams    = 0;
            noPorts         = 0;
            inErrors        = 0;
            timestamp       = 0;
        }
    };


    class Udp final
        : public Module
    {
    public:
        Udp() = default;
        virtual ~Udp() { };
        virtual std::map<std::string, double> collect() override;

    private:
        int readSnmp();
        int calculate();

    private:
        UdpStat_T currUdp_;
        UdpStat_T lastUdp_;
        std::map<std::string, double> udpMetric_;
    };


}; /* mod namespace end */


#endif /* __UDP_H__ */
