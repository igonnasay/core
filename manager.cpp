//
// Created by AIdancer on 17/2/11.
//

#include "tutils.h"
#include "minutes_strategy.h"
#include "easylogging++.h"
#include "manager.h"
#include <cassert>
#include <string>
#include <vector>
#include <boost/regex.hpp>

using namespace std;

StrategyControl::StrategyControl(CMdSpi **marketSpi, CTraderSpi **traderSpi)
{
    while((*marketSpi)==NULL || (!(*marketSpi)->IsMarketInited()))
    {
        cout << "[StrategyControl] : Waiting for marketAPI to init..." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    while((*traderSpi)==NULL || (!(*traderSpi)->IsTradeInited()))
    {
        cout << "[StrategyControl: Waiting for traderAPI to init..." << endl;
        this_thread::sleep_for(chrono::seconds(1));
    }
    assert(*marketSpi != NULL);
    assert(*traderSpi != NULL);
    assert((*marketSpi)->IsMarketInited());
    assert((*traderSpi)->IsTradeInited());
    this->marketSpi = *marketSpi;
    this->traderSpi = *traderSpi;
}

void StrategyControl::save_minutes_data(std::string instrument, std::string path)
{
    std::vector<MinuteData>& list = this->marketSpi->GetMinutesList(instrument);
    MarketUtil::save_minutes_list(list, path);
}

void StrategyControl::CmdStart() {
    cout << "[Control] : cmd start" << endl;
    if(this->marketSpi == NULL || this->traderSpi == NULL)
    {
        LOG(FATAL) << "marketSpi == NULL or traderSpi == NULL";
    }
    string command;
    StrategyBase *base = new M_Strategy(this->marketSpi, this->traderSpi);
    base->instrument = "m1705";
    vector<string> cmd_vec;
    while(true)
    {
        printf("[Control] : option >> ");
        getline(cin, command);
        command = MarketUtil::strip(command);
        MarketUtil::split(command, " ", cmd_vec);
        if(command == "start") {
            base->Start();
        }
        else if(command == "stop")
        {
            base->Stop();
        }
        else if(command == "exit") {
            exit(0);
        }
        else if(cmd_vec.size()==2 && cmd_vec[0]=="showminutes")
        {
            // command sample : shominutes m1705
            this->marketSpi->ShowMinutesData(cmd_vec[1]);
        }
        else if(cmd_vec.size()==3 && command == "saveminutes")
        {
            // command sample : saveminutes m1705 ./m1705.mdt
            this->save_minutes_data(cmd_vec[1], cmd_vec[2]);
        }
        else if(command == "saveall")
        {
            this->marketSpi->save_all();
        }
        else if(cmd_vec.size()==3 && cmd_vec[0]=="set" && cmd_vec[1]=="instrument")
        {
            // command sample : set instrument m1705
            if(base->run_sig_)
            {
                base->Stop();
            }
            base->instrument = cmd_vec[2];
        }
		else if(cmd_vec.size()==1 && cmd_vec[0]=="loadall") {
			// loadall
			this->marketSpi->load_all();
		}
		else if(cmd_vec.size()==2 && cmd_vec[0]=="showbar") {
			// showbar m1705
			this->marketSpi->show_bar_data(cmd_vec[1]);
		}
		else if(cmd_vec.size()==2 && cmd_vec[0]=="showbark") {
			this->marketSpi->show_bark_data(cmd_vec[1]);
		}
    }
}

void StrategyControl::excute_command(const string& command)
{
    //
}














