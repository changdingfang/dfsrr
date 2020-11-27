// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     test_sqlite.cpp
// Author:       dingfang
// CreateDate:   2020-10-21 20:18:32
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-02 21:00:29
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/database.h"

#include <string>

using namespace std;
using namespace common;

int callback(void *, int argc, char **argv, char **azColName)
{
    static bool first = true;
    if (first)
    {
        first = false;
        for (int i = 0; i < argc; ++i)
        {
            LOG(INFO, "{}\t\t", azColName[i]);
        }
        LOG(INFO, "");
    }
    for (int i = 0; i < argc; ++i)
    {
        if (strlen(argv[i]) > 7)
        {
            LOG(INFO, "{}\t", argv[i] ? argv[i] : nullptr);
        }
        else
        {
            LOG(INFO, "{}\t\t", argv[i] ? argv[i] : nullptr);
        }
    }

    return 0;
}


int testSqlite()
{
    try
    {
        Database db("./dbsqlite.db");
        const char *createTabSql = "create table dftest( \
                           id int primary key not null, \
                           name text not null, \
                           age int not null);";
        if (db.execSql(createTabSql, callback) != 0)
        {
            LOG(ERROR, "exec sql error!");
            exit(-1);
        }

        const char *insertDataSql = "insert into dftest( \
                           id, name, age \
                           ) values (6, 'dingfang3', 26),(7, 'hello', 2);";
        if (db.execSql(insertDataSql, callback) != 0)
        {
            LOG(ERROR, "exec sql error!");
            exit(-1);
        }

        const char *selectDataSql = "select * from dftest";

        if (db.execSql(selectDataSql, callback) != 0)
        {
            LOG(ERROR, "exec sql error!");
            exit(-1);
        }
    }
    catch (...)
    {
        LOG(CRITICAL, "init database class failed!");
        exit (-1);
    }

    LOG(INFO, "sql exec successfully");

    return 0;
}


int main(void)
{
    dflog::InitLog("./sqlite.log", dflog::loggerOption::CONSOLE);
    dflog::SetLevel(DEBUG);
    testSqlite();

    return 0;
}
