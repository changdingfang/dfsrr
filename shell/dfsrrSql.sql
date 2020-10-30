/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * FileName:     dfsrrSql.sql
 * Author:       dingfang
 * CreateDate:   2020-10-29 19:08:25
 * ModifyAuthor: dingfang
 * ModifyDate:   2020-10-29 21:34:18
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

drop database if exists dfsrr_test;
create database dfsrr_test;

use dfsrr_test;

create table `cpu`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    util float default 0,
    idle float default 0,
    user float default 0,
    sys  float default 0,
    hirq float default 0,
    sirq float default 0,
    wait float default 0
) engine=innodb default charset=utf8;


create table `memory`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    used    float default 0,
    free    float default 0,
    buff    float default 0,
    cache   float default 0,
    total   float default 0,
    util    float default 0
) engine=innodb default charset=utf8;


create table `load`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    load1   float default 0,
    load5   float default 0,
    load15  float default 0,
    runq    float default 0,
    plit    float default 0
) engine=innodb default charset=utf8;


create table `partition`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    bfree   float default 0,
    bused   float default 0,
    btotl   float default 0,
    util    float default 0,
    ifree   float default 0,
    itotl   float default 0,
    iutil   float default 0,
    device  varchar(128) default "",
    mount   varchar(128) default ""
) engine=innodb default charset=utf8;


create table `tcp`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    active  float default 0,
    pasive  float default 0,
    iseg    float default 0,
    outseg  float default 0,
    EstRes  float default 0,
    AtmpFa  float default 0,
    CurrEs  float default 0,
    retran  float default 0
) engine=innodb default charset=utf8;


create table `udp`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    idgm    float default 0,
    odgm    float default 0,
    noport  float default 0,
    idmerr  float default 0
) engine=innodb default charset=utf8;


create table `traffic`
(
    idx int unsigned primary key not null auto_increment,
    `timestamp` int unsigned not null,
    `ip` varchar(32) not null,
    bytin   float default 0,
    bytout  float default 0,
    pktin   float default 0,
    pktout  float default 0,
    pkterr  float default 0,
    pktdrp  float default 0,
    device  varchar(128) default ""
) engine=innodb default charset=utf8;
