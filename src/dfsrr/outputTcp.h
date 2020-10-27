// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputTcp.h
// Author:       dingfang
// CreateDate:   2020-10-24 10:49:15
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-27 21:15:24
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __OUTPUT_TCP_H__
#define __OUTPUT_TCP_H__

#include "output.h"
#include "common/network.h"

#include <memory>

namespace dfsrr
{


    static const int PkgLen = 8;

    enum Protocol_E
    {
        MODULE_DATA = 5,
    };


    struct TcpPackage_T
    {
        UINT32 type;
        UINT32 size;
        std::string msg;

        bool deserializa(const std::string &data);
        std::string serializa();
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
    };


}; /* dfsrr namespace end */


#endif /* __OUTPUT_TCP_H__ */
