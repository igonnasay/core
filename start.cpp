//
// Created by AIdancer on 16/10/5.
//

#include "easylogging++.h"
#include "tutils.h"
#include "market.h"
#include "trade.h"
#include "strategy.h"
#include "manager.h"
#include <cstring>
#include <string>
#include <mutex>
#include <thread>
#include <iostream>

INITIALIZE_EASYLOGGINGPP

using namespace std;

//config information
char passwd[105];

//API for market
CThostFtdcMdApi* pMarketUserApi = NULL;
CThostFtdcMdSpi* pMarketUserSpi = NULL;
CMdSpi *pCMdSpi = NULL;

//API for trade
CTraderSpi* pTraderUserSpi = NULL;
CThostFtdcTraderApi *pTraderUserApi = NULL;

//API for strategy
StrategyControl *pStrategyApi = NULL;

void MarketInit()
{
    vector<string> list;
    list.clear();
    FILE *file;
    char str[105], val[105];
    file = fopen("./config/market.cf", "r");
    while(fscanf(file, "%s", str) != EOF)
    {
        string contract(str);
        list.push_back(contract);
    }
    fclose(file);

    char brokerid[105];
    char investorid[105];
    char password[105];
    file = fopen("./config/info.cf", "r");
    while(fscanf(file, "%s", str) != EOF)
    {
        if(strcmp(str, "m_brokerid") == 0)   fscanf(file, "%s", brokerid);
        if(strcmp(str, "m_investorid") == 0)   fscanf(file, "%s", investorid);
        if(strcmp(str, "m_password") == 0)   fscanf(file, "%s", password);
    }
    fclose(file);


    char front_address[] = "tcp://180.168.212.76:41213";
    pMarketUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// Create UserApi
    pCMdSpi = new CMdSpi(pMarketUserApi);
    pMarketUserSpi= (CThostFtdcMdSpi *)pCMdSpi;
    pCMdSpi->SetBrokerId(brokerid);
    pCMdSpi->SetInvestorID(investorid);
    pCMdSpi->SetPassword(password);
    pCMdSpi->SetInstrumentList(list);
    pCMdSpi->DebugInfo();
    pMarketUserApi->RegisterSpi(pMarketUserSpi);						// Register MarketUserSpi
    pMarketUserApi->RegisterFront(front_address);					// Connect to broker's market server
    pMarketUserApi->Init();
    pMarketUserApi->Join();
}

void TraderInit()
{
    FILE *file;
    char str[105];
    char brokerid[105];
    char investorid[105];
    char password[105];
    file = fopen("./config/info.cf", "r");
    while(fscanf(file, "%s", str) != EOF)
    {
        if(strcmp(str, "t_brokerid") == 0)   fscanf(file, "%s", brokerid);
        if(strcmp(str, "t_investorid") == 0)   fscanf(file, "%s", investorid);
        if(strcmp(str, "t_password") == 0)   fscanf(file, "%s", password);
    }
    fclose(file);
    char Trade_FRONT_ADDR[105] = "tcp://180.168.212.76:41205";
    pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();            // Create UserApi
    pTraderUserSpi = new CTraderSpi(pTraderUserApi);
    pTraderUserSpi->SetTradeAddress(Trade_FRONT_ADDR);
    pTraderUserSpi->SetBrokerId(brokerid);
    pTraderUserSpi->SetInvestorID(investorid);
    pTraderUserSpi->SetPassword(password);
    pTraderUserSpi->SetPassword(passwd);
    pTraderUserSpi->DebugInfo();
    pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);            // Register TraderUserSpi
    pTraderUserApi->SubscribePublicTopic(THOST_TERT_QUICK);           		// Register Public Stream  
    pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);           		// Register Private Stream
    pTraderUserApi->RegisterFront(Trade_FRONT_ADDR);                            // connect
    pTraderUserApi->Init();

    pTraderUserApi->Join();
}

void StrategyInit()
{
    pStrategyApi = new StrategyControl(&pCMdSpi, &pTraderUserSpi);
    pStrategyApi->CmdStart();
}

void Test() {
    CThostFtdcQryContractBankField *pQryContractBank;
}

extern double test_ta();

int main()
{
    el::Configurations conf("./easylogging/my-conf.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    cout << "Press your trade password : ";
    cin >> passwd;
    char tailchar[5];
    cin.getline(tailchar, 4);
    thread market_thread(MarketInit);
    thread trader_thread(TraderInit);
    thread strategy_thread(StrategyInit);
    market_thread.join();
    trader_thread.join();
    strategy_thread.join();
    return  0;
}










