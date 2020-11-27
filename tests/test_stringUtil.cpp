// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     test_stringUtil.cpp
// Author:       dingfang
// CreateDate:   2020-11-01 11:16:49
// ModifyAuthor: dingfang
// ModifyDate:   2020-11-01 11:49:28
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "common/stringUtil.h"

#include "assert.h"

using namespace std;
using namespace common;

void testSplit()
{
    vector<string> expect { "util", "idle", "free", "hellowrold", " failed", "", "end" };
    string s("util,idle,free,hellowrold, failed,,end");
    vector<string> res = split(s, ",");

    assert(expect.size() == res.size());
    for (unsigned int i = 0; i < res.size(); ++i)
    {
        LOG(INFO, "expect: [{}], result: [{}]", expect[i], res[i]);
        assert(expect[i] == res[i]);
    }
}


void testStrip()
{
    vector<string> strv 
    { 
        " helloworld"
            , "df "
            , "   df"
            , "df     "
            , " df df  "
            , " df df   df "
            , "\tdf"
            , "df\t "
    };

    vector<string> expect
    { 
        "helloworld"
            , "df"
            , "df"
            , "df"
            , "df df"
            , "df df   df"
            , "df"
            , "df"
    };

    assert(expect.size() == strv.size());
    for (unsigned int i = 0; i < expect.size(); ++i)
    {
        string res = strip(strv[i]);
        LOG(INFO, "expect: [{}], result: [{}]", expect[i], res);
        assert(expect[i] == res);
    }
}


int main(void)
{
    dflog::InitLog("./test_stringUtil.log", dflog::loggerOption::CONSOLE);
    dflog::SetLevel(DEBUG);
    testSplit();
    testStrip();

    return 0;
}
