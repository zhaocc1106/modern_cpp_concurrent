/**
 * 使用atomic_flag生成一个自旋锁类
 */

#include <iostream>
#include <atomic>
#include <thread>

class MyLock {
private:
    std::atomic_flag atomicFlag;
public:
    MyLock();

    void lock();

    void unlock();
};

MyLock::MyLock() {
    atomicFlag.clear();
}

void MyLock::lock() {
    while (atomicFlag.test_and_set(std::memory_order_acquire)) // 如果flag为false，则代表当前没有人占用这个锁，则flag置为true。如果flag为true，则代表当前有人占用锁，则只能一直忙等
        std::this_thread::yield();
}

void MyLock::unlock() {
    atomicFlag.clear(std::memory_order_release); // 置为false
}