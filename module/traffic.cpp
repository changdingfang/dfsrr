// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// FileName:     traffic.cpp
// Author:       dingfang
// CreateDate:   2020-10-16 19:18:04
// ModifyAuthor: dingfang
// ModifyDate:   2020-10-16 21:20:14
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include "dflog/dflog.h"
#include "traffic.h"
#include "fileHelper.h"

#include <sstream>

using namespace std;

namespace mod
{


        std::map<std::string, double> Traffic::collect()
        {
            if (this->readNetDev() != 0)
            {
                return std::move(trafficMetric_);
            }
            this->calculate();

            lastTrafficMap_ = currTrafficMap_;

            return std::move(trafficMetric_);
        }


        int Traffic::readNetDev()
        {
            FileHelper f(NetDev);
            if (f.ifstream().fail())
            {
                LOG(WARN, "open snmp file failed!");
                return -1;
            }

            string line;
            while (getline(f.ifstream(), line))
            {
                string::size_type index = line.find_first_of(":");
                if (index == string::npos)
                {
                    continue;
                }

                string::size_type posStart = line.find_first_not_of(" ");
                string devName = line.substr(posStart, index - posStart);

                if (!this->checkNetDev(devName))
                {
                    continue;
                }

                stringstream ss(line.substr(index + 1));
                UINT64 discard;
                TrafficStat_T netStat = { 0 };
                ss >> netStat.bytein >> netStat.pktin >> netStat.pkterrin >> netStat.pktdrpin
                    >> discard >> discard >> discard >> discard 
                    >> netStat.byteout >> netStat.pktout >> netStat.pkterrout >> netStat.pktdrpout
                    >> discard >> discard >> discard >> discard;
                netStat.timestamp = ::time(nullptr);

                if (netStat.pktin > 0)
                {
                    currTrafficMap_[devName] = netStat;
                }
            }

            return 0;
        }


        int Traffic::calculate()
        {
            TrafficStat_T totalSt = { 0 };
            UINT64 ts = 0;
            for (const auto &it : currTrafficMap_)
            {
                string devName = it.first;
                auto lastIt = lastTrafficMap_.find(devName);
                if (lastIt == lastTrafficMap_.end())
                {
                    continue;
                }
                const TrafficStat_T &currStat = it.second;
                const TrafficStat_T &lastStat = lastIt->second;
                ts = Delta(currStat.timestamp, lastStat.timestamp);
                UINT64 bytein       = Delta(currStat.bytein, lastStat.bytein);
                UINT64 byteout      = Delta(currStat.byteout, lastStat.byteout);
                UINT64 pktin        = Delta(currStat.pktin, lastStat.pktin);
                UINT64 pktout       = Delta(currStat.pktout, lastStat.pktout);
                UINT64 pkterrin     = Delta(currStat.pkterrin, lastStat.pkterrin);
                UINT64 pktdrpin     = Delta(currStat.pktdrpin, lastStat.pktdrpin);
                UINT64 pkterrout    = Delta(currStat.pkterrout, lastStat.pkterrout);
                UINT64 pktdrpout    = Delta(currStat.pktdrpout, lastStat.pktdrpout);

                totalSt.bytein      += bytein;
                totalSt.byteout     += byteout;
                totalSt.pktin       += pktin;
                totalSt.pktout      += pktout;
                totalSt.pkterrin    += pkterrin;
                totalSt.pktdrpin    += pktdrpin;
                totalSt.pkterrout   += pkterrout;
                totalSt.pktdrpout   += pktdrpout;
            }

            trafficMetric_["bytin"]     = Ratio(totalSt.bytein, ts);
            trafficMetric_["bytout"]    = Ratio(totalSt.byteout, ts);
            auto pktinIt = trafficMetric_.insert(make_pair("pktin", Ratio(totalSt.pktin, ts)));
            auto pktoutIt = trafficMetric_.insert(make_pair("pktout", Ratio(totalSt.pktout, ts)));

            double pktSum =  pktinIt.first->second + pktoutIt.first->second;
            trafficMetric_["pkterr"]    = Ratio(totalSt.pkterrin + totalSt.pkterrout, pktSum);
            trafficMetric_["pktdrp"]    = Ratio(totalSt.pktdrpin + totalSt.pktdrpout, pktSum);

            return 0;
        }


        bool Traffic::checkNetDev(const string &devName)
        {
            const string NetDevice[] = {"eth"};
            for (int i = 0; i < 1; ++i)
            {
                if (devName.size() >= NetDevice[i].size()
                        && devName.compare(0, NetDevice[i].size(), NetDevice[i]) == 0)
                {
                    return true;
                }
            }

            return  false;
        }


}; /* mod namespace end */
