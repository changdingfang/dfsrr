// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     traffic.h
// Author:       dingfang
// CreateDate:   2020-10-16 19:08:43
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 20:22:42
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __TRAFFIC_H__
#define __TRAFFIC_H__


#include "module.h"

namespace mod
{


    struct TrafficStat_T
    {
        UINT64 bytein;
        UINT64 byteout;
        UINT64 pktin;
        UINT64 pktout;
        UINT64 pkterrin;
        UINT64 pktdrpin;
        UINT64 pkterrout;
        UINT64 pktdrpout;
        UINT64 timestamp;
    };


    class Traffic final
        : public Module
    {
    public:
        Traffic() = default;
        virtual ~Traffic() { };
        virtual std::map<std::string, double> collect() override;

    private:
        int readNetDev();
        int calculate();
        bool checkNetDev(const std::string &devName);

    private:
        std::map<std::string, TrafficStat_T> currTrafficMap_;
        std::map<std::string, TrafficStat_T> lastTrafficMap_;
        std::map<std::string, double> trafficMetric_;
    };


}; /* mod namespace end */


#endif /* __TRAFFIC_H__ */
