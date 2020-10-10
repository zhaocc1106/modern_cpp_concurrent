/**
 * 测试使用
 */

#include <iostream>

#include "thread_safe_queue.hpp"

int main() {
    zhaocc::ThreadSafeQueue<int> thread_safe_queue;
    thread_safe_queue.push(10);
    std::cout << *thread_safe_queue.try_pop() << std::endl;
}
