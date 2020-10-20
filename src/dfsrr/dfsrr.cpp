// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     dfsrr.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 19:14:19
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 21:40:48
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dfsrr/dfsrr.h"
#include "dfsrr/outputLocal.h"
#include "nlohmann/json.hpp"

#include <unistd.h>

#include <fstream>

using namespace std;
using namespace mod;
using json = nlohmann::json;

namespace dfsrr
{


    Dfsrr::Dfsrr(std::string configFile)
        : stop_(false)
          , configFile_(configFile)
          , config_()
    {
        if (!this->parseConfig())
        {
            throw ("parse config error!");
        }
    }


    void Dfsrr::run()
    {
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

                for (auto &output : outputVec_)
                {
                    string data;
                    output->conver(cd, data);
                    LOG(DEBUG, "data: [{}]", data);
                    output->addData(cd.moduleName, 100, data);
                    output->sendData();
                }
            }
            ::usleep(config_.intv);
        }
    }


    bool Dfsrr::parseConfig()
    {
        if (configFile_.empty())
        {
            LOG(WARN, "config file is empty");
            return false;
        }

        std::ifstream fin;
        fin.open(configFile_, std::ios_base::in);
        if (fin.fail())
        {
            LOG(WARN, "open config file failed!");
            return false;
        }

        string configContent(""), line;

        while (getline(fin, line))
        {
            configContent += line;
        }

        fin.close();

        auto confJson = json::parse(configContent);

        config_.intv = 10 * 1000;

        for (const auto &output : confJson["output"])
        {
            LOG(DEBUG, "{}", output["type"]);
            if (output["type"] == "local")
            {
                config_.outputLocal = { true, output["pathdir"] };

                outputVec_.push_back(make_shared<OutputLocal>(config_.outputLocal.pathdir));
            }
            else if (output["type"] == "tcp")
            {
                config_.outputTcp = { true, output["addr"], output["port"] };
                /* */
            }
        }

        for (const auto &mod : confJson["module"])
        {
            moduleVec_.push_back({ make_shared<DfSrrModule>(mod["module"]), mod["module"], mod["name"], mod["interval"], mod["filter"], 0 });
        }

        return true;
    }


}; /* dfsrr namespace end */
