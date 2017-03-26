//
// Created by AIdancer on 16/10/16.
//

#include "strategy_data.h"
#include <iostream>

string TradeInfo::GetInstrument() {
  string ret(this->instrument);
  return  ret;
}
char TradeInfo::GetDirection() {
  return  this->direction;
}
int TradeInfo::GetVolume() {
  return  this->volume;
}
double TradeInfo::GetOpenPrice() {
  return  this->openPrice;
}
double TradeInfo::GetStopLoss() {
  return  this->stopLoss;
}
char TradeInfo::GetTradeState() {
  return  this->tradeState;
}
void TradeInfo::SetTradeState(char state) {
  if(state != TradeInfo::Started && state != TradeInfo::Waiting && state != TradeInfo::Finished) {
    cout << "Error Occured" << endl;
  }
  else this->tradeState = state;
}











