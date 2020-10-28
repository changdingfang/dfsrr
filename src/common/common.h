// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     common.h
// Author:       dingfang
// CreateDate:   2020-10-21 08:42:00
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-28 17:46:19
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/stat.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <string>
#include <vector>


namespace common
{


    static bool mkdir_(const std::string &path)
    {
        return (::mkdir(path.c_str(), 0755) == 0);
    }


    static bool pathExists(const std::string &path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }


    static bool createDir(std::string path)
    {
        if (path.empty())
        {
            return false;
        }

        if (pathExists(path))
        {
            return true;
        }

        size_t searchOffset = 0;
        do
        {
            std::string::size_type pos = path.find('/', searchOffset);
            if (pos == std::string::npos)
            {
                pos = path.size();
            }
            std::string subdir = path.substr(0, pos);
            if (!subdir.empty() && !pathExists(subdir) && !mkdir_(subdir))
            {
                return false;
            }
            searchOffset = pos + 1;
        } while (searchOffset < path.size());

        return true;
    }


    static void gethostname(std::string &hostname)
    {
        char host[256];
        ::gethostname(host, 256);
        hostname = host;
    }


    static void getip(std::vector<std::string> &ipVec)
    {
        struct ifaddrs *ifas = NULL;

        for (getifaddrs(&ifas); ifas != NULL; ifas = ifas->ifa_next)
        {
            std::string deviceName, addrStr;
            if (ifas->ifa_addr->sa_family == AF_INET)
            {
                char addr[INET_ADDRSTRLEN] = { 0 };
                ::inet_ntop(AF_INET, &((struct sockaddr_in *)ifas->ifa_addr)->sin_addr, addr, INET_ADDRSTRLEN);
                deviceName  = ifas->ifa_name;
                addrStr     = addr;
            }
            else if (ifas->ifa_addr->sa_family == AF_INET6)
            {
                continue;
                char addr[INET6_ADDRSTRLEN] = { 0 };
                ::inet_ntop(AF_INET, &((struct sockaddr_in *)ifas->ifa_addr)->sin_addr, addr, INET6_ADDRSTRLEN);
                deviceName  = ifas->ifa_name;
                addrStr     = addr;
            }

            if (!deviceName.empty() && deviceName != "lo" && addrStr != "127.0.0.1")
            {
                ipVec.push_back(addrStr);
                deviceName.clear();
            }
        }
    }


}; /* common namespace end */

#endif /* __COMMON_H__ */
