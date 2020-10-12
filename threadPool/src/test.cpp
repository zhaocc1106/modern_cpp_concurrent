/**
 * 测试使用
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

#include "thread_safe_queue.hpp"
#include "simple_thread_pool.hpp"

/* 测试线程安全队列 */
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

/* 测试线程安全队列中存放的对象的析构顺序 */
void test_destruction_order() {
    class TestClass {
    private:
        int val;
    public:
        TestClass() : val(0) {
        }

        explicit TestClass(int val_) : val(val_) {
            std::cout << "TestClass(int)" << std::endl;
        }

        TestClass(const TestClass& other) {
            std::cout << "TestClass(const TestClass&)" << std::endl;
            val = other.val;
        }

        TestClass(TestClass&& other) noexcept {
            std::cout << "TestClass(TestClass&&)" << std::endl;
            val = other.val;
        }

        ~TestClass() {
            std::cout << "~TestClass() val: " << val << std::endl;
        }
    };

    zhaocc::ThreadSafeQueue<TestClass> thread_safe_queue;
    thread_safe_queue.push(TestClass(1)); // 调用了一次构造函数构造临时对象，一次移动构造函数构造shared_ptr指向的对象，一次析构函数析构掉临时对象
    std::cout << std::endl;
    thread_safe_queue.push(TestClass(2)); // 调用了一次构造函数构造临时对象，一次移动构造函数构造shared_ptr指向的对象，一次析构函数析构掉临时对象
    std::cout << std::endl;
    thread_safe_queue.push(TestClass(3)); // 调用了一次构造函数构造临时对象，一次移动构造函数构造shared_ptr指向的对象，一次析构函数析构掉临时对象
    std::cout << std::endl;

    // 析构顺序为 ~TestClass() val: 3， ~TestClass() val: 2， ~TestClass() val: 1。
    // 在析构unique_ptr<Node>时会先析构对象的next属性，所以析构的顺序和队列存放的顺序是逆的。
}

/* 测试简单的线程池 */
void test_simple_thread_pool() {
    zhaocc::SimpleThreadPool thread_pool;

    for (int i = 0; i < std::thread::hardware_concurrency() + 5; i++) { // 有concurrency个任务被立即执行，剩余的5个延时1s有工作线程空闲下来才会执行
        thread_pool.submit([i]() {
            std::cout << "[thread " << std::this_thread::get_id() << "] i: " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}

int main() {
    // test_thread_safe_queue();
    // test_destruction_order();
    test_simple_thread_pool();
}
