//
// Created by AIdancer on 16/10/16.
//

#ifndef CLION_STRATEGYDATA_H
#define CLION_STRATEGYDATA_H

#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcUserApiDataType.h"
#include <string>

using namespace std;

class TradeInfo {
public:
	const static char Buy = THOST_FTDC_D_Buy;
	const static char Sell = THOST_FTDC_D_Sell;
	const static char Open = THOST_FTDC_OFEN_Open;
	const static char Close = THOST_FTDC_OFEN_Close;
	const static char Started = 'Y';
	const static char Waiting = 'N';
	const static char Finished = 'F';

	string GetInstrument();
	TThostFtdcDirectionType GetDirection();
	int GetVolume();
	double GetOpenPrice();
	double GetStopLoss();
	char GetTradeState();
	void SetTradeState(char state);

private:
	TThostFtdcInstrumentIDType instrument;
	TThostFtdcDirectionType direction;
	int volume;
	double openPrice;
	double stopLoss;
	char tradeState;
};

#endif //CLION_STRATEGYDATA_H





