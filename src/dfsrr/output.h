// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     output.h
// Author:       dingfang
// CreateDate:   2020-10-20 19:55:28
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 14:23:13
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "common/type.h"
#include "dfsrrModule.h"

#include <string>
#include <queue>
#include <mutex>

namespace dfsrr
{


    struct OutputData_T
    {
        std::string name;
        std::string data;
        UINT64      timestamp;
    };


    class Output
    {
    public:
        Output();
        virtual ~Output() { };

        virtual int convert(const mod::CollectData_T &, UINT64, std::string &);

        int addData(const std::string &name, UINT64 ts, const std::string &data);
        int sendData();

    protected:
        virtual int send(const OutputData_T &od) = 0;

    private:
        std::queue<OutputData_T> dataQueue_;
        std::mutex      mutex_;
        UINT32          maxSize_;
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_H__ */
