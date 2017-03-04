//
// Created by AIdancer on 17/2/11.
//

#include "tutils.h"
#include "minutes_strategy.h"
#include "easylogging++.h"
#include "manager.h"
#include <cassert>
#include <ctime>
#include <string>
#include <vector>

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

	thread t([&]() {
		while(true) {
			time_t now = time(0);
			tm *ltm = localtime(&now);
			if((ltm->tm_hour == 15 && ltm->tm_min == 17) || (ltm->tm_hour == 2 && ltm->tm_min == 32)) {
				LOG(INFO) << "Trade time end, core will save the data && exit later";
				this->marketSpi->save_all();
				LOG(INFO) << "Saveall data successfully, now core will exit, Bye!";
				exit(0);
			}
			this_thread::sleep_for(chrono::seconds(1));
		}
	});
	t.detach();

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
			if(base->run_sig_)
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
		else if(cmd_vec.size()==3 && cmd_vec[0]=="set" && cmd_vec[1]=="volume") {
			base->volume = std::stoi(cmd_vec[2].c_str());
		}
		else if(cmd_vec.size()==1 && cmd_vec[0]=="loadall") {
			// loadall
			this->marketSpi->load_all();
		}
		else if(cmd_vec.size()==2 && cmd_vec[0]=="showbar") {
			// showbar m1705
			this->marketSpi->show_bar_data(cmd_vec[1]);
		}
		else if(cmd_vec.size()==1 && cmd_vec[0]=="showinfo") {
			base->ShowInfo();
		}
    }
}

void StrategyControl::excute_command(const string& command)
{
    //
}














