// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.h
// Author:       dingfang
// CreateDate:   2020-10-20 19:08:11
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-25 22:43:02
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_H__
#define __DFSRR_H__

#include "common/type.h"
#include "dfsrr/output.h"
#include "dfsrrModule.h"
#include "nlohmann/json.hpp"

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <thread>

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
        UINT32  intv { 10 * 1000 };    /* us */
        int     checkOutputIntv_ { 10 }; /* s */

    };


    class Dfsrr
    {
    public:
        Dfsrr(nlohmann::json conf);
        ~Dfsrr() { };

        void run();
        inline void stop() { stop_ = true; };

    private:
        bool parseConfig(const nlohmann::json &);
        inline bool isStop() { return stop_; };

        static int sendData(Dfsrr *p);

        bool checkOutput();

    private:
        bool stop_;
        DfsrrConfig_T config_;

        std::vector<Module_T> moduleVec_;
        std::vector< std::shared_ptr<Output> > outputVec_;

        std::shared_ptr<std::thread> sendThreadPtr_;

        time_t lastCheckTime_;
    };


}; /* dfsrr namespace end */


#endif /* __DFSRR_H__ */
