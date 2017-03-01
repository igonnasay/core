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

private:
	char Signal(double ma, double tail_max, double tail_min, double current_price);
    CMdSpi *marketSpi;
    CTraderSpi *traderSpi;
};

#endif //CLION_MINUTES_STRATEGY_H
