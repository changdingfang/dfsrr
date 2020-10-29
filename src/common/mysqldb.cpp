// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     mysqldb.cpp
// Author:       dingfang
// CreateDate:   2020-10-29 19:59:16
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 20:03:58
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/mysqldb.h"

using namespace std;

namespace common
{


    MysqlDB::MysqlDB(const DBInfo_T &dbinfo)
        : dbinfo_(dbinfo)
          , conn_(nullptr)
    {
        LOG(INFO, "init mysql database...");
        MYSQL *conn = ::mysql_init(nullptr);
        if (!::mysql_real_connect(conn
                    , dbinfo.ip.c_str()
                    , dbinfo.dbUser.c_str()
                    , dbinfo.dbUserPwd.c_str()
                    , dbinfo.dbName.c_str()
                    , dbinfo.port
                    , nullptr
                    , 0))
        {
            LOG(ERROR, "connecting to database error: [{}]", ::mysql_error(conn));
            throw("connecting to database failed!");
        }
        mysql_set_character_set(conn, "utf8");
        conn_ = conn;
        LOG(INFO, "connect msyql database successful!");
    }


    MysqlDB::~MysqlDB()
    {
        if (conn_)
        {
            mysql_close(conn_);
            conn_ = nullptr;
        }
    }


    bool MysqlDB::execSql(const string &sql)
    {
        if (sql.empty())
        {
            LOG(WARN, "sql is emtpy!");
            return false;
        }
        if (::mysql_real_query(conn_, sql.data(), sql.size()) != 0)
        {
            LOG(ERROR, "exec sql failed! sql: [{}], error: [{}]", sql, ::mysql_error(conn_));
            return false;
        }

        MYSQL_RES *result   = nullptr;
        MYSQL_ROW row       = nullptr;

        result = ::mysql_use_result(conn_);
        if (result)
        {
            // int numFields   = ::mysql_num_fields(result);
            // int numRows     = ::mysql_num_rows(result);

            while ((row = ::mysql_fetch_row(result)) != nullptr)
            {
                /* select result */
                /* row[0]; */
                /* row[1]; */
                /* row[2]; */
            }
            ::mysql_free_result(result);
        }
        else
        {
            if (::mysql_field_count(conn_) == 0)
            {
                /* update insert, delete */
                LOG(DEBUG, "successful! rows: [{}]", ::mysql_affected_rows(conn_));
            }
            else
            {
                LOG(ERROR, "get result error: [{}]", ::mysql_error(conn_));
                return false;
            }
        }

        return true;
    }


}; /* common namespace end */
