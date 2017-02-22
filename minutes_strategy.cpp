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

void M_Strategy::StrategyMethod() {
    TThostFtdcInstrumentIDType contract;
    int N_ = 30;
    double orderPrice = 0;
    double stopLoss = 0;
    char orderDirection = '-';
    double openPrice = 0;
    double currentPrice = 0;
    double tailMax = 0;
    double tailMin = 0;
    int volume = 1;
    //double stop_profit_limit_value = 20;
    //double stop_loss_limit_value = 5;
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
        if(data.cur <= N_+1) {
            continue;
        }
        else
        {
            currentPrice = this->marketSpi->GetLastPrice(this->instrument);
            tailMax = *max_element(data.high+(data.cur-N_-1), data.high+(data.cur-1));
			tailMin = *min_element(data.low+(data.cur-N_-1), data.low+(data.cur-1));

            // Do stoploss first
            if(orderDirection == 'b' && currentPrice < stopLoss) {
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Sell, Close, volume);
                printf("[StopLoss] : Sell Close At %.2f\n", currentPrice);
                orderDirection = '-';
            }

            if(orderDirection == 's' && currentPrice > stopLoss) {
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Buy, Close, volume);
                printf("[StopLoss] : Buy Close At %.2f\n", currentPrice);
                orderDirection = '-';
            }

            // Check whether should AI open new position.
            if(orderDirection == '-' && currentPrice > tailMax) {
                orderDirection = 'b';
                stopLoss = tailMin;
                orderPrice = currentPrice;
                printf("[Open Position] : Buy Open At %.2f\n", orderPrice);
                this->traderSpi->ReqMarketPriceOrderInsert(contract, Buy, Open, volume);
            }
            if(orderDirection == '-' && currentPrice < tailMin) {
                orderDirection = 's';
                stopLoss = tailMax;
                orderPrice = currentPrice;
                printf("[Open Position] : Sell Open At %.2f\n", orderPrice);
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














