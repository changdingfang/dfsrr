// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.h
// Author:       dingfang
// CreateDate:   2020-10-31 12:30:23
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-31 16:48:19
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_H__
#define __DFSRR_H__

#include "event2/http.h"
#include "nlohmann/json.hpp"
#include "common/mysqldb.h"

#include <map>
#include <set>
#include <string>
#include <memory>

namespace dfsrrWebServer
{

    static const int SERVER_ERROR = 503;
    static const std::string SERVER_ERROR_MSG = "server internal error!";
    static const int SERVER_NOT_FOUND = 404;

    class DfsrrDataSelect
    {
    public:
        DfsrrDataSelect(const nlohmann::json &conf);
        ~DfsrrDataSelect() = default;
        static void dfsrr(struct evhttp_request *req, void *arg);
        int doRun(const std::map<std::string, std::string> &, std::string &);

    private:
        bool parseConfig(const nlohmann::json &conf);

        void parseCommonParam(const std::map<std::string, std::string> &);
        int selectData(nlohmann::json &data);

    private:
        common::DBInfo_T dbinfo_;
        std::unique_ptr<common::MysqlDB> dbPtr_;

        std::string mod_;
        std::string metric_;
        std::string watch_;

        static std::set<std::string> moduleSet_;

    };

}; /* dfsrrWebServer namespace end */


#endif /* __DFSRR_H__ */
