#!/bin/bash
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# FileName:     build.sh
# Author:       dingfang
# CreateDate:   2020-10-28 20:42:57
# ModifyAuthor: dingfang
# ModifyDate:   2020-11-02 20:21:11
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


SCRIPT=`pwd`/${0}
ROOTDIR=`dirname ${SCRIPT}`

build()
{
    cd ${ROOTDIR}
    cd ..
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir build && cd build
    cmake ..
    make
}


package()
{
    cd ${ROOTDIR}
    cd ..
    if [ -d package ]; then
        rm -rf package
    fi
    mkdir -p package/dfsrr
    mkdir package/dfsrr/bin
    mkdir package/dfsrr/conf

    cp build/bin/dfsrr* package/dfsrr/bin/
    cp conf/dfsrr*.json package/dfsrr/conf/
    cp shell/dfsrrctl.sh package/dfsrr/
    cp shell/dfsrrSql.sql package/dfsrr/

    cp -r imgs package/dfsrr/

    cd package
    tar -zcf dfsrr.tar.gz dfsrr/
}


build
package
