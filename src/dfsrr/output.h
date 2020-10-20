// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     output.h
// Author:       dingfang
// CreateDate:   2020-10-20 19:55:28
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 21:46:16
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include "common/type.h"
#include "dfsrrModule.h"

#include <string>

namespace dfsrr
{


    class Output
    {
    public:
        Output() = default;
        virtual ~Output() { };

        virtual int conver(const mod::CollectData_T &, std::string &);
        virtual int addData(const std::string &name, UINT64 ts, const std::string &data) = 0;
        virtual int sendData() = 0;
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_H__ */
