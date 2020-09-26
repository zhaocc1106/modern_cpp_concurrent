/**
 * 使用内存屏障排序两个线程之间的非原子操作
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <cassert>

static bool x = false; // 待排序的非原子变量
static std::atomic<bool> y;
static std::atomic<int> z;

void thread1_func() {
    x = true;
    std::atomic_thread_fence(std::memory_order_release); // 使用内存屏障防止x和y排序
    y.store(true, std::memory_order_relaxed); // 因为使用了内存屏障，所以这里可以使用memory_order_relaxed
}

void thread2_func() {
    while (!y.load(std::memory_order_relaxed)); // 如果原子变量y未被置成true，一直忙等，因为使用了内存屏障，所以这里可以使用memory_order_relaxed
    std::atomic_thread_fence(std::memory_order_acquire); // 和thread1 release构成一对
    if (x) {
        ++z;
    }
}

int main() {
    x = false;
    y = false;
    z = 0;

    std::thread a(thread1_func);
    std::thread b(thread2_func);

    a.join();
    b.join();

    assert(z.load() != 0); // 该断言永远不会触发
}