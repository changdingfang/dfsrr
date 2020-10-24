// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputLocal.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 20:47:17
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 13:57:33
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrr/outputLocal.h"

using namespace std;
using namespace common;

namespace dfsrr
{


    OutputLocal::OutputLocal(std::string datadir)
        : datadir_(datadir)
    {
    }


    OutputLocal::~OutputLocal()
    {
    }


    int OutputLocal::convert(const mod::CollectData_T &cd, UINT64 timestamp, std::string &data)
    {
        string sql;

        for (const auto &data : cd.dataVec)
        {
            string tmpSql("insert into "), fields("(timestamp,"), values(" values(");
            tmpSql += cd.moduleName + tableSuffix;
            values += to_string(timestamp) + ",";
            for (const auto &modStat : data.modStatVec)
            {
                fields += modStat.key + ",";
                values += to_string(modStat.value) + ",";
            }

            for (const auto &tag : data.modStatTagVec)
            {
                fields += tag.key + ",";
                values += "'" + tag.value + "',";
            }
            fields = fields.substr(0, fields.size() - 1) + ")";
            values = values.substr(0, values.size() - 1) + ");";
            tmpSql += fields + " " + values;
            sql += tmpSql;
        }

        data = std::move(sql);

        return 0;
    }


    int OutputLocal::send(const OutputData_T &od)
    {
        /* * * * * * * * * * * * * * * * * */
        /* 数据按天循环存储, 后面需要补充 */
        /* * * * * * * * * * * * * * * * * */


        string filename(datadir_);
        filename += "/" + od.name + ".db";

        string sql(od.data);
        try
        {
            Database db(filename);
            string errMsg;
            int ret = 0;
            int errCount = 2;

            do
            {
                ret = db.execSql(sql, nullptr, nullptr, errMsg);
                if (ret)
                {
                    if (errCount-- && !errMsg.compare(0, 14, "no such table:"))
                    {
                        if (!this->createTable(od.name + tableSuffix, sql, db))
                        {
                            continue;
                        }
                        LOG(ERROR, "create table failed!");
                    }
                    LOG(ERROR, "exec sql error! sql: [{}]", sql);
                    return -1;
                }
            } while (ret);
        }
        catch (...)
        {
            LOG(ERROR, "write data failed! sql: [{}]", sql);
            return -1;
        }

        return 0;
    }


    int OutputLocal::createTable(const std::string &tbName, const std::string &rawSql, Database &db)
    {
        LOG(INFO, "will create table...");
        string sql("create table ");
        sql += tbName + "(timestamp INTEGER NOT NULL,";
        // "insert into cpu_stat (timestamp,user,sys,idle), values(12334,1.1,2.2,3.3);
        string::size_type fieldStart = rawSql.find_first_of(",");
        string::size_type endPos = rawSql.find_first_of(")");
        while (fieldStart != string::npos)
        {
            string::size_type fieldEnd = rawSql.find_first_of(",", fieldStart + 1);
            if (fieldEnd == string::npos || fieldEnd > endPos)
            {
                break;
            }
            sql += rawSql.substr(fieldStart + 1, fieldEnd - fieldStart - 1);
            fieldStart = fieldEnd;

            sql += rawSql[fieldStart + 1] == '\'' ?  " TEXT NOT NULL," : " REAL NOT NULL,";
        }
           
        sql += rawSql.substr(fieldStart + 1, endPos - fieldStart - 1);
        sql += rawSql[fieldStart + 1] == '\'' ?  " TEXT NOT NULL," : " REAL NOT NULL,";
        sql = sql.substr(0, sql.size() - 1);

        sql += ");";
        LOG(INFO, "create sql: [{}]", sql);

        return db.execSql(sql);
    }


}; /* dfsrr namespace end */
