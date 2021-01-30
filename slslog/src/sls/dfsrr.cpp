// =======================================================================
//  FileName:     dfsrr.cpp
//  Author:       dingfang
//  CreateDate:   2021-01-06 19:32:26
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-01-30 20:25:26
// =======================================================================

#include "dflog/dflog.h"
#include "common/stringUtil.h"
#include "sls/dfsrr.h"

#include <vector>

using namespace std;
using namespace common;
using json = nlohmann::json;

namespace sls
{


    std::set<std::string> DfsrrDataSelect::moduleSet_ 
    {
        "cpu"
            , "memory"
            , "load"
            , "partition"
            , "traffic"
            , "tcp"
            , "udp"
    };

    std::map< std::string, std::set<std::string> > DfsrrDataSelect::metricSet_
    {
        { 
            "cpu", 
                {
                    "idx", "ip", "timestamp"
                        , "hirq", "idle", "sirq"
                        , "sys", "user", "util", "wait"
                }
        },
        {
            "memory",
            { 
                "idx", "ip", "timestamp"
                    , "buff", "cache", "free"
                    , "total", "used", "util" 
            }
        },
        {
            "load",
            {
                "idx", "ip", "timestamp"
                    , "load1", "load5", "load15", "plit", "runq"
            }
        },
        {
            "partition",
            {
                "idx", "ip", "timestamp"
                    , "bfree", "btotl", "bused", "device"
                    , "ifree", "itotl", "iutil", "mount", "util"
            }
        },
        {
            "traffic",
            {
                "idx", "ip", "timestamp"
                    , "bytin", "bytout", "device"
                    , "pktdrp" , "pkterr", "pktin", "pktout",
            }
        },
        {
            "tcp",
            {
                "idx", "ip", "timestamp"
                    , "AtmpFa", "CurrEs", "EstRes", "active"
                    , "iseg", "outseg", "pasive", "retran"
            }
        },
        {
            "udp",
            {
                "idx", "ip", "timestamp"
                    , "idgm", "idmerr", "noport", "odgm"
            }
        }
    };


    DfsrrDataSelect::DfsrrDataSelect(const nlohmann::json &conf)
    {
        if (!this->parseConfig(conf))
        {
            throw ("parse config error!");
        }

        try
        {
            dbPtr_ = unique_ptr<MysqlDB>(new MysqlDB(dbinfo_));
        }
        catch (...)
        {
            LOG(ERROR, "init mysql database failed!");
            throw;
        }
    }


    bool DfsrrDataSelect::parseConfig(const json &confJson)
    {
        auto dbIt = confJson.find("database");
        if (dbIt == confJson.end())
        {
            LOG(WARN, "not found database config!");
            return false;
        }

        try
        {
            dbinfo_.ip       = dbIt->at("addr");
            dbinfo_.port     = dbIt->at("port");
            dbinfo_.dbName   = dbIt->at("dbname");
            dbinfo_.dbUser   = dbIt->at("username");
            dbinfo_.dbUserPwd= dbIt->at("userpwd");
        }
        catch (json::exception &e)
        {
            LOG(ERROR, "error: [{}]", e.what());
            return false;
        }
        catch (...)
        {
            LOG(ERROR, "database config failed...");
            return false;
        }

        return true;
    }


    int DfsrrDataSelect::getDfsrrData(const std::string &mod, const std::string &t, std::list<std::map<std::string, std::string>> &dataList)
    {
        json data = json::array();

        string metrics;
        auto modSet = metricSet_[mod];
        for (auto &metric : modSet)
        {
            metrics.append(metric).append(",");
        }
        metrics = metrics.substr(0, metrics.size() - 1);

        /* LOG(INFO, "metrics : {}", metrics); */
        this->selectData(mod, metrics, t, data);

        /* this->json2data(data, dataList); */

        /* dataList = data; */
        list<map<string, string>> m = data;
        dataList = m;

        /* for (const auto &element : data) */
        /* { */
        /*     map<string, string> m = element; */
        /*     for (const auto &it : element.items()) */
        /*     { */
        /*         LOG(DEBUG, "key----> {}: {}", it.key(), it.value()); */
        /*     } */
        /*     for (const auto &it : m) */
        /*     { */
        /*         LOG(DEBUG, "key: {}, value:{}", it.first, it.second); */
        /*     } */
        /* } */

        /* LOG(INFO, "data: {}", data.dump(4)); */
        return 0;
    }


    int DfsrrDataSelect::selectData(const string &mod, const string &metrics, const std::string &t, nlohmann::json &data)
    {
        string sql("select ");
        sql += metrics + " from `" + mod + "` where timestamp > " + t;


        sql += " order by timestamp limit 10000";
        if (!dbPtr_->execSql(sql, data))
        {
            LOG(ERROR, "exec sql failed!");
            return -1;
        }

        return 0;
    }


}; /* dfsrrWebServer namespace end */

