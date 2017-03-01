#include "tutils.h"
#include "market.h"
#include "BaseData.h"
#include <cstdio>
#include <cstring>
#include <cassert>
#include <ctime>
#include <string>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
using namespace std;

// 请求编号

std::string CMdSpi::GetInput(std::string prompt)
{
	printf("%s", prompt.c_str());
	string line;
	cin >> line;
	return  line;
}

CMdSpi::CMdSpi(CThostFtdcMdApi* pMarketUserApi)
{
	this->pMarketUserApi = pMarketUserApi;
	this->marketMap.clear();
	this->marketIRequestID = 0;
	this->marketInited = false;
}

void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
						int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}

void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	this->ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
    cout << "Is Requesting..." << endl;
	int iResult = pMarketUserApi->ReqUserLogin(&req, ++marketIRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
							CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pMarketUserApi->GetTradingDay() << endl;
		// 请求订阅行情
		SubscribeMarketData();
	}
}

void CMdSpi::SubscribeMarketData()
{
	int iResult = pMarketUserApi->SubscribeMarketData(marketInstrumentID, (int)this->GetInstrumentListSize());
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubMarketData succeed..." << endl;
    this->marketInited = true;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	Time t(pDepthMarketData->UpdateTime);
	time_t now = time(0);
	tm *ltm = localtime(&now);
	if(abs(ltm->tm_min - t.minute) >= 3)  {
		printf("%s --> time : remote differ too much from local.\n", pDepthMarketData->InstrumentID);
		return;
	}

	string instrument = pDepthMarketData->InstrumentID;
	Tick tick(instrument, pDepthMarketData->UpdateTime, pDepthMarketData->LastPrice);
    if(!MarketUtil::IsValidTradeTime(instrument, tick.GetTs())) {
        return;
	}
    this->current_time_.set_time(pDepthMarketData->UpdateTime);

	// return;
	// Update MinuteData List & Data Table.
    vector<MinuteData> &minutes_vec = this->GetMinutesList(instrument);
	Data& data = this->get_data_table(pDepthMarketData->InstrumentID);
	if(minutes_vec.size() == 0) {
		minutes_vec.push_back(MinuteData(tick));
		data.add(tick.price, tick.price, tick.price, tick.price);
	} else if (minutes_vec.back().SameMinute(tick)) {
		minutes_vec.back().Update(tick);
		data.update(tick.price);
	} else {
		minutes_vec.push_back(MinuteData(tick));
		data.add(tick.price, tick.price, tick.price, tick.price);
	}

    this->last_time[pDepthMarketData->InstrumentID] = t;
    market_map_mutex.lock();
	this->marketMap[pDepthMarketData->InstrumentID] = pDepthMarketData;
    market_map_mutex.unlock();
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}

void CMdSpi::SetMarketAddress(const string &marketAddress)
{
	strcpy(this->MARKET_FRONT_ADDR, marketAddress.c_str());
}

void CMdSpi::SetBrokerId(const std::string &brokerID)
{
	strcpy(this->BROKER_ID, brokerID.c_str());
}

void CMdSpi::SetInvestorID(const std::string &investorID) 
{
	strcpy(this->INVESTOR_ID, investorID.c_str());
}

void CMdSpi::SetPassword(const std::string &passwd)
{
	strcpy(this->PASSWORD, passwd.c_str());
}

int CMdSpi::GetInstrumentListSize()
{
	return  this->instrumentList.size();
}

void CMdSpi::SetInstrumentList(const std::vector< std::string> &list)
{
	this->instrumentList.clear();
	int cnt = (int)list.size();
	for(int i = 0; i < cnt; i++)
	{
		this->instrumentList.push_back(list[i]);
		strcpy(this->cInstrumentList[i], list[i].c_str());
		this->marketInstrumentID[i] = cInstrumentList[i];
	}
}

void CMdSpi::DebugInfo()
{
	printf("BrokerID = %s\n", this->BROKER_ID);
	printf("InvestorID = %s\n", this->INVESTOR_ID);
	printf("Password = %s\n", this->PASSWORD);
	for(int i = 0; i < this->instrumentList.size(); i++)
	{
		printf("%s\n", cInstrumentList[i]);
	}
}

bool CMdSpi::IsMarketInited() {
	return  this->marketInited;
}

double CMdSpi::GetLastPrice(const std::string& instrument) {
    if(marketMap.find(instrument) == marketMap.end())
        return  -1.0;
	market_map_mutex.lock();
	CThostFtdcDepthMarketDataField *field = marketMap[instrument];
	double ans = field->LastPrice;
	market_map_mutex.unlock();
	return  ans;
}

double CMdSpi::GetOpenPrice(const std::string& instrument) {
    if(marketMap.find(instrument) == marketMap.end())
        return  -1.0;
	market_map_mutex.lock();
	CThostFtdcDepthMarketDataField *field = marketMap[instrument];
	double openprice = field->OpenPrice;
	market_map_mutex.unlock();
	return  openprice;
}

int CMdSpi::get_ts() {
    return this->current_time_.get_ts();
}

std::string CMdSpi::get_trading_day()
{
	return string(this->pMarketUserApi->GetTradingDay());
}

std::vector<MinuteData>& CMdSpi::GetMinutesList(const std::string& instrument)
{
	if(this->m_map.find(instrument) == this->m_map.end())
		this->m_map[instrument] = vector<MinuteData>();
	return  this->m_map[instrument];
}

int CMdSpi::GetMinutesListSize(const string instrument)
{
	return this->m_map[instrument].size();
}
double CMdSpi::GetMinutesRangeMax(const string& instrument, int start, int last)
{
	vector<MinuteData> &list = this->m_map[instrument];
	double ans = list[start].GetHighPrice();
	for(int i = start+1; i <= last; i++)
		ans = max(ans, list[i].GetHighPrice());
	return  ans;
}
double CMdSpi::GetMinutesRangeMin(const string& instrument, int start, int last)
{
	vector<MinuteData> &list = this->m_map[instrument];
	double ans = list[start].GetLowPrice();
	for(int i = start+1; i <= last; i++)
		ans = min(ans, list[i].GetLowPrice());
	return  ans;
}

double CMdSpi::GetMinutesTailRangeMax(const std::string& instrument, int tailcount) {
    vector<MinuteData> &list = this->m_map[instrument];
    assert(tailcount < list.size());
    int start = list.size() - tailcount - 1;
    int end = start + tailcount - 1;
    double ans = list[start].GetHighPrice();
	for(int i = start + 1; i <= end; i++)
		ans = max(ans, list[i].GetHighPrice());
	return  ans;
}

double CMdSpi::GetMinutesTailRangeMin(const std::string& instrument, int tailcount) {
	vector<MinuteData> &list = this->m_map[instrument];
    assert(tailcount < list.size());
	int start = list.size() - tailcount - 1;
	int end = start + tailcount - 1;
	double ans = list[start].GetLowPrice();
	for(int i = start + 1; i <= end; i++)
		ans = min(ans, list[i].GetLowPrice());
	return  ans;
}

MinuteData CMdSpi::GetMinuteDataAtIndex(const string instrument, int index)
{
	vector<MinuteData> &list = this->m_map[instrument];
    assert(index < list.size());
	return  list[index];
}

void CMdSpi::save_all() {
    FILE* file = fopen("./data_all.mdt", "wb");
    for(auto it = this->m_map.begin(); it != this->m_map.end(); it++)
    {
		auto& vec = it->second;
		for(const auto& e : vec)
		{
			fprintf(file, "%s %s %.4f %.4f %.4f %.4f\n",
					e.GetInstrument().c_str(), e.GetTime().c_str(), e.open, e.high, e.low, e.close);
		}
    }
    fclose(file);
}

void CMdSpi::load_all() {
	FILE* file = fopen("./data_all.mdt", "rb");
	char instrument[30], t[15];
	double open, high, low, close;
	while(fscanf(file, "%s %s %lf %lf %lf %lf", instrument, t, &open, &high, &low, &close) != EOF) {
		Data& data = this->get_data_table(instrument);
		data.add(open, high, low, close);
	}
	fclose(file);
}

void CMdSpi::show_bar_data(const string& instrument) {
	Data& data = this->get_data_table(instrument);
	for(int i = 0; i < data.cur; i++) {
		printf("%s %.4f %.4f %.4f %.4f\n", instrument.c_str(), data.open[i], data.high[i], data.low[i], data.close[i]);
	}
}

void CMdSpi::ShowMinutesData(const std::string instrument) {
    if(this->m_map.find(instrument) == this->m_map.end())
		return;
    vector<MinuteData> &list = this->m_map[instrument];
    int size = list.size();
	if(size == 0) {
		cout << "Empty minutes list..." << endl;
		return;
	}
    else {
        for (int i = 0; i < size; i++) {
            cout << list[i].ToString() << " " << list[i].GetTs() << endl;
        }
    }
}

Data& CMdSpi::get_data_table(const string& instrument)
{
	if(data_table.find(instrument) == data_table.end())
	{
		data_table[instrument] = Data();
		data_table[instrument].clear();
	}
	return  data_table[instrument];
}







