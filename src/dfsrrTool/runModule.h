// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     runModule.h
// Author:       dingfang
// CreateDate:   2020-10-14 19:25:08
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-22 18:50:58
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __RUN_MODULE_H__
#define __RUN_MODULE_H__

#include "common/type.h"
#include "dfsrrModule.h"

namespace dfssrTool
{


    enum PRINT_MODE_E
    {
        PRINT_NULL,
        PRINT_LAST_N_DATA,
        PRINT_SPECIL_TIME,
        PRINT_LIVE
    };


    struct ModuleConfig_T
    {
        PRINT_MODE_E printMode;
        std::string name;
        std::string specifieTime;
        UINT64 lastN { 1 };
        int interval { 1 };
    };

    class RunMode
    {
    public:
        RunMode(ModuleConfig_T &config);
        int run();

    private:
        void printLastNData();
        void printDateDate();
        void printLiveData();

        void printHead(const mod::CollectData_T &cd);
        void printValue(const mod::CollectData_T &cd, const std::string &ts);

        void formatHead(const std::string &metric, std::string &optLine, std::string &headStr);
        void formatTag(const std::string &value, std::string &valueStr);
        void formatValue(double value, std::string &valueStr);


    private:
        ModuleConfig_T config_;
    };


}; /* dfssrTool namespace end */


#endif /* __RUN_MODULE_H__ */
