// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     database.cpp
// Author:       dingfang
// CreateDate:   2020-10-22 19:33:54
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-22 19:37:00
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "database.h"

using namespace std;

namespace common
{


    Database::Database(string dbfile)
        : dbfile_(dbfile)
          , handle_(nullptr)
    {
        if (::sqlite3_open(dbfile_.c_str(), &handle_))
        {
            LOG(CRITICAL, "open database file failed!");
            throw("open database file failed!");
        }
    }


    Database::~Database()
    {
        if (handle_)
        {
            ::sqlite3_close(handle_);
            handle_ = nullptr;
        }
    }


    int Database::execSql(const std::string &sql)
    {
        string errMsg;
        return this->execSql(sql, nullptr, nullptr, errMsg);
    }


    int Database::execSql(const std::string &sql, ::sqlite3_callback cb)
    {
        string errMsg;
        return this->execSql(sql, cb, nullptr, errMsg);
    }


    int Database::execSql(const std::string &sql, ::sqlite3_callback cb, void *data)
    {
        string errMsg;
        return this->execSql(sql, cb, data, errMsg);
    }


    int Database::execSql(const std::string &sql, ::sqlite3_callback cb, void *data, std::string &errMsg)
    {
        char *em = nullptr;
        int ret = ::sqlite3_exec(handle_, sql.c_str(), cb, data, &em);
        if (ret)
        {
            errMsg = em;
            LOG(ERROR, "exec sql error! ret: [{}] msg: [{}]", ret, errMsg);
        }

        return ret;
    }


}; /* common namespace end */
