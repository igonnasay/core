//
// Created by AIdancer on 16/10/5.
//

#ifndef CLION_STRATEGY_H
#define CLION_STRATEGY_H

#include "market.h"
#include "trade.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

class StrategyBase
{
public:
  bool run_sig_;
	bool buy_flag;
	bool sell_flag;
	int volume;
	double upper_limit_price;
	double lower_limit_price;
	double open_price;
  string instrument;
  thread *s_thread;
  virtual void StrategyMethod() = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;
	virtual void ShowInfo() = 0;
};

class TradeAssistant
{
public:
  TradeAssistant();
  void add_exp(const string& exp);
  void show_exp();
  void remove_exp_by_id(int _id);

private:
  vector<string> exp_list;
  map<int, char> exp_flag_table;
};

#endif //CLION_STRATEGY_H
