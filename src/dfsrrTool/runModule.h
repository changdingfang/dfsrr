// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     runModule.h
// Author:       dingfang
// CreateDate:   2020-10-14 19:25:08
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-23 18:53:15
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __RUN_MODULE_H__
#define __RUN_MODULE_H__

#include "common/type.h"
#include "dfsrrModule.h"

#include <unordered_set>

namespace dfssrTool
{

    static std::string DataDir = "/home/dfsrr/dfsrr/data";
    static const int DLine = 15;
    typedef std::map<time_t, mod::CollectData_T, std::less<time_t>> ModData_t;


    enum PRINT_MODE_E
    {
        PRINT_NULL,
        PRINT_LAST_N_DATA,
        PRINT_SPECIL_TIME,
        PRINT_LIVE
    };


    struct ModuleConfig_T
    {
        PRINT_MODE_E printMode   { PRINT_NULL };
        std::string name         { "" };
        std::unordered_set<std::string> filter;


        std::string specifieTime { "" };
        std::string filename     { "" };
        UINT64 lastN { 1 };
        int interval { 1 };
    };

    class RunMode
    {
    public:
        RunMode(ModuleConfig_T &config);
        int run();

    private:
        int dealConfig();
        void printLastNData();
        void printDateDate();
        void printLiveData();
        void defaultPrint();

        void printModData(const ModData_t &modData);

        void printHead(const mod::CollectData_T &cd);
        void printValue(const mod::CollectData_T &cd, const std::string &ts);

        void formatHead(const std::string &metric, std::string &optLine, std::string &headStr);
        void formatTag(const std::string &value, std::string &valueStr);
        void formatValue(double value, std::string &valueStr);

        inline bool checkFilter(const std::string &key) const
        {
            if (config_.filter.size() == 0)
            {
                return true;
            }
            return config_.filter.find(key) != config_.filter.end();
        }


    private:
        ModuleConfig_T config_;
    };


}; /* dfssrTool namespace end */


#endif /* __RUN_MODULE_H__ */
