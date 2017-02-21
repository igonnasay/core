#ifndef _MARKET_H
#define _MARKET_H

#ifdef MATHLIBRARY_EXPORTS
#define MARKET_API __declspec(dllexport) 
#else
#define MARKET_API __declspec(dllimport) 
#endif

#ifndef DLL
//#define DLL __declspec(dllexport)
#endif

#include "BaseData.h"
#include "tutils.h"
#include "ThostFtdcMdApi.h"
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

using namespace std;

class CMdSpi : public CThostFtdcMdSpi
{
public:
	
	static std::string GetInput(std::string prompt);


	CMdSpi(CThostFtdcMdApi* pMarketUserApi);
	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///心跳超时警告。当长时间未收到报文时，该方法被调用。
	///@param nTimeLapse 距离上次接收报文的时间
	virtual void OnHeartBeatWarning(int nTimeLapse);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	//初始化
	void SetMarketAddress(const std::string&);
	void SetBrokerId(const std::string &brokerID);
	void SetInvestorID(const std::string &investorID);
	void SetPassword(const std::string &passwd);
	int GetInstrumentListSize();
	void SetInstrumentList(const std::vector< std::string> &list);

	//服务接口API

	void DebugInfo();
    bool IsMarketInited();
	double GetLastPrice(const std::string& instrument);
	double GetOpenPrice(const std::string& instrument);
    int get_ts();
	std::string get_trading_day();

	// API for MinuteData
    std::vector<MinuteData>& GetMinutesList(const std::string& instrument);
	int GetMinutesListSize(const std::string instrument);
	double GetMinutesRangeMax(const std::string& instrument, int start, int last);
	double GetMinutesRangeMin(const std::string& instrument, int start, int last);
    double GetMinutesTailRangeMax(const std::string& instrument, int tailcount);
	double GetMinutesTailRangeMin(const std::string& instrument, int tailcount);
	MinuteData GetMinuteDataAtIndex(const std::string instrument, int index);
    void ShowMinutesData(const std::string instrument);
    void save_all();

	// API for 5 Minutes
	// old version
	void Get5MinutesBarList(const std::string& instrument, std::vector<Bar>& vec);
	double Get5MinutesBarTailRangeMax(const std::string& instrument, int tailcnt);
	double Get5MinutesBarTailRangeMin(const std::string& instrument, int tailcnt);
	void Show5MinutesData(const std::string& instrument);

	// Public Getter
	Data& get_data_table(const string& instrument);

	// Public field
	map<string, CThostFtdcDepthMarketDataField* > marketMap;
	map<string, Time> last_time;

private:
    bool marketInited;
	int marketIRequestID;
	Time current_time_;
	CThostFtdcMdApi* pMarketUserApi;
	void ReqUserLogin();
	void SubscribeMarketData();
	bool IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo);

	char MARKET_FRONT_ADDR[105];
	TThostFtdcBrokerIDType	BROKER_ID;	
	//char MARKET_FRONT_ADDR[] = "tcp://180.168.212.76:41213";		// 海通期货行情前置地址
	TThostFtdcInvestorIDType INVESTOR_ID;			// 投资者代码
	TThostFtdcPasswordType  PASSWORD;			// 用户密码
	std::vector< std::string > instrumentList;
	char cInstrumentList[105][15];
	char *marketInstrumentID[105];			// 行情订阅列表
	std::mutex market_map_mutex;

	map<string, std::vector<MinuteData> >  m_map;
	map<string, Data> data_table;
};

#endif






