// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputLocal.h
// Author:       dingfang
// CreateDate:   2020-10-20 20:17:20
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 15:10:54
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_LOCAL_H__
#define __OUTPUT_LOCAL_H__

#include "dfsrr/output.h"
#include "common/database.h"

namespace dfsrr
{


    static const std::string tableSuffix = "_stat";


    class OutputLocal final
        : public Output
    {
    public:
        OutputLocal(std::string filename);
        virtual ~OutputLocal();

        virtual int convert(const mod::CollectData_T &, UINT64 timestamp,  std::string &) override;

    private:
        virtual int send(const OutputData_T &od) override;
        int createTable(const std::string &tbName, const std::string &rawSql, common::Database &db);

    private:
        std::string     datadir_;
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_LOCAL_H__ */
