//
// Created by AIdancer on 17/2/9.
//

#ifndef CLION_TUTILS_H
#define CLION_TUTILS_H

#include "base_data.h"

#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

class MarketUtil
{
public:
    const static int Trade_Time_Begin = 9 * 3600;
    const static int Trade_Time_End = 23 * 3600 + 30 * 60;
	static int IntToTs(int t);
    static bool IsValidTradeTime(const string& instrument, int ts);
    static bool is_same_minute(int ts1, int ts2);
    static bool is_same_second(int ts1, int ts2);
    static bool is_same_5minute(int ts1, int ts2);

    static bool save_minutes_list(const vector<MinuteData>& list, string path);

    static string strip(const string& str);
    static void split(const string& str, const string& ch_set, vector<string>& result);

	static int GetGlobalTradeCnt();
	static int AddGlobalTradeCnt();

	static void LoadTradeTime();

private:
	static int trade_cnt_;
	static map<string, vector<int> > trade_time_;
};

#endif //CLION_TUTILS_H
