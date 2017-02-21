//
// Created by AIdancer on 17/2/12.
//

#include "gtest/gtest.h"
#include "../BaseData.h"
#include "../tutils.h"
#include "easylogging++.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/regex.hpp>

INITIALIZE_EASYLOGGINGPP

using namespace std;

TEST(TestSample, Add)
{
    int a = 2, b = 3;
    EXPECT_EQ(a + b, 5);
}

TEST(BaseData, Time)
{
    Time t1("20:58:59");
    Time t2("20:58:59");
    EXPECT_EQ(t1.get_ts(), t2.get_ts());
    EXPECT_TRUE(MarketUtil::is_same_second(t1.get_ts(), t2.get_ts()));
    t2.second = 58;
    EXPECT_FALSE(MarketUtil::is_same_second(t1.get_ts(), t2.get_ts()));
    EXPECT_TRUE(MarketUtil::is_same_minute(t1.get_ts(), t2.get_ts()));
    EXPECT_TRUE(MarketUtil::is_same_5minute(t1.get_ts(), t2.get_ts()));
    EXPECT_EQ(t1.get_ts()-t2.get_ts(), 1);
    t2 = Time(20*3600 + 58*60 + 59);
    EXPECT_EQ(t1.get_ts(), t2.get_ts());
}

TEST(Tutils, BarList)
{
    Bar bar;
    bar.set_bar_data("00:00:00", 2000, 2500, 1800, 2200.0);
    BarList list;
    list.set_bar_size(5);
    for(int i = 0; i < 10; i++)
    {
        list.add_bar(bar);
        bar.b_time.second += 1;
        bar.high -= 1;
        bar.low += 1;
    }
    EXPECT_NEAR(list.get_range_open(0, 10), 2000, 1e-6);
    EXPECT_NEAR(list.get_range_close(0, 10), 2200.00, 1e-6);
    EXPECT_NEAR(list.get_range_high(0, 10), 2500, 1e-6);
    EXPECT_NEAR(list.get_range_low(0, 10), 1800, 1e-6);
    EXPECT_NEAR(list.get_range_high(1, 10), 2499, 1e-6);
    EXPECT_NEAR(list.get_range_low(1, 9), 1801, 1e-6);
}

TEST(Regex, Example)
{
    string str = "192.168.1.1";
    boost::regex pattern("^[0-9]+.[0-9]+.\\d+.[0-9]+$");
    boost::smatch ret;
    bool flag = boost::regex_match(str, ret, pattern);
    EXPECT_TRUE(flag);
    str = "exp:1001:m1705:buy:>=:2900.5:2880.004:2990";
    pattern = boost::regex("^exp:\\d+:\\S+:\\S+:\\S+:\\S+:\\S+:\\S+$");
    EXPECT_TRUE(boost::regex_match(str, ret, pattern));
}

TEST(BaseData, ExpInfo)
{
    string str = "exp:1001:m1705:buy:>=:2900.5:2880.004:2990";
    EXPECT_TRUE(ExpInfo::is_exp_str(str));
    vector<string> ret;
    ExpInfo::split(str, ret);
    EXPECT_EQ(ret[0], "exp");
    EXPECT_EQ(ret[1], "1001");
    EXPECT_EQ(ret[2], "m1705");
    EXPECT_EQ(ret[3], "buy");
    EXPECT_EQ(ret[4], ">=");
    EXPECT_EQ(ret[5], "2900.5");
    EXPECT_EQ(ret[6], "2880.004");
    EXPECT_EQ(ret[7], "2990");

    ExpInfo info(str);
    EXPECT_EQ(info.action_id, 1001);
    EXPECT_EQ(info.instrument, "m1705");
    EXPECT_EQ(info.action, "buy");
    EXPECT_EQ(info.condition, ">=");
    EXPECT_NEAR(info.action_price, 2900.5, 1e-6);
    EXPECT_NEAR(info.stop_loss, 2880.004, 1e-6);
    EXPECT_NEAR(info.stop_profit, 2990, 1e-6);
    EXPECT_EQ(info.status, '-');

    EXPECT_TRUE(info.open_suggest(2901.5));
    info.set_status('b');
    EXPECT_TRUE(info.close_suggest(2800));
    EXPECT_TRUE(info.close_suggest(3000.0));

    str = "exp:1001:m1705:sell:<:2900:2910:2700";
    EXPECT_TRUE(ExpInfo::is_exp_str(str));
    info = ExpInfo(str);
    EXPECT_TRUE(info.open_suggest(2899));
    info.set_status('s');
    EXPECT_TRUE(info.open_suggest(2899));
    EXPECT_FALSE(info.open_suggest(2901));
    EXPECT_TRUE(info.close_suggest(2912));
    EXPECT_TRUE(info.close_suggest(2650));
}

TEST(BaseData, Data)
{
    Data data;
    EXPECT_EQ(data.cur, 0);
    data.add(2900, 2900, 2900, 2900);
    for(int i = 0; i < 10; i++){
        double tmp = 2900.0 + i + 1;
        data.add(tmp, tmp, tmp, tmp);
    }
    data.update(2800);
    EXPECT_EQ(data.cur, 11);
    EXPECT_NEAR(data.open[data.cur-1], 2910, 1e-6);
    EXPECT_NEAR(data.high[data.cur-1], 2910, 1e-6);
    EXPECT_NEAR(data.low[data.cur-1], 2800, 1e-6);
}

TEST(Tutils, read_data)
{
    string path = "./data.mdt";
    double eps = 1e-6;
    Data dt;
    MarketUtil::read_data_from_mdt(path, dt);
    EXPECT_EQ(dt.cur, 15);
    EXPECT_NEAR(dt.open[0], 2900, eps);
    EXPECT_NEAR(dt.high[0], 2910, eps);
    EXPECT_NEAR(dt.low[0], 2809, eps);
    EXPECT_NEAR(dt.close[0], 2901, eps);
    int last = dt.cur - 1;
    EXPECT_NEAR(dt.open[last], 2883, eps);
    EXPECT_NEAR(dt.high[last], 2890, eps);
    EXPECT_NEAR(dt.low[last], 2700, eps);
    EXPECT_NEAR(dt.close[last], 2701, eps);

    EXPECT_NEAR(*max_element(dt.high, dt.high+dt.cur), 2910, eps);
    EXPECT_NEAR(*min_element(dt.low, dt.low+dt.cur), 2700, eps);
}

TEST(Tutils, strip)
{
    string str = "  hello \n \t  \n \t";
    EXPECT_EQ(MarketUtil::strip(str), "hello");
    cout << MarketUtil::strip(str) << endl;
    str = "";
    EXPECT_EQ(MarketUtil::strip(str), "");
    str = "a";
    EXPECT_EQ(MarketUtil::strip(str), "a");
}

TEST(tutils, split)
{
	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
	double x = 42.0;
	EXPECT_NEAR(x, 42.0, 1e-6);
}











