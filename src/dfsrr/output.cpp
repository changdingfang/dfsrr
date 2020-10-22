// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     output.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 20:15:22
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-22 19:35:27
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "output.h"

using namespace std;

namespace dfsrr
{


        int Output::convert(const mod::CollectData_T &cd, UINT64, std::string &data)
        {
            string d("");
            for (const auto &data : cd.dataVec)
            {
                for (const auto &modStat : data.modStatVec)
                {
                    d += modStat.key;
                    d += " ";
                    d += to_string(modStat.value);
                    d += " ";
                }

                for (const auto &tag : data.modStatTagVec)
                {
                    d += tag.key;
                    d += " ";
                    d += tag.value;
                    d += " ";
                }
            }

            data = std::move(d);

            return 0;
        }


}; /* dfsrr namespace end */
