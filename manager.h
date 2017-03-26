//
// Created by AIdancer on 17/2/11.
//

#ifndef CORE_MANAGER_H
#define CORE_MANAGER_H

#include "strategy.h"
#include "base_data.h"
#include "market.h"
#include "trade.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class StrategyPool {
public:
	StrategyPool(string config_file);
	
private:
	vector<StrategyBase*> strategy;
};

class StrategyControl
{
public:
  StrategyControl(CMdSpi **marketSpi, CTraderSpi **traderSpi);
  void save_minutes_data(std::string instrument, std::string path);
  void CmdStart();

  map<string, Data> data_table;
private:
  void excute_command(const string& command);

  CMdSpi *marketSpi;
  CTraderSpi *traderSpi;
};

#endif //CORE_MANAGER_H
