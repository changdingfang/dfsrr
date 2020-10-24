// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputTcp.h
// Author:       dingfang
// CreateDate:   2020-10-24 10:49:15
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 15:16:45
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_TCP_H__
#define __OUTPUT_TCP_H__

#include "output.h"
#include "common/network.h"

namespace dfsrr
{


    class OutputTcp final
        : public Output
    {
    public:
        OutputTcp(const std::string &addr, unsigned short port);
        virtual ~OutputTcp();

        virtual int convert(const mod::CollectData_T &, UINT64 timestamp,  std::string &) override;
        virtual int send(const OutputData_T &od) override;

    private:
        common::Network *pNet_;
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_TCP_H__ */
