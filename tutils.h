//
// Created by AIdancer on 17/2/9.
//

#ifndef CLION_TUTILS_H
#define CLION_TUTILS_H

#include "BaseData.h"

#include <cmath>
#include <cstring>
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
    static bool IsValidTradeTime(int ts);
    static bool is_same_minute(int ts1, int ts2);
    static bool is_same_second(int ts1, int ts2);
    static bool is_same_5minute(int ts1, int ts2);

    static bool save_minutes_list(const vector<MinuteData>& list, string path);
    static void read_data_from_mdt(const string& mdt_file, Data& data);

    static string strip(const string& str);
    static void split(const string& str, const string& ch_set, vector<string>& result);
};

#endif //CLION_TUTILS_H
