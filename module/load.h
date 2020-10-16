// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     load.h
// Author:       dingfang
// CreateDate:   2020-10-15 18:57:44
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 19:33:44
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __LOAD_H__
#define __LOAD_H__

#include "module.h"

namespace mod
{


    struct LoadStat_T
    {
        double load1;
        double load5;
        double load15;
        double nrRuning;
        double nrThreads;
    };


    class Load
        : public Module
    {
    public:
        Load() = default;
        virtual ~Load() { };
        virtual std::map<std::string, double> collect();

    private:
        int readLoad();

    private:
        LoadStat_T currLoad_;
        std::map<std::string, double> loadMetric_;
    };


}; /* mod namespace end */

#endif /* __LOAD_H__ */
