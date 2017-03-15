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

StrategyPool::StrategyPool(string config_file) {
	// init strategy pool with config file.
}

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
    vector<string> cmd;

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
        MarketUtil::split(command, " ", cmd);
        if(command == "start") {
			// note : 必须开盘后才可以调用此命令，否则无法获得合约开盘／涨停／跌停价格，程序会发生崩溃，自动退出。
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
        else if(cmd.size()==2 && cmd[0]=="showminutes")
        {
            // command sample : shominutes m1705
            this->marketSpi->ShowMinutesData(cmd[1]);
        }
        else if(cmd.size()==3 && command == "saveminutes")
        {
            // command sample : saveminutes m1705 ./m1705.mdt
            this->save_minutes_data(cmd[1], cmd[2]);
        }
        else if(command == "saveall")
        {
            this->marketSpi->save_all();
        }
        else if(cmd.size()==3 && cmd[0]=="set" && cmd[1]=="instrument")
        {
            // command sample : set instrument m1705
            if(base->run_sig_)
            {
                base->Stop();
            }
            base->instrument = cmd[2];
        }
		else if(cmd.size()==3 && cmd[0]=="set" && cmd[1]=="volume") {
			base->volume = std::stoi(cmd[2].c_str());
		}
		else if(cmd.size()==1 && cmd[0]=="loadall") {
			// loadall
			this->marketSpi->load_all();
		}
		else if(cmd.size()==2 && cmd[0]=="showbar") {
			// showbar m1705
			this->marketSpi->show_bar_data(cmd[1]);
		}
		else if(cmd.size()==1 && cmd[0]=="showinfo") {
			// showinfo
			base->ShowInfo();
		}
		else if(cmd.size()==3 && cmd[0]=="set" && cmd[1]=="buy_flag") {
			// set buy_flag true
			if(cmd[2] == "true")  base->buy_flag = true;
			else if(cmd[2] == "false")  base->buy_flag = false;
		}
		else if(cmd.size()==3 && cmd[0]=="set" && cmd[1]=="sell_flag") {
			// set sell_flag true
			if(cmd[2] == "true")  base->sell_flag = true;
			else if(cmd[2] == "false")  base->sell_flag = false;
		}
		else if(cmd.size() == 2 && cmd[0] == "buy_open") {
			// buy_open 3000
			double price = std::stod(cmd[1].c_str());
			this->traderSpi->GiveOrder(base->instrument, price, base->volume, "buy", "open");
		}
		else if(cmd.size() == 2 && cmd[0] == "sell_open") {
			// sell_open 2900
			double price = std::stod(cmd[1].c_str());
			this->traderSpi->GiveOrder(base->instrument, price, base->volume, "sell", "open");
		}
		else if(cmd.size() == 2 && cmd[0] == "buy_close_today") {
			// buy_close_today 3010
			double price = std::stod(cmd[1].c_str());
			string exchange = this->traderSpi->GetExchange(base->instrument);
			if(exchange == "SHFE") {
				this->traderSpi->GiveOrder(base->instrument, price, base->volume, "buy", "close_today");
			}
			else {
				this->traderSpi->GiveOrder(base->instrument, price, base->volume, "buy", "close");
			}
		}
		else if(cmd.size() == 2 && cmd[0] == "sell_close_today") {
			// sell_close_today 2875
			double price = std::stod(cmd[1].c_str());
			string exchange = this->traderSpi->GetExchange(base->instrument);
			if(exchange == "SHFE") {
				this->traderSpi->GiveOrder(base->instrument, price, base->volume, "sell", "close_today");
			}
			else {
				this->traderSpi->GiveOrder(base->instrument, price, base->volume, "sell", "close");
			}
		}
    }
}

void StrategyControl::excute_command(const string& command)
{
    //
}














