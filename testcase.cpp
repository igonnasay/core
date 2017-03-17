//
// Created by AIdancer on 16/10/4.
//

#include "base_data.h"
#include <iostream>
#include <string>
#include <cassert>

using namespace std;

void TestTime()
{
    char str[100] = "09:01:32";
    Time time(str);
    string content = time.ToString();
    assert(strcmp(str, content.c_str()) == 0);
}

void TestMinute()
{
    char t1[100] = "09:03:12";
    char t2[100] = "21:59:59";
    Tick tick("m1701", "09:03:09", 2300);
    Tick tick2("m1701", "09:03:51", 2309);
    Tick tick3("m1701", "09:04:01", 2203);
    Tick tick4("m1701", "09:03:23", 2289);
    Tick tick5("m1701", "09:03:59", 2302);
    MinuteData minuteData(tick);
    minuteData.Update(tick2);
    minuteData.Update(tick3);
    minuteData.Update(tick4);
    minuteData.Update(tick5);
    assert(minuteData.GetOpenPrice()-2300 < 1e-7);
    assert(minuteData.GetClosePrice() - 2302 < 1e-7);
    assert(minuteData.GetLowPrice() - 2289 < 1e-7);
    assert(minuteData.GetHighPrice() - 2309 < 1e-7);
//    printf("open = %.2f  close = %.2f  low = %.2f  high = %.2f\n", minuteData.GetOpenPrice(), minuteData.GetClosePrice(),
//            minuteData.GetLowPrice(), minuteData.GetHighPrice());
}

int main()
{
    TestTime();
    TestMinute();
    return  0;
}





