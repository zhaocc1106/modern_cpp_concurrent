/**
 * 顺序一致性次序
 */

#include <thread>
#include <atomic>
#include <cassert>
#include <iostream>

std::atomic<bool> x = {false};
std::atomic<bool> y = {false};
std::atomic<int> z = {0};

void write_x()
{
    x.store(true, std::memory_order_seq_cst);
}

void write_y()
{
    y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y()
{
    while (!x.load(std::memory_order_seq_cst))
        ;
    if (y.load(std::memory_order_seq_cst)) {
        ++z;
    }
}

void read_y_then_x()
{
    while (!y.load(std::memory_order_seq_cst))
        ;
    if (x.load(std::memory_order_seq_cst)) {
        ++z;
    }
}

int main()
{
    for (int i = 0; i < 10000; i++) {
        std::thread a(write_x);
        std::thread b(write_y);
        std::thread c(read_x_then_y);
        std::thread d(read_y_then_x);
        a.join();
        b.join();
        c.join();
        d.join();
        // 如果不使用顺序一致模型的话，则此处就可能触发断言
        std::cout << z << std::endl;
        assert(z.load() != 0);
        x = false;
        y = false;
        z = 0;
    }
}