// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     partition.h
// Author:       dingfang
// CreateDate:   2020-10-16 19:07:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 19:35:00
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __PARTITION_H__
#define __PARTITION_H__

#include "module.h"

namespace mod
{


    class Partition
        : public Module
    {
    public:
        virtual std::map<std::string, double> collect() {};
    };


}; /* mod namespace end */


#endif /* __PARTITION_H__ */
