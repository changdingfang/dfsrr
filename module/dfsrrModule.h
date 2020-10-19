// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrrModule.h
// Author:       dingfang
// CreateDate:   2020-10-14 19:16:03
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-19 21:34:52
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __DFSRR_MODULE_H__
#define __DFSRR_MODULE_H__

#include "dflog/dflog.h"
#include "cpu.h"
#include "memory.h"
#include "load.h"
#include "tcp.h"
#include "udp.h"
#include "partition.h"
#include "traffic.h"

#include <string>
#include <map>

namespace mod
{

    
    class DfSrrModule
    {
    public:
        DfSrrModule(const std::string &name)
        {
            name_ = name;
            if (name == "cpu")
            {
                module_ = new Cpu();
            }
            else if (name == "memory" || name == "mem")
            {
                module_ = new Memory();
            }
            else if (name == "tcp")
            {
                module_ = new Tcp();
            }
            else if (name == "udp")
            {
                module_ = new Udp();
            }
            else if (name == "load")
            {
                module_ = new Load();
            }
            else if (name == "partition")
            {
                module_ = new Partition();
            }
            else if (name == "traffic")
            {
                module_ = new Traffic();
            }
            else
            {
                LOG(WARN, "not found module name!");
            }
        }

        ~DfSrrModule()
        {
            delete module_;
            module_ = nullptr;
        }

        CollectData_T collect()
        {
            if (module_ == nullptr)
            {
                LOG(WARN, "module not found!");
                return CollectData_T();
            }

            return std::move(module_->collect());
        }

    private:
        std::string name_;
        Module * module_ { nullptr };
    };


}; /* mod namespace end */


#endif /* __DFSRR_MODULE_H__ */
