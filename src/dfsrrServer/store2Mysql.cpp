// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     store2Mysql.cpp
// Author:       dingfang
// CreateDate:   2020-10-29 19:59:12
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 18:30:00
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "nlohmann/json.hpp"
#include "dfsrrServer/store2Mysql.h"

using namespace std;
using namespace common;
using json = nlohmann::json;

namespace dfsrrServer
{


    Store2Mysql::Store2Mysql(const DBInfo_T &dbinfo)
        : stop_(false)
          , maxSize_(1024)
    {
        LOG(DEBUG, "store2Mysql init...");
        dbPtr_ = make_shared<MysqlDB>(dbinfo);
        th_ = thread(Store2Mysql::storeData, this);
    }


    Store2Mysql::~Store2Mysql()
    {
        this->stop();
        th_.join();
    }


    int Store2Mysql::convert(const string  &data, string &dst)
    {
        json jdata = json::parse(data);

        string sql;
        string mod = jdata["module"];
        string ts = to_string(jdata["timestamp"]);

        LOG(INFO, "convert....");

        json jmodData = jdata["data"];
        for (auto &d : jmodData)
        {
            string tmpSql("insert into "), fields("(timestamp,"), values(" values(");
            tmpSql += mod;
            values += ts + ",";
            for (auto it = d.begin(); it != d.end(); ++it)
            {
                fields += it.key() + ",";
                values += to_string(it.value()) + ",";
                // LOG(INFO, "key: [{}], ", it.key());
                // LOG(INFO, "value[{}]", to_string(it.value()));
            }
            fields = fields.substr(0, fields.size() - 1) + ")";
            values = values.substr(0, values.size() - 1) + ");";
            tmpSql += fields + " " + values;
            sql += tmpSql;
        }

        LOG(INFO, "sql: [{}]", sql);

        dst = std::move(sql);

        return 0;
    }


    int Store2Mysql::addData(const std::string &data)
    {
        StoreData_T sd;
        sd.data = data;
        std::unique_lock<std::mutex> lock(mutex_);
        if (dataQueue_.size() > maxSize_)
        {
            LOG(WARN, "queue full, discard data!");
            return -1;
        }
        dataQueue_.push(sd);

        return 0;
    }


    int Store2Mysql::storeData(Store2Mysql *pS2m)
    {
        if (pS2m == nullptr)
        {
            LOG(ERROR, "Store2Mysql ptr is null!");
            return -1;
        }

        while (!pS2m->isStop())
        {
            ::usleep(10 * 1000);
            StoreData_T  sd;
            {
                std::unique_lock<std::mutex> lock(pS2m->mutex_);
                if (pS2m->dataQueue_.empty())
                {
                    // LOG(DEBUG, "data queue is empty...");
                    continue;
                }
                sd = pS2m->dataQueue_.front();
                pS2m->dataQueue_.pop();
            }
            if (!pS2m->dbPtr_)
            {
                LOG(WARN, "dbptr is null!");
                pS2m->stop();
                break;
            }
            /* data 为sql语句, 直接执行sql存储 */
            if (!pS2m->dbPtr_->execSql(sd.data))
            {
                LOG(WARN, "exec sql error! sql: [{}]", sd.data);
            }
        }

        return 0;
    }


}; /* dfsrrServer namespace end */
