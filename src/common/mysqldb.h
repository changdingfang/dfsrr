// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     mysqldb.h
// Author:       dingfang
// CreateDate:   2020-10-29 19:54:34
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-29 20:03:55
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __MYSQLDB_H__
#define __MYSQLDB_H__

#include "common/type.h"

#include <mysql/mysql.h>

#include <string>

namespace common
{


    struct DBInfo_T
    {
        std::string  ip;
	UINT32       port;
	std::string  dbName;
	std::string  dbUser;
	std::string  dbUserPwd;
    };


    class MysqlDB
    {
    public:
        explicit MysqlDB(const DBInfo_T &dbinfo);
        ~MysqlDB();

        bool execSql(const std::string &sql);

    private:
        DBInfo_T    dbinfo_;
        MYSQL       *conn_;
    };


}; /* common namespace end */


#endif /* __MYSQLDB_H__ */
