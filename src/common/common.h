// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     common.h
// Author:       dingfang
// CreateDate:   2020-10-21 08:42:00
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-21 08:45:19
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/stat.h>

#include <string>


namespace common
{


    static bool mkdir_(const std::string &path)
    {
        return (::mkdir(path.c_str(), 0755) == 0);
    }


    static bool pathExists(const std::string &path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }


    static bool createDir(std::string path)
    {
        if (path.empty())
        {
            return false;
        }

        if (pathExists(path))
        {
            return true;
        }

        size_t searchOffset = 0;
        do
        {
            std::string::size_type pos = path.find('/', searchOffset);
            if (pos == std::string::npos)
            {
                pos = path.size();
            }
            std::string subdir = path.substr(0, pos);
            if (!subdir.empty() && !pathExists(subdir) && !mkdir_(subdir))
            {
                return false;
            }
            searchOffset = pos + 1;
        } while (searchOffset < path.size());

        return true;
    }


}; /* common namespace end */

#endif /* __COMMON_H__ */
