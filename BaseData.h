#ifndef basedata_h
#define basedata_h

#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

class Time
{
public:
	Time();
	Time(const string& t);
	Time(const int ts);
	Time(int _hour, int _minute, int _second);
	void set_time(const string& t);
	void set_time(int _ts);
	void set_time(int _hour, int _minute, int _second);
	int get_ts();
	string ToString() const;

    int hour, minute, second;
};

struct Bar {
	double open, high, low, close;

    void set_bar_data(double _open, double _high, double _low, double _close)
    {
        open = _open;
        high = _high;
        low = _low;
        close = _close;
    }
};

struct BarList
{
    int bar_size;
    int cur;
    std::vector<Bar> bar_list;
	void clear() {
		bar_size = 0;
		cur = 0;
		bar_list.clear();
	}
};

struct Data {
	const static int N = 2000;
    Data();
	string instrument;
	int cur;
	double open[N], high[N], low[N], close[N];
	double close_sum[N];
	void clear();
    void add(double _open, double _high, double _low, double _close);
	void update(double _open, double _high, double _low, double _close);
    void update(double _price);
	void update_close_sum();
};

class Tick
{
public:
	Tick(const string &_instrument, const string &_time, double _price);
	string ToString();
	string GetInstrument();
	string GetTime();
	double GetPrice();
	int GetTs();

	Time t_time;
	double price;
	string instrument;
private:
	string time;
};


class MinuteData
{
public:
    MinuteData(const string& _instrument, double _open, double _high, double _low, double _close, const Time& _t);
	MinuteData(Tick &tick);
	void Update(Tick &tick);
	double GetOpenPrice();
	double GetClosePrice();
	double GetHighPrice();
	double GetLowPrice();
	string ToString();;
	bool SameMinute(Tick &tick);
	string GetInstrument() const;
	string GetTime() const;
	int GetTs();
	double open, close, high, low;
	Time m_time;
private:
	string instrument;
	string time;
};

class ExpInfo {
public:
	ExpInfo(const string& exp_str);
    bool open_suggest(double price);
	bool close_suggest(double price);
	void set_status(char _status);
	static bool is_exp_str(const string& exp_str);
	static void split(const string& exp_str, vector<string>& ret);

	string instrument;
	int action_id;
	string action;
	double action_price;
	double stop_loss;
	double stop_profit;
	string condition;
	char status;

private:
};

#endif






