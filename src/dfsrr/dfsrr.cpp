// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 19:14:19
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 20:30:16
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
          , lastCheckTime_(0)

    {
        if (!this->parseConfig(conf))
        {
            throw ("parse config error!");
        }
    }


    Dfsrr::~Dfsrr()
    {
        this->stop();
        if (sendThreadPtr_)
        {
            sendThreadPtr_->join();
        }
    }


    void Dfsrr::run()
    {
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
                this->checkOutput();
            }
            ::usleep(config_.intv);
        }
    }


    bool Dfsrr::parseConfig(const json &confJson)
    {
        for (const auto &output : confJson["output"])
        {
            auto it = output.find("type");
            if (it == output.end())
            {
                LOG(WARN, "not found output type!");
                continue;
            }

            LOG(INFO, "output type: [{}]", *it);

            if (*it == "local")
            {
                auto pathdirIt = output.find("pathdir");
                if (pathdirIt == output.end())
                {
                    LOG(WARN, "local pathdir config failed!");
                    continue;
                }
                UINT64 rotate = 7 * 24 * 60 * 60;
                auto rotateIt = output.find("rotateTime");
                if (rotateIt != output.end())
                {
                    rotate = *rotateIt;
                    rotate *= 24 * 60 * 60;
                }
                config_.outputLocal = { true, *pathdirIt, rotate };
                common::createDir(config_.outputLocal.pathdir);
                outputVec_.push_back(make_shared<OutputLocal>(config_.outputLocal.pathdir, config_.outputLocal.rotate));
            }
            else if (*it == "tcp")
            {
                auto addrIt = output.find("addr");
                auto portIt = output.find("port");
                if (addrIt == output.end() || portIt == output.end())
                {
                    LOG(WARN, "tcp config failed!");
                    continue;
                }
                config_.outputTcp = { true, *addrIt, *portIt };

                try
                {
                    outputVec_.push_back(make_shared<OutputTcp>(config_.outputTcp.addr, config_.outputTcp.port));
                }
                catch (const char *e)
                {
                    LOG(WARN, "new output tcp failed! error: [{}]", e);
                }
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
            auto it = p->outputVec_.begin();
            for ( ; it != p->outputVec_.end(); )
            {
                if ((*it)->sendData() != 0)
                {
                    it = p->outputVec_.erase(it);
                    continue;
                }
                ++it;
            }
            ::usleep(p->config_.intv * 3);
        }

        return 0;
    }


    bool Dfsrr::checkOutput()
    {
        time_t nowTime = ::time(nullptr);

        if (nowTime - lastCheckTime_ < config_.checkOutputIntv_)
        {
            return true;
        }

        lastCheckTime_ = nowTime;
        bool local = false, tcp = false;

        for (const auto &op : outputVec_)
        {
            if (config_.outputLocal.use && dynamic_cast<OutputLocal *>(op.get()) != nullptr)
            {
                local = true;
            }

            if (config_.outputTcp.use && dynamic_cast<OutputTcp *>(op.get()) != nullptr)
            { 
                tcp = true;
            }
        }

        if (config_.outputLocal.use && !local)
        {
            LOG(WARN, "use local output, but output local ptr failed!, pathDir: [{}]", config_.outputLocal.pathdir);
            common::createDir(config_.outputLocal.pathdir);
            outputVec_.push_back(make_shared<OutputLocal>(config_.outputLocal.pathdir, config_.outputLocal.rotate));
        }

        if (config_.outputTcp.use && !tcp)
        {
            try
            {
                LOG(WARN, "use tcp output, but output tcp ptr failed!, addr: [{}], port: [{}]"
                        , config_.outputTcp.addr
                        , config_.outputTcp.port);
                outputVec_.push_back(make_shared<OutputTcp>(config_.outputTcp.addr, config_.outputTcp.port));
            }
            catch(...) { };
        }
        LOG(INFO, "output vec.size: [{}]", outputVec_.size());

        return true;
    }


}; /* dfsrr namespace end */
