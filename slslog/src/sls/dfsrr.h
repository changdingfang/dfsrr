// =======================================================================
//  FileName:     dfsrr.h
//  Author:       dingfang
//  CreateDate:   2021-01-06 19:32:26
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-01-30 20:25:16
// =======================================================================

#ifndef __DFSRR_H__
#define __DFSRR_H__

#include "common/type.h"
#include "nlohmann/json.hpp"
#include "common/mysqldb.h"

#include <map>
#include <list>
#include <set>
#include <string>
#include <memory>

namespace sls
{


    class DfsrrDataSelect
    {
    public:
        DfsrrDataSelect(const nlohmann::json &conf);
        ~DfsrrDataSelect() = default;
        int getDfsrrData(const std::string &mod, const std::string &t, std::list<std::map<std::string, std::string>> &dataList);

    private:
        bool parseConfig(const nlohmann::json &conf);

        int selectData(const std::string &mod, const std::string &metrics, const std::string &t, nlohmann::json &data);

    private:
        common::DBInfo_T dbinfo_;
        std::unique_ptr<common::MysqlDB> dbPtr_;

        std::string mod_        { "" };
        std::string metric_     { "" };
        std::string watch_      { "" };
        std::string startTime_  { "" };
        std::string endTime_    { "" };
        std::string ip_         { "" };

        static std::set<std::string> moduleSet_;
        static std::map< std::string, std::set<std::string> > metricSet_;
    };


}; /* sls namespace end */


#endif /* __DFSRR_H__ */
