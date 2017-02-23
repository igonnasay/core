//
// Created by AIdancer on 17/2/8.
//

#include "minutes_strategy.h"
#include "easylogging++.h"

M_Strategy::M_Strategy(CMdSpi *marketSpi, CTraderSpi *traderSpi)
{
    this->marketSpi = marketSpi;
    this->traderSpi = traderSpi;
}

char M_Strategy::Signal(double ma, double tail_max, double tail_min, double current_price) {
	if((current_price > ma) && (current_price > tail_max)) return 'b';
	if((current_price < ma) && (current_price < tail_min)) return 's';
}

void M_Strategy::StrategyMethod() {
    TThostFtdcInstrumentIDType contract;
    int N_ = 11;
	int M_ = 30;
    double orderPrice = 0;
    double stopLoss = 0;
    char orderDirection = '-';
    double openPrice = 0;
    double currentPrice = 0;
    double tailMax = 0;
    double tailMin = 0;
	double ma30;
    this->volume = 1;
    cout << "[M_Strategy] Trading Thread Start..." << endl;
    printf("instrument = %s\n", this->instrument.c_str());
    strcpy(contract, this->instrument.c_str());
    Data& data = this->marketSpi->get_data_table(contract);
    while(this->run_sig_)
    {
        if(abs(openPrice) < 1e-7) {
            openPrice = this->marketSpi->GetOpenPrice(this->instrument);
            printf("openPrice = %.2f\n", openPrice);
        }
        if(data.cur <= N_+1 || data.cur <= M_+1) {
            continue;
        }
        else
        {
            currentPrice = this->marketSpi->GetLastPrice(this->instrument);
            tailMax = *max_element(data.high+(data.cur-N_-1), data.high+(data.cur-1));
			tailMin = *min_element(data.low+(data.cur-N_-1), data.low+(data.cur-1));
			ma30 = (data.close_sum[data.cur-1] - data.close_sum[data.cur-M_-1]) / M_;

            // Do stoploss first
            if(orderDirection == 'b' && (currentPrice < stopLoss || Signal(ma30, tailMax, tailMin, currentPrice) == 's')) {
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Sell, Close, volume);
                LOG(INFO) << "[StopLoss position] : Sell Close At " << currentPrice;
                orderDirection = '-';
            }

            if(orderDirection == 's' && (currentPrice > stopLoss || Signal(ma30, tailMax, tailMin, currentPrice) == 'b')) {
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Buy, Close, volume);
                LOG(INFO) << "[StopLoss position] : Buy Close At " << currentPrice;
                orderDirection = '-';
            }

            // Check whether should AI open new position.
            if(orderDirection == '-' && currentPrice > ma30 && currentPrice > tailMax) {
                orderDirection = 'b';
                stopLoss = tailMin;
                orderPrice = currentPrice;
                LOG(INFO) << "[Open position] : Buy Open At " << orderPrice;
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Buy, Open, volume);
            }
            if(orderDirection == '-' && currentPrice < ma30 && currentPrice < tailMin) {
                orderDirection = 's';
                stopLoss = tailMax;
                orderPrice = currentPrice;
                LOG(INFO) << "[Open position] : Sell Open At " << orderPrice;
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Sell, Open, volume);
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    //End of while - run_sig_
    if(orderDirection == 'b') {
        //clear rest buy position
        this->traderSpi->ReqMarketPriceOrderInsert(contract, Sell, Close, volume);
        orderDirection = '-';
        printf("[Close rest buy position ]\n");
    }
    else if(orderDirection == 's') {
        // clear rest sell position
        this->traderSpi->ReqMarketPriceOrderInsert(contract, Buy, Close, volume);
        orderDirection = '-';
        printf("[Close rest sell position]\n");
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














