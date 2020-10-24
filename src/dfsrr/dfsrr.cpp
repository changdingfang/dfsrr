// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 19:14:19
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-24 16:09:07
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dfsrr/dfsrr.h"
#include "common/common.h"
#include "dfsrr/outputLocal.h"
#include "dfsrr/outputTcp.h"

#include <unistd.h>

using namespace std;
using namespace mod;
using json = nlohmann::json;

namespace dfsrr
{


    Dfsrr::Dfsrr(json conf)
        : stop_(false)
          , config_()
    {
        if (!this->parseConfig(conf))
        {
            throw ("parse config error!");
        }
    }


    void Dfsrr::run()
    {
        stop_ =  false;

        sendThreadPtr_ = make_shared<std::thread>(Dfsrr::sendData, this);

        while (!this->isStop())
        {
            for (auto &mod : moduleVec_)
            {
                time_t nowTime = time(nullptr);
                if (nowTime - mod.lastTime < mod.intv)
                {
                    continue;
                }
                mod.lastTime = nowTime;
                CollectData_T cd = mod.modPtr->collect();
                if (cd.dataVec.empty())
                {
                    LOG(WARN, "collect data is null! module: [{}]", cd.moduleName);
                    continue;
                }

                for (auto &output : outputVec_)
                {
                    string data;
                    output->convert(cd, mod.lastTime, data);
                    // LOG(DEBUG, "data: [{}]", data);
                    output->addData(cd.moduleName, mod.lastTime, data);
                }
            }
            ::usleep(config_.intv);
        }

        sendThreadPtr_->join();
    }


    bool Dfsrr::parseConfig(const json &confJson)
    {
        config_.intv = 10 * 1000;

        for (const auto &output : confJson["output"])
        {
            try
            {
                LOG(INFO, "output type: [{}]", output.at("type"));
                if (output.at("type") == "local")
                {
                    config_.outputLocal = { true, output.at("pathdir") };
                    common::createDir(config_.outputLocal.pathdir);
                    outputVec_.push_back(make_shared<OutputLocal>(config_.outputLocal.pathdir));
                }
                else if (output.at("type") == "tcp")
                {
                    config_.outputTcp = { true, output.at("addr"), output.at("port") };
                    outputVec_.push_back(make_shared<OutputTcp>(config_.outputTcp.addr, config_.outputTcp.port));
                }
            }
            catch(...)
            {
                LOG(WARN, "not found output type!");
                continue;
            }
        }

        for (const auto &mod : confJson["module"])
        {
            try
            {
                moduleVec_.push_back(
                        {
                        make_shared<DfSrrModule>(mod.at("module"))
                        , mod.at("module")
                        , mod.at("name")
                        , mod.at("interval")
                        , mod.at("filter")
                        , 0 
                        });
            }
            catch(...)
            {
                LOG(WARN, "have module failed!");
                continue;
            }
        }

        return true;
    }


    int Dfsrr::sendData(Dfsrr *p)
    {
        LOG(INFO, "send data!");
        if (p == nullptr)
        {
            LOG(ERROR, "dfsrr ptr is null");
            return -1;
        }

        while (!p->isStop())
        {
            for (auto &output : p->outputVec_)
            {
                output->sendData();
            }
            ::usleep(p->config_.intv * 3);
        }

        return 0;
    }


}; /* dfsrr namespace end */
