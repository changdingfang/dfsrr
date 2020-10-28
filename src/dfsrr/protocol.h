// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     protocol.h
// Author:       dingfang
// CreateDate:   2020-10-27 20:42:44
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 08:57:46
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "dflog/dflog.h"
#include "common/type.h"

#include <string.h>

#include <string>

namespace dfsrrProtocol
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

        bool deserializa(const std::string &data)
        {
            if (data.size() < PkgLen)
            {
                LOG(WARN, "deserializa data failed!");
                return false;
            }
            ::memcpy(&type, data.data(), 4);
            ::memcpy(&size, data.data() + 4, 4);
            msg = data.substr(PkgLen, size);

            return true;
        }

        std::string serializa()
        {
            char buff[PkgLen] = { 0 };
            ::memcpy(buff, &type, 4);
            ::memcpy(buff + 4, &size, 4);

            std::string s;
            s.assign(buff, PkgLen);
            s += msg;

            return std::move(s);
        }
    };


}; /* dfsrrProtocol namespace end */


#endif /* __PROTOCOL_H__ */
