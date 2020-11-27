// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     test_mysql.cpp
// Author:       dingfang
// CreateDate:   2020-10-30 19:52:14
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-02 20:31:38
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "common/mysqldb.h"
#include "dflog/dflog.h"
#include "nlohmann/json.hpp"

#include <string>

using namespace std;
using namespace common;
using json = nlohmann::json;


void testMysql()
{
    DBInfo_T dbinfo = { "127.0.0.1", 3306, "dfsrr_test", "dfsrr", "1" };

    try
    {
        MysqlDB db(dbinfo);
        string sql;
        json jdata;
        sql += "show tables";
        if (!db.execSql(sql, jdata))
        {
            LOG(ERROR, "exec sql error!");
            return ;
        }
        LOG(INFO, "exec sql successful!");
        LOG(INFO, "data: {}", jdata.dump(4));
    }
    catch (...)
    {
        LOG(CRITICAL, "connect mysql database failed!");
        exit(-1);
    }
}


int main(void)
{
    dflog::InitLog("./mysqldbtest.log", dflog::loggerOption::CONSOLE);
    dflog::SetLevel(DEBUG);

    LOG(DEBUG, "sqlite test is running...");

    testMysql();

    return 0;
}
