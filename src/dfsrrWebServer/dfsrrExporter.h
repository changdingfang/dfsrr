// =======================================================================
//  FileName:     dfsrrExporter.h
//  Author:       dingfang
//  CreateDate:   2021-07-17 15:05:59
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-07-18 07:54:56
// =======================================================================

#ifndef __DFSRR_EXPORTER_H__
#define __DFSRR_EXPORTER_H__

#include "common/type.h"
#include "event2/http.h"
#include "nlohmann/json.hpp"
#include "common/mysqldb.h"

#include <string>
#include <map>
#include <set>

namespace dfsrrWebServer
{


    class Exporter
    {
    public:
        Exporter(const nlohmann::json &conf);
        ~Exporter() = default;
        static void exporter(struct evhttp_request *req, void *arg);
        int doRun(std::string &response);

    private:
        bool parseConfig(const nlohmann::json &confJson);
        int getMetgrics(std::string &metrics);
        int WrapPrometheus(std::string &metrics, const nlohmann::json &metricsJson, const std::string &mod);

    private:
        common::DBInfo_T dbinfo_;
        std::unique_ptr<common::MysqlDB> dbPtr_;
        static std::set<std::string> moduleSet_;
        static std::map< std::string, std::set<std::string> > metricSet_;
        static std::vector<std::string> mods_;
    };


}; /* dfsrrWebServer namespace end */


#endif /* __DFSRR_EXPORTER_H__ */
