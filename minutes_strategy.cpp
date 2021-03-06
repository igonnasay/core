//
// Created by AIdancer on 17/2/8.
//

#include "minutes_strategy.h"
#include "easylogging++.h"
#include <thread>

M_Strategy::M_Strategy(CMdSpi *marketSpi, CTraderSpi *traderSpi)
{
    this->marketSpi = marketSpi;
    this->traderSpi = traderSpi;
	this->volume = 0;
	this->buy_flag = false;
	this->sell_flag = false;
}

char M_Strategy::Signal(double ma, double tail_max, double tail_min, double current_price) {
	if((current_price > ma) && (current_price > tail_max)) return 'b';
	if((current_price < ma) && (current_price < tail_min)) return 's';
}
char M_Strategy::Signal(double ma10, double ma20, double ma30, double tail_max, double tail_min, double now) {
	if((now > ma10) && (now > ma20) && (now > ma30) && (now > tail_max))  return 'b';
	if((now < ma10) && (now < ma20) && (now < ma30) && (now < tail_min))  return 's';
}

void M_Strategy::StrategyMethod() {
  TThostFtdcInstrumentIDType contract;
  int N_ = 11;
	int M_ = 30;
  double orderPrice = 0;
  double stopLoss = 0;
  char orderDirection = '-';
  double openPrice = 0;
	double upper_limit_price = 1e9;
	double lower_limit_price = 0;
  double currentPrice = 0;
  double tailMax = 0;
  double tailMin = 0;
	double ma10, ma20, ma30;
	string exchange = this->traderSpi->GetExchange(this->instrument);

	string open_type = "open";
	string close_type = "close";
	if(exchange == "SHFE") {
		close_type = "close_today";
	}

  cout << "[M_Strategy] Trading Thread Start..." << endl;
  printf("instrument = %s\n", this->instrument.c_str());
	printf("exchange = %s\n", exchange.c_str());
  strcpy(contract, this->instrument.c_str());
  Data& data = this->marketSpi->get_data_table(contract);
  while(this->run_sig_)
  {
    if(abs(openPrice) < 1e-7) {
      openPrice = this->marketSpi->GetOpenPrice(this->instrument);
			upper_limit_price = this->marketSpi->GetUpperLimitPrice(this->instrument);
			lower_limit_price = this->marketSpi->GetLowerLimitPrice(this->instrument);
			this->open_price = openPrice;
			this->upper_limit_price = upper_limit_price;
			this->lower_limit_price = lower_limit_price;
      printf("openPrice = %.2f highlimit_price = %.2f  lowlimit_price = %.2f\n", openPrice, upper_limit_price, lower_limit_price);
    }
    if(data.cur <= N_+1 || data.cur <= M_+1) {
      continue;
    } else {
      currentPrice = this->marketSpi->GetLastPrice(this->instrument);
      tailMax = *max_element(data.high+(data.cur-N_-1), data.high+(data.cur-1));
			tailMin = *min_element(data.low+(data.cur-N_-1), data.low+(data.cur-1));
			ma10 = (data.close_sum[data.cur-1] - data.close_sum[data.cur-10-1]) / 10;
			ma20 = (data.close_sum[data.cur-1] - data.close_sum[data.cur-20-1]) / 20;
			ma30 = (data.close_sum[data.cur-1] - data.close_sum[data.cur-30-1]) / 30;

      // Do stoploss first
      if(orderDirection == 'b' && currentPrice < stopLoss){
				this->traderSpi->GiveOrder(this->instrument, lower_limit_price, this->volume, "sell", close_type);
        LOG(INFO) << "[StopLoss position] : Sell Close At " << currentPrice;
        orderDirection = '-';
				this_thread::sleep_for(chrono::milliseconds(100));
      }

      if(orderDirection == 's' && currentPrice > stopLoss) {
				this->traderSpi->GiveOrder(this->instrument, upper_limit_price, this->volume, "buy", close_type);
        LOG(INFO) << "[StopLoss position] : Buy Close At " << currentPrice;
        orderDirection = '-';
				this_thread::sleep_for(chrono::milliseconds(100));
      }
      // Check whether should AI open new position.
      if(orderDirection == '-' && this->buy_flag && Signal(ma10, ma20, ma30, tailMax, tailMin, currentPrice) == 'b') {
				orderDirection = 'b';
				stopLoss = tailMin;
				orderPrice = currentPrice;
				LOG(INFO) << "[Open position] : Buy Open At " << orderPrice;
				this->traderSpi->GiveOrder(this->instrument, upper_limit_price, this->volume, "buy", open_type);
			}
      if(orderDirection == '-' && this->sell_flag && Signal(ma10, ma20, ma30, tailMax, tailMin, currentPrice) == 's') {
      	orderDirection = 's';
        stopLoss = tailMax;
        orderPrice = currentPrice;
        LOG(INFO) << "[Open position] : Sell Open At " << orderPrice;
				this->traderSpi->GiveOrder(this->instrument, lower_limit_price, this->volume, "sell", open_type);
       }
     }
     this_thread::sleep_for(chrono::milliseconds(100));
	}

  //End of while - run_sig_
  if(orderDirection == 'b') {
    //clear rest buy position
		this->traderSpi->GiveOrder(this->instrument, lower_limit_price, this->volume, "sell", close_type);
    orderDirection = '-';
    LOG(INFO) << "[Close rest buy position]";
  }
  else if(orderDirection == 's') {
    // clear rest sell position
		this->traderSpi->GiveOrder(this->instrument, upper_limit_price, this->volume, "buy", close_type);
    orderDirection = '-';
    LOG(INFO) << "[Close rest sell position]";
  }

  cout << "[M_Strategy]Trading Thread now have been Stopped" << endl;
  cout << "[M_Strategy] Good Luck, Bye !" << endl;
}

void M_Strategy::Start() {
  this->run_sig_ = true;
  this->s_thread = new thread([&](){
    this->StrategyMethod();
  });
  this->s_thread->detach();
}

void M_Strategy::Stop() {
  this->run_sig_ = false;
  cout << "[Day Strategy] : thread " << this->s_thread->get_id() << "will be ended..." << endl;
}

void M_Strategy::ShowInfo() {
	printf("instrument : %s    volume : %d\nbuy_flag : %d   sell_flag : %d\nrun_sig : %d\n", this->instrument.c_str(), this->volume, this->buy_flag, this->sell_flag, this->run_sig_);
}


