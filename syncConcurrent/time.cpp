/**
 * 时间段与时间点
 */

#include <iostream>
#include <time.h>
#include <thread>
#include <ratio>

int main() {
    // period代表时钟节拍，由ratio<_Num, _Den>节拍比率指定，Num代表多少s，Den代表多少步
    std::ratio<10, 5> ratio; // 10s走5拍
    std::ratio<60, 1> minute_ratio; // 60s走1拍，即代表一分钟

    // duration<_Rep, _Period>代表时间段，_Rep代表数值类型，可以是int、long、double等指定，_Period是时钟节拍，代表1代表多大
    std::chrono::duration<long, std::ratio<60, 1>> minute_duration(10); // 可以代表十分钟

    // time_point<_Clock, _Dur>代表时间点，_Clock代表时钟类型，可以是steady_clock(匀速时钟)、system_clock(系统时钟)、
    // high_resolution_clock(高精度时钟)，_Dur则代表从时钟开始到现在度过的时间段类型，可以用秒、分、时等等精度表示
    std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes> minute_time_point;

    // 打印time_t类型的当前时间，默认精度是秒
    std::time_t timep;
    time(&timep);
    std::cout << timep << std::endl;

    // 使用system_clock获取当前时间点time_point，默认精度是纳秒
    std::cout << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;

    // 两种方式睡1s
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));

    // 打印time_t类型的当前时间，默认精度是秒
    time(&timep);
    std::cout << timep << std::endl;

    // 使用system_clock获取当前时间点time_point，默认精度是纳秒
    std::cout << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;

    // 将time_t类型转换成time_point时间
    std::cout << std::chrono::system_clock::from_time_t(timep).time_since_epoch().count() << std::endl;

    // 将time_point类型转换成time_t类型
    timep = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << timep << std::endl;

    // 不同精度时间段的转换，截断方式而非四舍五入
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds> (std::chrono::milliseconds(1234));
    std::cout << seconds.count() << std::endl;
}
