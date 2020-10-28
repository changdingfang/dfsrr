// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputTcp.h
// Author:       dingfang
// CreateDate:   2020-10-24 10:49:15
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 20:31:31
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_TCP_H__
#define __OUTPUT_TCP_H__

#include "dfsrr/output.h"
#include "common/network.h"
#include "dfsrr/protocol.h"

#include <memory>
#include <vector>

namespace dfsrr
{


    struct LocalInfo_T
    {
        std::string hostname;
        std::vector<std::string> ipVec;
    };


    class OutputTcp final
        : public Output
    {
    public:
        OutputTcp(const std::string &addr, unsigned short port);
        virtual ~OutputTcp();

        virtual int convert(const mod::CollectData_T &, UINT64 timestamp,  std::string &) override;

    private:
        virtual int send(const OutputData_T &od) override;

    private:
        std::unique_ptr<common::Network> netPtr_;
        LocalInfo_T li_;
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_TCP_H__ */
