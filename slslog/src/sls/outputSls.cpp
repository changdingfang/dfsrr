// =======================================================================
//  FileName:     outputSls.cpp
//  Author:       dingfang
//  CreateDate:   2021-01-05 20:50:39
//  ModifyAuthor: dingfang
//  ModifyDate:   2021-01-05 22:00:19
// =======================================================================

#include "outputSls.h"

#include "dflog/dflog.h"
#include "log_util.h"
#include "log_api.h"
#include "log_define.h"


#include <string.h>
#include <stdio.h>

#include <iostream>

using namespace std;
using namespace sls;

Sls::Sls()
{
    sls_log_init(LOG_GLOBAL_ALL);
}

Sls::~Sls()
{
    sls_log_destroy();
}


int Sls::send(const std::string &id
        , const std::string &secret
        , const std::string &endpoint
        , const std::string &project
        , const std::string &logstore
        , std::list<std::map<std::string, std::string>> dataList
        , const std::string &topic
        , const std::string &source
        , std::string &error)
{
    log_group_builder *bder = log_group_create();
    add_source(bder, source.c_str(), source.size());
    add_topic(bder, topic.c_str(), topic.size());

    if (dataList.empty())
    {
        return 0;
    }

    /* 添加标签 */
    /* add_tag(bder, "taga_key", strlen("taga_key"), "taga_value", */
    /*         strlen("taga_value")); */
    /* add_tag(bder, "tagb_key", strlen("tagb_key"), "tagb_value", */
    /*         strlen("tagb_value")); */
    /* add_pack_id(bder, "123456789ABC", strlen("123456789ABC"), 0); */

    for (auto &dataMap : dataList)
    {
        int keylen  = dataMap.size();
        char **keys     = new char* [keylen * sizeof(char *)]();
        size_t *keyLens = new size_t [keylen * sizeof(size_t *)]();
        char **values   = new char* [keylen * sizeof(char *)]();
        size_t *valLens = new size_t [keylen * sizeof(size_t *)]();

        int j = 0;
        for (const auto &it : dataMap)
        {
            keys[j]     = (char *)it.first.c_str();
            values[j]   = (char *)it.second.c_str();
            keyLens[j]  = it.first.size();
            valLens[j]  = it.second.size();
            ++j;
        }
        add_log_full(bder, time(nullptr), keylen, keys, keyLens, values, valLens);

        delete [] keys;
        delete [] keyLens;
        delete [] values;
        delete [] valLens;
    }

    log_post_option option;
    ::memset(&option, 0x00, sizeof(log_post_option));
    option.interface        = nullptr;
    option.connect_timeout  = 15;
    option.operation_timeout= 15;
    option.compress_type = 1;

    lz4_log_buf *pLZ4Buf = nullptr;
    pLZ4Buf = serialize_to_proto_buf_with_malloc_lz4(bder);

    log_group_destroy(bder);
    if (pLZ4Buf == nullptr)
    {
        LOG(ERROR, "serialize_to_proto_buf_with_malloc_lz4 failed");
        error = "serialize_to_proto_buf_with_malloc_lz4 failed";
        return -1;
    }
    post_log_result * rst = post_logs_from_lz4buf(endpoint.c_str(), id.c_str(),
            secret.c_str(), nullptr,
            project.c_str(), logstore.c_str(),
            pLZ4Buf, &option);
    LOG(INFO, "result {}", rst->statusCode);

    auto code = rst->errorMessage;
    post_log_result_destroy(rst);
    free_lz4_log_buf(pLZ4Buf);

    if (code != nullptr)
    {
        LOG(ERROR, "error message {}", rst->errorMessage);
        error = "error message: ";
        error += rst->errorMessage;
        return -1;
    }

    return 0;
}

