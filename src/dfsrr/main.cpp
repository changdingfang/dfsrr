// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     main.cpp
// Author:       dingfang
// CreateDate:   2020-10-20 19:57:36
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-20 21:20:23
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "dfsrr/dfsrr.h"

#include <string>


using namespace dfsrr;
using namespace std;


int init(int argc, char **argv)
{
    // FILELOG | CONSOLE
    dflog::InitLog("./dfssrTool.log", dflog::loggerOption::CONSOLE);
    // dflog::SetLevel(INFO);
    dflog::SetLevel(DEBUG);

    LOG(DEBUG, "dfssr is running...");


    Dfsrr dfsrr("/home/dingfang/git/dfsrr/conf/dfsrr.json");
    dfsrr.run();

    return 0;
}


int main(int argc, char **argv)
{
    init(argc, argv);

    return 0;
}
