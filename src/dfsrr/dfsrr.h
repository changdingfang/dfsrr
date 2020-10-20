// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.h
// Author:       dingfang
// CreateDate:   2020-10-20 19:08:11
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 21:14:03
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_H__
#define __DFSRR_H__

#include "common/type.h"
#include "dfsrr/output.h"
#include "dfsrrModule.h"

#include <memory>
#include <string>
#include <vector>
#include <set>

namespace dfsrr
{


    struct Module_T
    {
        std::shared_ptr<mod::DfSrrModule> modPtr;
        std::string mod;
        std::string name;
        UINT32 intv;
        std::set<std::string> filter;
        time_t lastTime;
    };


    struct OutputLocal_T
    {
        bool        use;
        std::string pathdir;
    };


    struct OutputTcp_T
    {
        bool        use;
        std::string addr;
        short       port;
    };


    struct DfsrrConfig_T
    {
        OutputLocal_T   outputLocal;
        OutputTcp_T     outputTcp;
        UINT32 intv;    /* us */
    };


    class Dfsrr
    {
    public:
        Dfsrr(std::string configPath);
        ~Dfsrr() { };

        void run();

    private:
        bool parseConfig();
        inline bool isStop() { return stop_; };

    private:
        bool stop_;
        std::string configFile_;
        DfsrrConfig_T config_;

        std::vector<Module_T> moduleVec_;
        std::vector< std::shared_ptr<Output> > outputVec_;
    };


}; /* dfsrr namespace end */



#endif /* __DFSRR_H__ */
