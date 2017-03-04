//
// Created by AIdancer on 17/2/8.
//

#ifndef CLION_MINUTES_STRATEGY_H
#define CLION_MINUTES_STRATEGY_H

#include "strategy.h"

class M_Strategy : public StrategyBase
{
public:
    M_Strategy(CMdSpi *marketSpi, CTraderSpi *traderSpi);
    void Start();
    void StrategyMethod();
    void Stop();
	void ShowInfo();

	// Sample : MarketOrder("m1705", Buy, Open, 1);
	// Sample : MarketOrder("m1705", Sell, Close, 1);
	void MarketOrder(const string& instrument, char order_dir, char order_type, int volume);

private:
	char Signal(double ma, double tail_max, double tail_min, double current_price);
	char Signal(double ma10, double ma20, double ma30, double tail_max, double tail_min, double now);
    CMdSpi *marketSpi;
    CTraderSpi *traderSpi;
};

#endif //CLION_MINUTES_STRATEGY_H
