#!/bin/bash
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# FileName:     dfsrrctl.sh
# Author:       dingfang
# CreateDate:   2020-10-28 21:02:47
# ModifyAuthor: dingfang
# ModifyDate:   2020-10-28 21:12:09
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SCRIPT=`pwd`/${0}
ROOTDIR=`dirname ${SCRIPT}`


start()
{
    cd bin
    nohup ./dfsrr ../conf/dfsrr.json 2>&1 &
}


stop()
{
    pid=`ps -elf | grep "./dfsrr ../conf/dfsrr.json" | grep -v "grep" | awk -F ' ' '{print $4}'`
    echo ${pid}
    kill -9 ${pid}
}


restart()
{
    stop
    sleep 3
    start
}


case ${1} in
    "start" )
        start
        ;;
    "stop" )
        stop
        ;;
    "restart" )
        restart
        ;;
    * )
        echo "========================================"
        echo "please input param start,stop or restart"
        echo "========================================"
        ;;
esac
