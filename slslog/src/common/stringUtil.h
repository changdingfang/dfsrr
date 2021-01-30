// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     stringUtil.h
// Author:       dingfang
// CreateDate:   2020-11-01 10:53:47
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 11:48:28
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <string>
#include <vector>

namespace common
{


    static std::vector<std::string> split(const std::string &str, const std::string &pattern)
    {
        if (str.empty() || pattern.empty() || pattern.size() > str.size())
        {
            return std::vector<std::string>();
        }

        std::vector<std::string> result;
        std::string::size_type posStart  = 0;
        std::string::size_type posEnd    = -1;

        while (posStart != std::string::npos)
        {
            posStart = posEnd;
            posEnd = str.find(pattern, posStart + 1);
            result.push_back(str.substr(posStart + 1, posEnd - posStart - 1));
            posStart = posEnd;
        }

        return result;
    }


    static inline bool isBlank(const char c)
    {
        return c == ' ' || c == '\0' || c == '\n' || c == '\t';
    }


    /* 去除左右两边空白字符 */
    static std::string strip(const std::string &str)
    {
        std::string::size_type i = 0;
        for (; i < str.size(); ++i)
        {
            if (!isBlank(str[i]))
            {
                break;
            }
        }

        std::string::size_type j = str.size() - 1;
        for (; j >= 0 && j != std::string::npos; --j)
        {
            if (!isBlank(str[j]))
            {
                break;
            }
        }

        return str.substr(i, j - i + 1);
    }


}; /* common namespace end */

#endif /* __STRING_UTIL_H__ */
