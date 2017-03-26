#ifndef trade_h
#define trade_h

#include "ThostFtdcTraderApi.h"
#include <string>
#include <map>

#define Buy THOST_FTDC_D_Buy
#define Sell THOST_FTDC_D_Sell

#define Open THOST_FTDC_OFEN_Open
#define Close THOST_FTDC_OFEN_Close

using namespace std;

class CTraderSpi : public CThostFtdcTraderSpi
{
public:
	CTraderSpi(CThostFtdcTraderApi *pTraderAPI);
	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询资金账户响应
	virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询投资者持仓响应
	virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	virtual void OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///用户登录请求
	void ReqUserLogin();
	///投资者结算结果确认
	void ReqSettlementInfoConfirm();
	///请求查询合约
	void ReqQryInstrument();
	///请求查询资金账户
	void ReqQryTradingAccount();
	///请求查询投资者持仓
	void ReqQryInvestorPosition(const string& instrument_id);
	///报单录入请求
	void ReqOrderInsert(TThostFtdcInstrumentIDType instrumentID, 
		TThostFtdcDirectionType direction,
		char offsetFlag,
		TThostFtdcPriceType price,
		TThostFtdcVolumeType volume
		);
	void ReqMarketPriceOrderInsert(TThostFtdcInstrumentIDType instrumentID, 
		TThostFtdcDirectionType direction,
		char offsetFlag,
		TThostFtdcVolumeType volume
		);
	///报单操作请求
	void ReqOrderAction(CThostFtdcOrderField *pOrder);

	// 是否收到成功的响应
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);
	// 是否我的报单回报
	bool IsMyOrder(CThostFtdcOrderField *pOrder);
	// 是否正在交易的报单
	bool IsTradingOrder(CThostFtdcOrderField *pOrder);

	void AutoIncOrderRef();

	void QueryOrder();

	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	void SetTradeAddress(const std::string&);
	void SetBrokerId(const std::string &brokerID);
	void SetInvestorID(const std::string &investorID);
	void SetPassword(const std::string &passwd);
	void DebugInfo();
	bool IsTradeInited();
  int ReqPositionVolume(const std::string instrument);
	string GetExchange(const string& instrument);

	// Order API designed by liqiaz
	void BuyOpen(const string& instrument, double price, int volume);
	void SellOpen(const string& instrument, double price, int volume);
	void BuyCloseToday(const string& instrument, double price, int volume);
	void SellCloseToday(const string& instrument, double price, int volume);
	void GiveOrder(const string& instrument, double price, int volume, const string& direction, const string& close_type);

private:
	CThostFtdcInputOrderField GetSampleInputOrderField();
	void SetOrderBrokerID(CThostFtdcInputOrderField &order, const string& broker);
	void SetOrderInvestorID(CThostFtdcInputOrderField &order, const string& investor);
	void SetOrderInstrumentID(CThostFtdcInputOrderField &order, const string& instrument_id);
	void SetOrderRef(CThostFtdcInputOrderField &order, const string& order_ref);
	void SetOrderPrice(CThostFtdcInputOrderField &order, double price);
	void SetOrderDirectionType(CThostFtdcInputOrderField &order, const string& direction);
	void SetOrderOpenCloseType(CThostFtdcInputOrderField &order, const string& direction);
	void SetOrderVolume(CThostFtdcInputOrderField &order, int volume);
	void SendOrder(CThostFtdcInputOrderField &order);

	void ShowInputOrderField(CThostFtdcInputOrderField &order);

  bool tradeInited;
	CThostFtdcTraderApi* pTraderUserApi;
	CThostFtdcOrderField *myOrder;
	char Trade_FRONT_ADDR[105];
	//char Trader_FRONT_ADDR[] = "tcp://180.168.212.76:41205";		// 前置地址
	TThostFtdcBrokerIDType  BROKER_ID;  
	TThostFtdcInvestorIDType INVESTOR_ID;
	TThostFtdcPasswordType  PASSWORD;  

	// 请求编号
	int traderIRequestID;

	// 会话参数
	TThostFtdcFrontIDType	FRONT_ID;	//前置编号
	TThostFtdcSessionIDType	SESSION_ID;	//会话编号
	TThostFtdcOrderRefType	ORDER_REF;	//报单引用

	int iNextOrderRef;

	map<string, string> exch_table;
};


#endif







