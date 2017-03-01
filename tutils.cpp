//
// Created by AIdancer on 17/2/9.
//

#include "tutils.h"
#include "bfunc.h"
#include "easylogging++.h"
#include "ta_func.h"
#include <cstdio>
#include <cassert>

int MarketUtil::IntToTs(int t) {
	int hour = t / 100;
	int minute = t % 100;
	return  hour * 3600 + minute * 60;
}

bool MarketUtil::IsValidTradeTime(const string& instrument, int ts)
{
	auto it = MarketUtil::trade_time_.find(instrument);
	vector<int>& t = it->second;
	int n = t.size();
	if(n & 1)
		LOG(FATAL) << "time periods is not even!";
	for(int i = 0; i < n; i += 2) {
		if(ts>=t[i] && ts<=t[i+1]) {
			return true;
		}
	}
	return  false;
}

bool MarketUtil::is_same_minute(int ts1, int ts2)
{
    Time t1(ts1), t2(ts2);
    return t1.minute == t2.minute;
}

bool MarketUtil::is_same_second(int ts1, int ts2)
{
    Time t1(ts1), t2(ts2);
    return  t1.second == t2.second;
}

bool MarketUtil::is_same_5minute(int ts1, int ts2)
{
    ts1 = ts1 / 60;
    ts2 = ts2 / 60;
    return  (ts1 / 5) == (ts2 / 5);
}

bool MarketUtil::save_minutes_list(const std::vector<MinuteData>& list, std::string path)
{
    FILE* file = fopen(path.c_str(), "a+");
    for(const auto& e: list)
    {
        fprintf(file, "%s %s %.4f %.4f %.4f %.4f\n", e.GetInstrument().c_str(), e.GetTime().c_str(), e.open, e.high, e.low, e.close);
    }
    fclose(file);
}

string MarketUtil::strip(const string& str)
{
    int head = 0, tail = str.length() - 1;
    while(head <= tail && (str[head]==' ' || str[head]=='\t' || str[head]=='\n'))  ++head;
    while(head <= tail && (str[tail]==' ' || str[tail]=='\t' || str[tail]=='\n'))  --tail;
    if(head > tail)  return "";
    else return str.substr(head, tail-head+1);
}

void MarketUtil::split(const string& str, const string& ch_set, vector<string>& result)
{
	bfunc::split_str(str, ch_set, result);
}

int MarketUtil::trade_cnt_ = 0;
map<string, vector<int> > MarketUtil::trade_time_;

int MarketUtil::GetGlobalTradeCnt() {
	return MarketUtil::trade_cnt_;
}

int MarketUtil::AddGlobalTradeCnt() {
	++MarketUtil::trade_cnt_;
}

void MarketUtil::LoadTradeTime() {
	char instrument[31];
	char time_str[200];
	map<string, vector<int> >& table = MarketUtil::trade_time_;
	table.clear();

	FILE* file = fopen("config/rule.cf", "rb");
	vector<string> s_vec;
	vector<int> t_vec;
	while(fscanf(file, "%s %s", instrument, time_str) != EOF) {
		s_vec.clear();
		t_vec.clear();
		bfunc::split_str(time_str, ",", s_vec);
		for(int i = 0; i < s_vec.size(); i++) {
			int t = std::stoi(s_vec[i]);
			t_vec.push_back(MarketUtil::IntToTs(t));
		}
		table[instrument] = t_vec;
	}
	fclose(file);
}

double test_ta()
{
    int n = 10;
    double in[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    double out[10];
    int beg, num;
    int ret = TA_SMA(0, n, in, 3, &beg, &num, out);
    for(int i = 0; i < 10; i++)
    {
        printf("%f ", out[i]);
    }
    printf("\n");
}







