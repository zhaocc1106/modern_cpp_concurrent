/**
 * 虚假唤醒
 */

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cassert>

static std::mutex m;
static std::condition_variable cv;
static int flag = 0;

void func1() {
    std::cout << "func1 begin" << std::endl;
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]() {
        if (flag != 1) {
            if (flag != 1) {
                std::cout << "func1 spurious wake" << std::endl; // 该log可能看到两次，第二次是虚假唤醒
            }
            return false;
        } else {
            return true;
        }
    });
    std::cout << "func1 wait end with flag: " << flag << std::endl;
    assert(flag == 1);
    flag = 2; // 故意制造一个虚假唤醒
}

void func2() {
    std::cout << "func2 begin" << std::endl;
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]() {
        if (flag != 1) {
            if (flag != 1) {
                std::cout << "func2 spurious wake" << std::endl;  // 该log可能看到两次，第二次是虚假唤醒
            }
            return false;
        } else {
            return true;
        }
    });
    std::cout << "func2 wait end with flag: " << flag << std::endl;
    assert(flag == 1);
    flag = 2; // 故意制造一个虚假唤醒
}


int main() {
    flag = 0;

    std::thread t1(func1);
    std::thread t2(func2);

    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::unique_lock<std::mutex> lock(m);
        flag = 1;
        cv.notify_all(); // 测试notify_one触发不了虚假唤醒？
    }

    t1.join();
    t2.join();
}