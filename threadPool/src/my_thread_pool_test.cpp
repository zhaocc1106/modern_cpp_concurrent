/**
 * 测试基于c11手写的thread pool
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>

#include "thread_safe_queue.hpp"
#include "simple_thread_pool.hpp"
#include "futured_thread_pool.hpp"
#include "parallel_quick_sort.hpp"
#include "multi_queue_thread_pool.hpp"

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

/* 测试futured thread pool */
void test_futured_thread_pool() {
    zhaocc::FuturedThreadPool thread_pool;
    unsigned task_count = std::thread::hardware_concurrency() + 5;

    std::vector<std::future<int>> futures;
    futures.reserve(task_count);
    for (int i = 0; i < task_count; i++) { // 有concurrency个任务被立即执行，剩余的5个延时1s有工作线程空闲下来才会执行
        futures.emplace_back(
                thread_pool.submit([i]() -> int {
                    std::cout << "[thread " << std::this_thread::get_id() << "] i: " << i << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return i;
                })
        );
    }

    // future阻塞等待所有任务完成
    for (auto& future : futures) {
        int ret = future.get();
        std::cout << "future return: " << ret << std::endl;
    }
}

/* 测试并行quick sort */
void test_parallel_quick_sort() {
    zhaocc::ParallelQuickSort<int, zhaocc::FuturedThreadPool> parallel_quick_sort(
            2); // 测试 2(线程池中并发线程数量) + 1(本线程) 个线程来并发排序
    std::list<int> arr{10, 11, 12, 4, 6, 2, 1, 3, 4, 6, 7, 9, 0};
    std::list<int> res = parallel_quick_sort.do_sort(arr);
    for (int& i : res) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

/* 测试多任务队列线程池 */
void test_multi_queue_thread_pool() {
    /* 所有的工作线程从线程池主任务队列获取任务 */
    zhaocc::MultiQueueThreadPool thread_pool(2);

    std::vector<std::future<int>> futures;
    unsigned task_count = 5;
    futures.reserve(task_count);
    for (int i = 0; i < task_count; i++) {
        futures.emplace_back(
                thread_pool.submit([i]() -> int { // 因为所有任务都是从非工作线程提交的，所以线程池只有主任务队列有任务，两个工作线程都从线程池的主任务队列中获取任务
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    return i;
                })
        );
    }

    // future阻塞等待所有任务完成
    for (auto& future : futures) {
        int ret = future.get();
        std::cout << "future return: " << ret << std::endl;
    }

    std::cout << std::endl << std::endl;

    /* 并行快排使用多任务队列线程池来并发，可以看到工作线程即有“从主任务中获取任务”，又有“从当前工作线程任务队列中获取任务”，又有“从其他工作线程的任务队列中窃取任务” */
    zhaocc::ParallelQuickSort<int, zhaocc::MultiQueueThreadPool> parallel_quick_sort(
            3); // 测试 3(线程池中并发线程数量) + 1(本线程) 个线程来并发排序
    std::list<int> arr{10, 11, 12, 4, 6, 2, 1, 3, 4, 6, 7, 9, 0, 10, 11, 12, 4, 6, 2, 1, 3, 4, 6, 7, 9, 0, 10, 11, 12,
                       4, 6, 2, 1, 3, 4, 6, 7, 9, 0, 10, 11, 12, 4, 6, 2, 1, 3, 4, 6, 7, 9, 0};
    std::list<int> res = parallel_quick_sort.do_sort(arr);
    for (int& i : res) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

int main() {
    test_thread_safe_queue();
    test_destruction_order();
    test_simple_thread_pool();
    test_futured_thread_pool();
    test_parallel_quick_sort();
    test_multi_queue_thread_pool();
}
