// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     outputLocal.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 20:47:17
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 21:46:12
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrr/outputLocal.h"

using namespace std;

namespace dfsrr
{


    OutputLocal::OutputLocal(std::string datadir)
        : datadir_(datadir)
    {
    }


    OutputLocal::~OutputLocal()
    {
    }


    int OutputLocal::addData(const std::string &name, UINT64 ts, const std::string &data)
    {
        LocalData_T ld;
        ld.name         = name;
        ld.timestamp    = ts;
        ld.data         = data;

        std::unique_lock<std::mutex> lock(mutex_);
        if (dataQueue_.size() > maxSize_)
        {
            LOG(WARN, "queue full, discard data!");
            return -1;
        }
        dataQueue_.push(ld);

        return 0;
    }


    int OutputLocal::sendData()
    {
        LocalData_T ld;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (dataQueue_.empty())
            {
                LOG(DEBUG, "data queue is empty...");
                return 0;
            }
            ld = dataQueue_.front();
            dataQueue_.pop();
        }

        /* * * * * * * * * * * * * * * * * */
        /* 数据按天循环存储, 后面需要补充 */
        /* * * * * * * * * * * * * * * * * */


        string content(to_string(ld.timestamp));
        content += " ";
        content += ld.name;
        content += " ";
        content += ld.data;
        content += "\n";

        string filename(datadir_);
        filename += "/" + ld.name + ".data";
        FILE *fd = std::fopen(filename.c_str(), "ab");
        if (fd == nullptr)
        {
            LOG(WARN, "open file error! filename: {}", filename);
            return -1;
        }
        LOG(DEBUG, "write content to file...: [{}]", content);
        if (std::fwrite(content.data(), 1, content.size(), fd) != content.size())
        {
            LOG(WARN, "write file content error!");
            std::fclose(fd);
            return -1;
        }

        std::fflush(fd);
        std::fclose(fd);

        return 0;
    }


}; /* dfsrr namespace end */
