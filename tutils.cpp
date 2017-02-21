//
// Created by AIdancer on 17/2/9.
//

#include "tutils.h"
#include "easylogging++.h"
#include "ta_func.h"
#include <cstdio>
#include <cassert>
#include <boost/algorithm/string.hpp>

bool MarketUtil::IsValidTradeTime(int ts)
{
    if((ts >= 32400 && ts <= 36900) || (ts >= 37800 && ts <= 41400)
            || (ts >= 48600 && ts <= 54000) || (ts >= 75600 && ts <= 84600))
        return  true;
    else
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

void MarketUtil::read_data_from_mdt(const string& mdt_file, Data& data)
{
    FILE *mdt = fopen(mdt_file.c_str(), "r");
    double _open, _high, _low, _close;
    while(fscanf(mdt, "%lf %lf %lf %lf", &_open, &_high, &_low, &_close) != EOF)
    {
        data.add(_open, _high, _low, _close);
    }
    fclose(mdt);
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
    boost::algorithm::split(result, str, boost::algorithm::is_any_of(ch_set));
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







