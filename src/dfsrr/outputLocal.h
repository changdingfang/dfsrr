// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputLocal.h
// Author:       dingfang
// CreateDate:   2020-10-20 20:17:20
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 21:46:06
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_LOCAL_H__
#define __OUTPUT_LOCAL_H__

#include "dfsrr/output.h"

#include <queue>
#include <mutex>
#include <condition_variable>

namespace dfsrr
{


    struct LocalData_T
    {
        std::string name;
        std::string data;
        UINT64      timestamp;
    };


    class OutputLocal final
        : public Output
    {
    public:
        OutputLocal(std::string filename);
        virtual ~OutputLocal();

        virtual int addData(const std::string &name, UINT64 ts, const std::string &data) override;
        virtual int sendData() override;

    private:
        std::string     datadir_;
        std::queue<LocalData_T> dataQueue_;
        std::mutex      mutex_;
        UINT32          maxSize_;
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_LOCAL_H__ */
