// =======================================================================
//  FileName:     outputSls.h
//  Author:       dingfang
//  CreateDate:   2021-01-05 20:41:38
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-01-05 21:43:53
// =======================================================================

#ifndef __OUTPUT_SLS_H__
#define __OUTPUT_SLS_H__

#include <string>
#include <list>
#include <map>

namespace sls
{


    class Sls
    {
    public:
        Sls();
        ~Sls();
        int send(const std::string &id
                , const std::string &secret
                , const std::string &endpoint
                , const std::string &project
                , const std::string &logstore
                , std::list<std::map<std::string, std::string>> dataList
                , const std::string &topic
                , const std::string &source
                , std::string &error);
    };


}; /* namespace end of sls */


#endif /* __OUTPUT_SLS_H__ */
