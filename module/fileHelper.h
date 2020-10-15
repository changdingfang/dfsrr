// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     fileHelper.h
// Author:       dingfang
// CreateDate:   2020-10-13 19:50:44
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-13 21:17:54
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __FILE_HELPER_H__
#define __FILE_HELPER_H__

#include <fstream>
#include <string>

namespace mod
{

    class FileHelper
    {
    public:
        FileHelper(std::string filename)
        {
            fin_.open(filename.c_str(), std::ios_base::in);
        }

        ~FileHelper()
        {
            fin_.close();
        }

        inline std::ifstream &ifstream() { return fin_; }

    private:
        std::ifstream fin_;
    };


}; /* mod namespace end */


#endif /* __FILE_HELPER_H__ */
