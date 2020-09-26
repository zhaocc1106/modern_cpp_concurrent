/**
 * 使用原子变量+releaxedOrdering内存模型实现一个多线程counter
 */

#include <iostream>
#include <cassert>
#include <thread>
#include <atomic>
#include <vector>

using namespace std;

atomic<int> counter(0); // 计数完肯定是1000000
// volatile static int counter = 0; // 计数完可能不是1000000

void func() {
    for (int i = 0; i < 1000; i++) {
        counter.fetch_add(1, memory_order_relaxed); // memory_order_relaxed保证原子操作
        // counter++;
    }
}

int main() {
    while (true) {
        thread ths[1000];
        for (auto &t : ths) {
            t = thread(func);
        }

        for (auto &t : ths) {
            t.join();
        }

        cout << counter << endl;
        assert(counter == 1000000);
        counter.store(0);
        // counter = 0;
    }
}
