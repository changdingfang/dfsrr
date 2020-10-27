#!/bin/bash
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# FileName:     build.sh
# Author:       dingfang
# CreateDate:   2020-10-27 22:02:13
# ModifyAuthor: dingfang
# ModifyDate:   2020-10-27 23:36:55
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

SCRIPT=`pwd`/${0}
ROOTDIR=`dirname ${SCRIPT}`

DST=${ROOTDIR}/../compiled/
INCLUDE=${DST}/include
LIB=${DST}/lib

cd ${ROOTDIR}

check()
{
    if [ ! -d ${INCLUDE} ]; then
        mkdir -p ${INCLUDE}
    fi

    if [ ! -d ${LIB} ]; then
        mkdir -p ${LIB}
    fi
}


dflog()
{
    cd ${ROOTDIR}
    tar -zxpf dflog.tar.gz
    cd dflog
    mkdir -p build && cd build
    cmake ..
    make
    cp lib/*.a ${LIB}
    cd ..

    if [ -d ${INCLUDE}/dflog ]; then
        rm -rf ${INCLUDE}/dflog/*
    else
        mkdir -p ${INCLUDE}/dflog
    fi

    if [ -d ${INCLUDE}/fmt ]; then
        rm -rf ${INCLUDE}/fmt/*
    else
        mkdir -p ${INCLUDE}/fmt
    fi

    cp -a src/dflog/*.h ${INCLUDE}/dflog
    cp -a src/fmt/*.h  ${INCLUDE}/fmt
}


libevent()
{
    cd ${ROOTDIR}
    tar -zxpf libevent-2.1.11-stable.tar.gz
    cd libevent-2.1.11-stable
    ./configure --enable-static=yes
    make -j4

    if [ -d ${LIB}/libevent ]; then
        rm -rf ${LIB}/libevent/*
    else
        mkdir -p ${LIB}/libevent
    fi

    if [ -d ${INCLUDE}/libevent ]; then
        rm -rf ${INCLUDE}/libevent/*
    else
        mkdir -p ${INCLUDE}/libevent
    fi

    cp -a .libs/libevent.a .libs/libevent_pthreads.a ${LIB}/libevent
    cp -a include/* ${INCLUDE}/libevent
}


nlohmann()
{
    cd ${ROOTDIR}
    tar -zxpf nlohmann.tar.gz
    if [ -d ${INCLUDE}/nlohmann ]; then
        rm -rf ${INCLUDE}/nlohmann
    fi

    cp -a nlohmann ${INCLUDE}/
}


sqlite3()
{
    cd ${ROOTDIR}
    tar -zxpf sqlite-autoconf-3330000.tar.gz
    cd sqlite-autoconf-3330000
    ./configure --enable-static=yes
    make -j4 

    if [ -d ${INCLUDE}/sqlite3 ]; then
        rm -rf ${INCLUDE}/sqlite3/*
    else
        mkdir -p ${INCLUDE}/sqlite3
    fi

    cp -a .libs/*.a ${LIB}/
    cp -a *.h ${INCLUDE}/sqlite3/
}


check

dflog
libevent
nlohmann
sqlite3

