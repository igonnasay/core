#include "base_data.h"
#include "easylogging++.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

Time::Time() {}

Time::Time(const string& t)
{
  this->set_time(t);
}
Time::Time(const int ts)
{
	this->set_time(ts);
}

void Time::set_time(int _ts)
{
	this->hour = _ts / 3600;
	this->minute = (_ts % 3600) / 60;
	this->second = _ts % 60;
}

Time::Time(int _hour, int _minute, int _second)
{
	this->set_time(_hour, _minute, _second);
}
int Time::get_ts() {
	return this->hour * 3600 + this->minute * 60 + this->second;
}

void Time::set_time(const string& t)
{
	sscanf(t.c_str(), "%d:%d:%d", &this->hour, &this->minute, &this->second);
}
void Time::set_time(int _hour, int _minute, int _second)
{
	this->hour = _hour;
	this->minute = _minute;
	this->second = _second;
}

string Time::ToString() const
{
	char ret[100];
	sprintf(ret, "%02d:%02d:%02d", this->hour, this->minute, this->second);
	return  string(ret);
}

Data::Data() {
  this->clear();
}

void Data::clear() {
	instrument = "";
	cur = 0;
	memset(open, 0, sizeof(open));
	memset(high, 0, sizeof(high));
	memset(low, 0, sizeof(low));
	memset(close, 0, sizeof(close));
	memset(close_sum, 0, sizeof(close_sum));
}

void Data::add(double _open, double _high, double _low, double _close)
{
	open[cur] = _open;
	high[cur] = _high;
	low[cur] = _low;
	close[cur] = _close;
	++cur;
	if(cur >= Data::N)
	LOG(FATAL) << this->instrument << " Data's size has exceeded it's capacity.";
	this->update_close_sum();
}

void Data::update(double _open, double _high, double _low, double _close)
{
	int index = cur - 1;
	if(index < 0 )  LOG(FATAL) << "index out of bound.";
	open[index] = _open;
	high[index] = _high;
	low[index] = _low;
	close[index] = _close;
	this->update_close_sum();
}

void Data::update(double _price)
{
  int index = cur - 1;
	if(index < 0 )  LOG(FATAL) << "index out of bound.";
	high[index] = max(high[index], _price);
	low[index] = min(low[index], _price);
	close[index] = _price;
	this->update_close_sum();
}

void Data::update_close_sum()
{
	int index = this->cur - 1;
	if(index < 0)  return;
	else if(index == 0)  close_sum[index] = close[index];
	else close_sum[index] = close_sum[index-1] + close[index];
}

Tick::Tick(const string &_instrument, const string &_time, double _price)
{
	this->instrument = _instrument;
	this->time = _time;
	this->price = _price;
	this->t_time = Time(_time);
}

string Tick::ToString()
{
	char str[105];
	sprintf(str, "%s %s %.2f", this->instrument.c_str(), this->time.c_str(), this->price);
	return  string(str);
}

string Tick::GetInstrument()
{
	return  this->instrument;
}

string Tick::GetTime()
{
	return  this->time;
}

double Tick::GetPrice()
{
	return  this->price;
}

int Tick::GetTs()
{
	return t_time.get_ts();
}

MinuteData::MinuteData(const string& _instrument, double _open, double _high, double _low, double _close, const Time& _t)
{
	this->instrument = _instrument;
	this->open = _open;
	this->high = _high;
	this->low = _low;
	this->close = _close;
	this->m_time = _t;
	this->time = _t.ToString();
}

MinuteData::MinuteData(Tick &tick)
{
	this->instrument = tick.GetInstrument();
	this->time = tick.GetTime();
	this->open = this->close = this->high = this->low = tick.GetPrice();
	this->m_time = Time(this->time);
}

string MinuteData::ToString() {
	char str[205];
	sprintf(str, "%s %s %.2f %.2f %.2f %.2f", this->instrument.c_str(), this->time.c_str(),
	this->open, this->high, this->low, this->close);
	return string(str);
}

void MinuteData::Update(Tick &tick)
{
	string _time = tick.GetTime();
	Time tick_time(_time);
	if(this->m_time.hour == tick_time.hour && this->m_time.minute == tick_time.minute)
	{
		double new_price = tick.GetPrice();
		this->high = max(this->high, new_price);
		this->low = min(this->low, new_price);
        this->close = new_price;
		this->m_time = tick_time;
        this->time = _time;
	}
	else {
		cerr << "Not in the same minues" << endl;
	}
}

bool MinuteData::SameMinute(Tick &tick) 
{
	if(tick.t_time.hour == m_time.hour && tick.t_time.minute == m_time.minute)
		return  true;
	else
		return  false;
}
double MinuteData::GetOpenPrice()
{
	return  this->open;
}
double MinuteData::GetClosePrice()
{
	return  this->close;
}
double MinuteData::GetHighPrice()
{
	return  this->high;
}
double MinuteData::GetLowPrice()
{
	return  this->low;
}
string MinuteData::GetInstrument() const
{
	return  this->instrument;
}
string MinuteData::GetTime() const
{
	return  this->time;
}
int MinuteData::GetTs() {
	return this->m_time.get_ts();
}

// Class ExpInfo

ExpInfo::ExpInfo(const string& exp_str)
{
	if(!ExpInfo::is_exp_str(exp_str))
	{
		LOG(FATAL) << "exp_str is ilegal";
	} else {
		vector<string> ret;
		ExpInfo::split(exp_str, ret);
		this->instrument = ret[2];
		this->action_id = std::stoi(ret[1].c_str());
		this->action = ret[3];
		this->condition = ret[4];
		this->action_price = std::stod(ret[5].c_str());
		this->stop_loss = std::stod(ret[6]);
		this->stop_profit = std::stod(ret[7]);
		this->status = '-';
	}
}

bool ExpInfo::open_suggest(double price)
{
	if((price > this->action_price) && (this->condition == ">"))
		return  true;
	if((price >= this->action_price) && (this->condition == ">="))
		return  true;
	if((price < this->action_price) && (this->condition == "<"))
		return  true;
	if((price <= this->action_price) && (this->condition == "<="))
		return  true;
	return  false;
}

bool ExpInfo::close_suggest(double price) {
	if((this->status == 'b') && (this->action == "buy") && (price >= this->stop_profit))
		return  true;
	if((this->status == 'b') && (this->action == "buy") && (price <= this->stop_loss))
		return  true;
	if((this->status == 's') && (this->action == "sell") && (price <= this->stop_profit))
		return  true;
	if((this->status == 's') && (this->action == "sell") && (price >= this->stop_loss))
		return  true;
	return false;
}

void ExpInfo::set_status(char _status) {
	this->status = _status;
}

bool ExpInfo::is_exp_str(const string& exp_str)
{
  boost::regex pattern("^exp:\\d+:\\S+:\\S+:\\S+:\\S+:\\S+:\\S+$");
  boost::smatch ret;
	if(boost::regex_match(exp_str, ret, pattern))
		return  true;
	else {
		return false;
	}
}

void ExpInfo::split(const string& exp_str, vector<string>& ret)
{
  boost::algorithm::split(ret, exp_str, boost::algorithm::is_any_of(":"));
}










