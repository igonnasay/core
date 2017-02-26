#include "tutils.h"
#include "trade.h"
#include "easylogging++.h"
#include <iostream>
#include <cstdio>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <string>
#include <mutex>
#include "ThostFtdcTraderApi.h"

using namespace std;



void CTraderSpi::AutoIncOrderRef()
{
	int val = 0;
	sscanf(ORDER_REF, "%d", &val);
	val++;
	sprintf(ORDER_REF, "%d", val);
}

// 流控判断
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

CTraderSpi::CTraderSpi(CThostFtdcTraderApi *pTraderAPI)
{
	pTraderUserApi = pTraderAPI;
	this->traderIRequestID = 0;
	this->iNextOrderRef = 0;
    this->tradeInited = false;
}

void CTraderSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CTraderSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pTraderUserApi->ReqUserLogin(&req, ++traderIRequestID);
	cerr << "--->>> 发送用户登录请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
}

void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
								CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		// 保存会话参数
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
		iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pTraderUserApi->GetTradingDay() << endl;
		cerr << "Onrsp trade succeed..." << endl;
		this->tradeInited = true;
		cerr << "tradeAPI inited succeed..." << endl;
		///投资者结算结果确认
        //this->ReqSettlementInfoConfirm();
		TThostFtdcInstrumentIDType instrument="m1701";
		//ReqOrderInsert(instrument, Buy, Open, 2850, 1);
		//ReqMarketPriceOrderInsert(instrument, Buy, Open, 1);
		//ReqSettlementInfoConfirm();
		//ReqOrderInsert(INSTRUMENT_ID, Buy, Open, 2850, 1);
		//ReqOrderInsert(INSTRUMENT_ID, Buy, Open, 2849, 1);
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, this->BROKER_ID);
	strcpy(req.InvestorID, this->INVESTOR_ID);
	int iResult = pTraderUserApi->ReqSettlementInfoConfirm(&req, ++traderIRequestID);
	cerr << "--->>> 投资者结算结果确认: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	TThostFtdcInstrumentIDType INSTRUMENT_ID = "m1701";
	cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询合约

		CThostFtdcQryInstrumentField QryInstrument;
		memset(&QryInstrument,0,sizeof(QryInstrument));
		int iResult = pTraderUserApi->ReqQryInstrument(&QryInstrument, ++traderIRequestID);

		bool success =false;

		///请求查询资金账户
		CThostFtdcQryTradingAccountField QryTradingAccount;
		memset(&QryTradingAccount,0,sizeof(QryTradingAccount));
		iResult = pTraderUserApi->ReqQryTradingAccount(&QryTradingAccount, ++traderIRequestID);
		if(iResult==0){
			success = true;
		}

		///查询持仓
		CThostFtdcQryInvestorPositionField req;
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, BROKER_ID);
		strcpy(req.InvestorID, INVESTOR_ID);
		iResult = pTraderUserApi->ReqQryInvestorPosition(&req, ++traderIRequestID);
		if(iResult==0){
			success = true;
		}

		///查询报单
		CThostFtdcQryOrderField QryOrder;
		memset(&QryOrder,0,sizeof(QryOrder));
		iResult = pTraderUserApi->ReqQryOrder(&QryOrder, ++traderIRequestID);
		if(iResult==0){
			success = true;
		}
		///查询成交
		CThostFtdcQryTradeField QryTrade;
		memset(&QryTrade,0,sizeof(QryTrade));
		iResult = pTraderUserApi->ReqQryTrade(&QryTrade, ++traderIRequestID);
		if(iResult==0){
			success = true;
		}


		///取手续费率
		CThostFtdcQryInstrumentCommissionRateField QryInstrumentCommissionRate;
		memset(&QryInstrumentCommissionRate,0,sizeof(QryInstrumentCommissionRate));
		strcpy(QryInstrumentCommissionRate.BrokerID, BROKER_ID);
		strcpy(QryInstrumentCommissionRate.InvestorID, INVESTOR_ID);
		strcpy(QryInstrumentCommissionRate.InstrumentID, INSTRUMENT_ID);
		iResult = pTraderUserApi->ReqQryInstrumentCommissionRate(&QryInstrumentCommissionRate, ++traderIRequestID);

		///取保证金率
		CThostFtdcQryInstrumentMarginRateField QryInstrumentMarginRate;
		memset(&QryInstrumentMarginRate,0,sizeof(QryInstrumentMarginRate));
		strcpy(QryInstrumentMarginRate.BrokerID, BROKER_ID);
		strcpy(QryInstrumentMarginRate.InvestorID, INVESTOR_ID);
		strcpy(QryInstrumentMarginRate.InstrumentID, INSTRUMENT_ID);
		///投机
		QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Speculation;
		iResult = pTraderUserApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++traderIRequestID);

		///套利
		QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Arbitrage;
		iResult = pTraderUserApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++traderIRequestID);

		///套保
		QryInstrumentMarginRate.HedgeFlag=THOST_FTDC_HF_Hedge;
		iResult = pTraderUserApi->ReqQryInstrumentMarginRate(&QryInstrumentMarginRate,++traderIRequestID);
	}
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cout<<"instrument info: "<<pInstrument->InstrumentID<<endl;
}

void CTraderSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cerr << "--->>> " << "OnRspQryInstrumentCommissionRate" << endl;
	if(pInstrumentCommissionRate!=NULL){

		cout<<pInstrumentCommissionRate->InstrumentID<<" "<< pInstrumentCommissionRate->OpenRatioByMoney <<" " << pInstrumentCommissionRate->OpenRatioByVolume <<endl;
		cout<<"end sleep"<<endl;
	}

	if(bIsLast){
		//WriteCommissionRate2File();
	}
}

void CTraderSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cerr << "--->>> " << "OnRspQryInstrumentMarginRate" << endl;
	if(pInstrumentMarginRate!=NULL){
		cout<< pInstrumentMarginRate->InstrumentID<<" "<< pInstrumentMarginRate->HedgeFlag<<" "<<pInstrumentMarginRate->LongMarginRatioByMoney<<" "<<pInstrumentMarginRate->LongMarginRatioByVolume<<endl;
	}
}

void CTraderSpi::ReqQryInstrument()
{
	TThostFtdcInstrumentIDType INSTRUMENT_ID = "m1701";
	CThostFtdcQryInstrumentField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryInstrument(&req, ++traderIRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> 请求查询合约: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			break;
		}
		else
		{
			cerr << "--->>> 请求查询合约: "  << iResult << ", 受到流控" << endl;
		}
	} // while
}

//void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
//{
//	cerr << "--->>> " << "OnRspQryInstrument" << endl;
//	if (bIsLast && !IsErrorRspInfo(pRspInfo))
//	{
//		///请求查询合约
//		ReqQryTradingAccount();
//	}
//}

void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryTradingAccount(&req, ++traderIRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> 请求查询资金账户: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			break;
		}
		else
		{
			cerr << "--->>> 请求查询资金账户: "  << iResult << ", 受到流控" << endl;
		}
	} // while
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///请求查询投资者持仓
		ReqQryInvestorPosition();
	}
}

void CTraderSpi::ReqQryInvestorPosition()
{
	TThostFtdcInstrumentIDType INSTRUMENT_ID = "m1701";
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryInvestorPosition(&req, ++traderIRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> 请求查询投资者持仓: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			break;
		}
		else
		{
			cerr << "--->>> 请求查询投资者持仓: "  << iResult << ", 受到流控" << endl;
		}
	} // while
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///报单录入请求
		cout<<pInvestorPosition->InstrumentID<<pInvestorPosition->Position<<pInvestorPosition->OpenVolume<<pInvestorPosition->PositionProfit<<endl;
	}
}

void CTraderSpi::ReqOrderInsert(TThostFtdcInstrumentIDType instrumentID, 
								TThostFtdcDirectionType direction,
								char offsetFlag,
								TThostFtdcPriceType price,
								TThostFtdcVolumeType volume
								)
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, INVESTOR_ID);
	///合约代码
	strcpy(req.InstrumentID, instrumentID);

	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///报单价格条件: 限价
	req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
	///买卖方向:
	//req.Direction = THOST_FTDC_D_Buy;
	req.Direction = direction;

	///组合开平标志: 开仓
	//req.CombOffsetFlag[0] = THOST_FTDC_OFEN_Open;
	req.CombOffsetFlag[0] = offsetFlag;

	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	//req.LimitPrice = LIMIT_PRICE;
	req.LimitPrice = price;

	///数量: 1
	//req.VolumeTotalOriginal = 1;
	req.VolumeTotalOriginal = volume;

	///有效期类型: 当日有效
	//req.TimeCondition = THOST_FTDC_TC_IOC;
	req.TimeCondition = THOST_FTDC_TC_GFD;
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///用户强评标志: 否
	req.UserForceClose = 0;
	AutoIncOrderRef();
	int iResult = pTraderUserApi->ReqOrderInsert(&req, ++traderIRequestID);
	cerr << "--->>> 报单录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
	MarketUtil::AddGlobalTradeCnt();
	LOG(INFO) << "RequestLimitPriceOrderInsert --> GlobalTradeCnt : " << MarketUtil::GetGlobalTradeCnt();
	if(MarketUtil::GetGlobalTradeCnt() > 100)
		LOG(FATAL) << "GlobalTradeCnt exceeds 100. Too much Trade! Program will crash!";
}

void CTraderSpi::ReqMarketPriceOrderInsert(TThostFtdcInstrumentIDType instrumentID, 
										   TThostFtdcDirectionType direction,
										   char offsetFlag,
										   TThostFtdcVolumeType volume
										   )
{
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, BROKER_ID);
	///投资者代码
	strcpy(req.InvestorID, INVESTOR_ID);
	///合约代码
	strcpy(req.InstrumentID, instrumentID);

	///报单引用
	strcpy(req.OrderRef, ORDER_REF);
	req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
	///买卖方向:
	req.Direction = direction;

	///组合开平标志: 开仓
	req.CombOffsetFlag[0] = offsetFlag;

	///组合投机套保标志
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///价格
	//req.LimitPrice = LIMIT_PRICE;
	req.LimitPrice = 0;

	///数量: 1
	req.VolumeTotalOriginal = volume;

	///有效期类型: 当日有效
	//req.TimeCondition = THOST_FTDC_TC_IOC;
	req.TimeCondition = THOST_FTDC_TC_IOC;
	///GTD日期
	//	TThostFtdcDateType	GTDDate;
	///成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///最小成交量: 1
	req.MinVolume = 1;
	///触发条件: 立即
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///止损价
	//	TThostFtdcPriceType	StopPrice;
	///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///业务单元
	//	TThostFtdcBusinessUnitType	BusinessUnit;
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///用户强评标志: 否
	req.UserForceClose = 0;

	AutoIncOrderRef();
	int iResult = pTraderUserApi->ReqOrderInsert(&req, ++traderIRequestID);
	cerr << "--->>> 报单录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
	MarketUtil::AddGlobalTradeCnt();
	LOG(INFO) << "RequestMarketOrderInsert --> GlobalTradeCnt : " << MarketUtil::GetGlobalTradeCnt();
	if(MarketUtil::GetGlobalTradeCnt() > 100)
		LOG(FATAL) << "GlobalTradeCnt exceeds 100. Too much Trade! Program will crash!";
}

void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	static bool ORDER_ACTION_SENT = false;		//是否发送了报单
	if (ORDER_ACTION_SENT)
		return;

	CThostFtdcInputOrderActionField req;
	memset(&req, 0, sizeof(req));
	///经纪公司代码
	strcpy(req.BrokerID, pOrder->BrokerID);
	///投资者代码
	strcpy(req.InvestorID, pOrder->InvestorID);
	///报单操作引用
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///报单引用
	strcpy(req.OrderRef, pOrder->OrderRef);
	///请求编号
	//	TThostFtdcRequestIDType	RequestID;
	///前置编号
	req.FrontID = FRONT_ID;
	///会话编号
	req.SessionID = SESSION_ID;
	///交易所代码
	//	TThostFtdcExchangeIDType	ExchangeID;
	///报单编号
	//	TThostFtdcOrderSysIDType	OrderSysID;
	///操作标志
	req.ActionFlag = THOST_FTDC_AF_Delete;
	///价格
	//	TThostFtdcPriceType	LimitPrice;
	///数量变化
	//	TThostFtdcVolumeType	VolumeChange;
	///用户代码
	//	TThostFtdcUserIDType	UserID;
	///合约代码
	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pTraderUserApi->ReqOrderAction(&req, ++traderIRequestID);
	cerr << "--->>> 报单操作请求: "  << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
	ORDER_ACTION_SENT = true;
}

void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
								  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

///报单通知
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	LOG(INFO) << "--->>> " << "OnRtnOrder  " <<"orderstatus: " << pOrder->OrderStatus;
}

///成交通知
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
	LOG(INFO) << "--->>> " << "OnRtnTrade  " <<pTrade->InstrumentID<<"  成交数量"<<pTrade->Volume <<"  成交价格："<<pTrade->Price;
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	LOG(INFO) << "--->>> " << "OnRspError";
	IsErrorRspInfo(pRspInfo);
}

bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		LOG(INFO) << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg;
	return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
		(pOrder->SessionID == SESSION_ID) &&
		(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
		(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

void CTraderSpi::QueryOrder()
{
	TThostFtdcInstrumentIDType INSTRUMENT_ID;
	CThostFtdcQryOrderField q;
	memset(&q, 0, sizeof(q));
	strcpy(q.BrokerID, BROKER_ID);
	strcpy(q.InvestorID, INVESTOR_ID);
	strcpy(q.InstrumentID, INSTRUMENT_ID);
	int status = pTraderUserApi->ReqQryOrder(&q, ++traderIRequestID);
}

void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo,
							   int nRequestID, bool bIsLast)
{
	if(pOrder == NULL)
		cout << "没有查询到报单数据..." << endl;
	else
		cout << pOrder->InstrumentID << " " << pOrder->VolumeTotal << " " <<
		pOrder->VolumeTraded << " " << pOrder->InsertTime << " " <<pOrder->LimitPrice 
		<< " " << pOrder->CancelTime << endl;
}

void CTraderSpi::SetTradeAddress(const string &tradeAddress)
{
	strcpy(this->Trade_FRONT_ADDR, tradeAddress.c_str());
}
 
void CTraderSpi::SetBrokerId(const string &brokerID)
{
    strcpy(this->BROKER_ID, brokerID.c_str());
}
 
void CTraderSpi::SetInvestorID(const std::string &investorID)
{
	strcpy(this->INVESTOR_ID, investorID.c_str());
}
 
void CTraderSpi::SetPassword(const std::string &passwd)
{
    strcpy(this->PASSWORD, passwd.c_str());
}

void CTraderSpi::DebugInfo()
{
	printf("brokerid : %s\n", this->BROKER_ID);
	printf("investorid : %s\n", this->INVESTOR_ID);
	printf("password : %s\n", this->PASSWORD);
}

bool CTraderSpi::IsTradeInited() {
    return  this->tradeInited;
}

int ReqPositionVolume(const std::string instrument) {
	// To Do
	return  0;
}












