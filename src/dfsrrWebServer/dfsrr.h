// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.h
// Author:       dingfang
// CreateDate:   2020-10-31 12:30:23
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 13:06:30
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_H__
#define __DFSRR_H__

#include "common/type.h"
#include "event2/http.h"
#include "nlohmann/json.hpp"
#include "common/mysqldb.h"

#include <map>
#include <set>
#include <string>
#include <memory>

namespace dfsrrWebServer
{


    class DfsrrDataSelect
    {
    public:
        DfsrrDataSelect(const nlohmann::json &conf);
        ~DfsrrDataSelect() = default;
        static void dfsrr(struct evhttp_request *req, void *arg);
        int doRun(const std::map<std::string, std::string> &, std::string &);

    private:
        bool parseConfig(const nlohmann::json &conf);

        int parseCommonParam(const std::map<std::string, std::string> &);
        int selectData(nlohmann::json &data);

    private:
        common::DBInfo_T dbinfo_;
        std::unique_ptr<common::MysqlDB> dbPtr_;

        std::string mod_        { "" };
        std::string metric_     { "" };
        std::string watch_      { "" };
        std::string startTime_  { "" };
        std::string endTime_    { "" };

        static std::set<std::string> moduleSet_;
        static std::map< std::string, std::set<std::string> > metricSet_;
    };

}; /* dfsrrWebServer namespace end */


#endif /* __DFSRR_H__ */
