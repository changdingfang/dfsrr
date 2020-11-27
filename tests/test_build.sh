#!/bin/bash
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# FileName:     test_build.sh
# Author:       dingfang
# CreateDate:   2020-11-02 20:43:36
# ModifyAuthor: dingfang
# ModifyDate:   2020-11-02 21:39:04
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SCRIPT=`pwd`/${0}
ROOTDIR=`dirname ${SCRIPT}`

bindir=${ROOTDIR}/../build/bin

build()
{
    cd ${ROOTDIR}
    cd ..
    # if [ -d build ]; then
    #     rm -rf build
    # fi
    # mkdir build && cd build
    cd build
    cmake _DTEST_BUILD=on ..
    make
}


checkHttpServer()
{
    sleep 1
    result=`curl -s http://localhost:50000`
    # echo "result: ${result}"
    if [ "${result}" != "hello world" ]; then
        echo -e "\033[0;31;40mtest failed! ===>> [test_httpServer]\033[0m"
        echo -e "\033[0;31;40mplease check ${bindir}/test.log\033[0m"
    fi
    killall -15 test_httpServer
}


check()
{
    if [ ! -d ${bindir} ]; then
        echo "not found bin dir! dir: ${bindir}"
        echo "test failed!"
        exit -1
    fi
    cd ${bindir}
    t=`ls test_*`
    # echo "${t}"

    for f in ${t};
    do
        echo -e "\033[0;32;40;mtest: [${f}]\033[0m"

        if [ "${f}" == "test_httpServer" ]; then
            checkHttpServer &
        fi

        ./${f} >> test.log 2>&1
        ret=$?

        # echo ret: ${ret}
        if [ ${ret} -ne 0 ]; then
            echo -e "\033[0;31;40mtest failed! ===>> [${f}]\033[0m"
            echo -e "\033[0;31;40mplease check ${bindir}/test.log\033[0m"
        fi
    done
}


build
check
