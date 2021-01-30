// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     mysqldb.cpp
// Author:       dingfang
// CreateDate:   2020-10-29 19:59:16
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 09:56:40
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/mysqldb.h"

using namespace std;
using json = nlohmann::json;

namespace common
{


    MysqlDB::MysqlDB(const DBInfo_T &dbinfo)
        : dbinfo_(dbinfo)
          , conn_(nullptr)
    {
        if (!this->connect())
        {
            throw("connecting to database failed!");
        }
    }


    MysqlDB::~MysqlDB()
    {
        this->close();
    }


    bool MysqlDB::connect()
    {
        LOG(INFO, "init mysql database...");
        MYSQL *conn = ::mysql_init(nullptr);
        if (!::mysql_real_connect(conn
                    , dbinfo_.ip.c_str()
                    , dbinfo_.dbUser.c_str()
                    , dbinfo_.dbUserPwd.c_str()
                    , dbinfo_.dbName.c_str()
                    , dbinfo_.port
                    , nullptr
                    , 0))
        {
            LOG(ERROR, "connecting to database error: [{}], addr: [{}], port: [{}], dbname: [{}], user: [{}]"
                    , ::mysql_error(conn)
                    , dbinfo_.ip
                    , dbinfo_.port
                    , dbinfo_.dbName
                    , dbinfo_.dbUser);
            return false;
        }

        mysql_set_character_set(conn, "utf8");
        conn_ = conn;
        LOG(INFO, "connect msyql database successful!");

        return true;
    }


    bool MysqlDB::reconnect()
    {
        this->close();
        return this->connect();
    }


    void MysqlDB::close()
    {
        if (conn_)
        {
            ::mysql_close(conn_);
            conn_ = nullptr;
        }
    }


    bool MysqlDB::execSql(const std::string &sql)
    {
        json jdata;
        return execSql(sql, jdata);
    }


    bool MysqlDB::execSql(const string &sql, json &jdata)
    {
        if (sql.empty())
        {
            LOG(WARN, "sql is emtpy!");
            return false;
        }
        if (::mysql_real_query(conn_, sql.data(), sql.size()) != 0)
        {
            string errMsg(::mysql_error(conn_));
            LOG(ERROR, "exec sql failed! sql: [{}], error: [{}]", sql, errMsg);
            if (errMsg == "MySQL server has gone away")
            {
                if (!this->reconnect())
                {
                    LOG(ERROR, "reconnect mysql failed!");
                    return false;
                }
            }
            return false;
        }

        MYSQL_RES *result   = nullptr;
        MYSQL_ROW row       = nullptr;

        result = ::mysql_use_result(conn_);
        if (result)
        {
            int numFields   = ::mysql_num_fields(result);
            // int numRows     = ::mysql_num_rows(result);

            MYSQL_FIELD *fields = ::mysql_fetch_field(result);
            while ((row = ::mysql_fetch_row(result)) != nullptr)
            {
                json jrow;
                for (int i = 0; i < numFields; ++i)
                {
                    jrow[fields[i].name] = row[i];
                }
                jdata.push_back(jrow);
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
