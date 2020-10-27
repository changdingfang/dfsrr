// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     database.h
// Author:       dingfang
// CreateDate:   2020-10-22 19:31:05
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-27 22:22:51
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "sqlite3/sqlite3.h"

#include <string>

namespace common
{


    class Database
    {
    public:
        Database(std::string dbfile);
        ~Database();

        int execSql(const std::string &sql);
        int execSql(const std::string &sql, ::sqlite3_callback cb);
        int execSql(const std::string &sql, ::sqlite3_callback cb, void *data);
        int execSql(const std::string &sql, ::sqlite3_callback cb, void *data, std::string &errMsg);

    private:
        std::string     dbfile_;
        ::sqlite3       *handle_;
    };


}; /* common namespace end */


#endif /* __DATABASE_H__ */
