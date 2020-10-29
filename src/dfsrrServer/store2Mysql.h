// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     store2Mysql.h
// Author:       dingfang
// CreateDate:   2020-10-29 19:50:46
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 17:28:12
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __STORE2_MYSQL_H__
#define __STORE2_MYSQL_H__

#include "common/mysqldb.h"

#include <string>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>

namespace dfsrrServer
{


    struct StoreData_T
    {
        std::string data;
    };

    
    class Store2Mysql
    {
    public:
        Store2Mysql(const common::DBInfo_T &dbinfo);
        ~Store2Mysql();

        int convert(const std::string &data, std::string &dst);
        int addData(const std::string &data);

        inline void stop() { stop_ = true; }
    private:
        inline bool isStop() const { return stop_; }

        static int storeData(Store2Mysql *);

    private:
        bool stop_;
        std::shared_ptr<common::MysqlDB> dbPtr_;
        std::queue<StoreData_T> dataQueue_;
        UINT32          maxSize_;
        std::mutex      mutex_;
        std::thread     th_;
    };


}; /* dfsrrServer namespace end */


#endif /* __STORE2_MYSQL_H__ */
