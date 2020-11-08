# 简介
dfsrr是一个资源监控工具, 用于收集系统信息, 如cpu、内存、负载、网络等。  
收集的数据可通过命令行实时打印, 也可以存储在本地文件或上报到服务端。  


![dfsrr](/imgs/dfsrr.png)

# 编译安装

## 编译

**依赖**  

- mysql-devel
- gcc
- cmake

```shell
cd shell
# 编译(包含第三方依赖) 三方依赖编译时间比较久, 第一次编译后, 再编译可以不用添加 --depend
./build.sh --depend
```

## 安装

编译后在项目根目录下有个 `package` 文件夹,  里面的 `dfsrr.tar.gz` 是编译后打包压缩文件. 解压到指定路径既安装路径(推荐解压到 `/home/dfsrr/` 下)  

## 说明

package包结构
```shell
dfsrr
├── bin
│   ├── dfsrr
│   ├── dfsrr-server
│   ├── dfsrrTool
│   └── dfsrr-WebServer
├── conf
│   ├── dfsrr-http-server.json
│   ├── dfsrr.json
│   └── dfsrr-server.json
├── dfsrrctl.sh
├── dfsrrSql.sql
└── imgs
    └── favicon.jpg
```

- dfsrr
    采集服务
- dfsrr-server
    采集服务服务端
- dfsrrTool
    命令行工具(可以转移到 `/usr/bin/` 下作为命令行)
- dfsrr-WebServer
    web服务端, 支撑http服务
- dfsrrctl.sh
    开启/停止/重启 dfsrr服务
- dfsrrSql.sql
    创建dfsrr-server存储需要的mysql数据库和表

配置文件名与程序文件一一对应


dfsrr可以通过修改配置文件支持采集数据保存到本地sqlite, 也支持通过tcp发送到远端服务器

# 命令行

## 用法

```shell
$ dfsrrTool -h
Usage: dfsrrTool [Module] [options]
Options:
        -w / --watch    打印最近的n条数据:      dfsrrTool --cpu -w 10
        -l / --live     实时打印:               dfsrrTool --cpu -l
        -i / --interval 指定时间间隔(s):        dfsrrTool --cpu -l -i 5
        -t / --time     打印指定时间数据:       dfsrrTool --cpu -t 20201001
                                                dfsrrTool --cpu -t 2020100108
                                                dfsrrTool --cpu -t 202010010800
        -f / --file     从文件中读取数据:       dfsrrTool --cpu -f filename
        -s / --spec     指定显示指标字段:       dfsrrTool --cpu -s util,sys,user
        -h / --help     帮助
Modules:
        --cpu           CPU
        --memory        内存
        --load          系统运行队列和平均负载
        --partition     磁盘分区
        --tcp           tcp流量
        --udp           udp流量
        --traffic       流量
```

## 示例

```shell
$ dfsrrTool --cpu -l -i 1
--------------------------------cpu--------------------------------
Time              user     sys    wait    hirq    sirq    util    idle
07/11 10:07:27    2.03    0.51    0.00    0.00    0.00    2.54   97.46
07/11 10:07:28    1.50    1.00    0.00    0.00    0.00    2.50   97.50
07/11 10:07:29    0.51    0.51   15.23    0.00    0.00    1.02   83.76

$ dfsrrTool  --load -w 2
------------------------load------------------------
Time             load1   load5  load15    runq    plit
07/11 10:07:30    0.24    0.17    0.22    1.00  373.00
07/11 10:08:00    0.22    0.17    0.22    1.00  376.00
```
