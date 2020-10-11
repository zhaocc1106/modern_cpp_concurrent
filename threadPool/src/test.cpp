/**
 * 测试使用
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

#include "thread_safe_queue.hpp"

void test_thread_safe_queue() {
    zhaocc::ThreadSafeQueue<int> thread_safe_queue;

    std::thread thread1([&] {
        while (true) {
            thread_safe_queue.push(10);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    auto get_func = [&] {
        while (true) {
            int value = *(thread_safe_queue.wait_and_pop());
            assert(value == 10); // wait pop返回值必定不为0
            std::cout << "[thread " << std::this_thread::get_id() << "] wait_and_pop: " << value << std::endl;
            thread_safe_queue.wait_and_pop(value);
            assert(value == 10); // wait pop返回值必定不为0
            std::cout << "[thread " << std::this_thread::get_id() << "] wait_and_pop(reference): " << value
                      << std::endl;
            std::shared_ptr<int> value_ptr = thread_safe_queue.try_pop();
            value = value_ptr != nullptr ? *value_ptr : 0;
            assert(value == 10 || value == 0); // try pop返回值可能为0
            std::cout << "[thread " << std::this_thread::get_id() << "] try_pop: " << value << std::endl;
            value = 0;
            thread_safe_queue.try_pop(value);
            assert(value == 10 || value == 0); // try pop返回值可能为0
            std::cout << "[thread " << std::this_thread::get_id() << "] try_pop(reference): " << value
                      << std::endl;
        }
    };

    std::thread thread2(get_func);
    std::thread thread3(get_func);
    std::thread thread4(get_func);

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
}

int main() {
    test_thread_safe_queue();
}
