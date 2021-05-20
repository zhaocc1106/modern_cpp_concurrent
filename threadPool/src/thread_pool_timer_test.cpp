//
// Created by zhaochaochao on 2021/5/17.
//

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>

#include "thread_pool_timer.h"

static std::string GenerateTimeStr() {
  /*Generate time str such as 2017-08-05 09:22:55.726*/
  timeval tv{};
  gettimeofday(&tv, nullptr);
  struct tm* tm_now = localtime(&tv.tv_sec);
  char time_str[30];
  memset(time_str, 0, sizeof(time_str));
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_now);
  snprintf(time_str + 19, 5, ".%lu", (unsigned long) round(tv.tv_usec / 1000.0));
  return std::string(time_str);
}

static void Func1(void *args) {
  std::cout << __func__ << " thread_id: " << std::this_thread::get_id() << ", current time: "
            << GenerateTimeStr()
            << std::endl;
}

static void Func2(void *args) {
  std::cout << __func__ << " thread_id: " << std::this_thread::get_id() << ", current time: "
            << GenerateTimeStr()
            << std::endl;
}

static void Func3(void *args) {
  std::cout << __func__ << " thread_id: " << std::this_thread::get_id() << ", current time: "
            << GenerateTimeStr()
            << std::endl;
}

static void Func4(void *args) {
  std::cout << __func__ << " thread_id: " << std::this_thread::get_id() << ", current time: "
            << GenerateTimeStr()
            << std::endl;
}

class MyClass {
 public:
  void Func5(void *args) {
    std::cout << __func__ << " thread_id: " << std::this_thread::get_id() << ", current time: "
              << GenerateTimeStr()
              << std::endl;
  }
};

int main(int argc, char *argv[]) {
  ThreadPoolTimerContainer thread_pool_timer_container(4);

  thread_pool_timer_container.AddTimer(Func1, nullptr, 1000, true); // Timer should be started firstly.

  std::thread th1([&]() {
    thread_pool_timer_container.Start();
    int64_t timer_id = thread_pool_timer_container.AddTimer(Func1, nullptr, 1000, true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    thread_pool_timer_container.CancelTimer(timer_id);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    thread_pool_timer_container.Stop();
  });

  std::thread th2([&]() {
    thread_pool_timer_container.Start();
    int64_t timer_id = thread_pool_timer_container.AddTimer(Func2, nullptr, 1000, true);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    thread_pool_timer_container.Stop();
  });

  std::thread th3([&]() {
    thread_pool_timer_container.Start();
    int64_t timer_id = thread_pool_timer_container.AddTimer(Func3, nullptr, 1000, true);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    thread_pool_timer_container.CancelTimer(timer_id);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    thread_pool_timer_container.Stop();
  });

  std::thread th4([&]() {
    thread_pool_timer_container.Start();
    int64_t timer_id = thread_pool_timer_container.AddTimer(Func4, nullptr, 1000, true);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    thread_pool_timer_container.Stop();
  });

  std::thread th5([&]() {
    thread_pool_timer_container.Start();
    MyClass c;
    int64_t timer_id =
        thread_pool_timer_container.AddTimer(std::bind(&MyClass::Func5, &c, std::placeholders::_1),
                                             nullptr,
                                             1000,
                                             false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    thread_pool_timer_container.CancelTimer(timer_id);
    std::this_thread::sleep_for(std::chrono::seconds(6));
    thread_pool_timer_container.Stop();
  });

  th1.join();
  th2.join();
  th3.join();
  th4.join();
  th5.join();

  return 0;
}