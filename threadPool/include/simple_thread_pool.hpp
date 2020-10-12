/**
 * 实现一个简单的线程池，固定多个工作线程一直在工作，进来任务会被分配给某一个工作线程给执行。
 */

#ifndef THREADPOOL_SIMPLE_THREAD_POOL_HPP
#define THREADPOOL_SIMPLE_THREAD_POOL_HPP

#include <thread>
#include <atomic>
#include <functional>
#include <vector>

#include "thread_safe_queue.hpp"
#include "threads_joiner.hpp"

namespace zhaocc {
    class SimpleThreadPool {
    private:
        std::atomic<bool> done; // 线程池所有任务是否将结束
        zhaocc::ThreadSafeQueue<std::function<void()>> work_queue; // 任务队列
        std::vector<std::thread> threads; // 所有工作线程
        zhaocc::ThreadsJoiner threads_joiner; // threads joiner，帮助在线程池析构时能够等待所有线程工作结束，必须放到threads后面，这样析构的时候先析构它

        void worker_thread_func(); // 工作线程执行的函数

    public:
        SimpleThreadPool(); // 构造函数
        ~SimpleThreadPool(); // 析构函数

        template<typename FuncType>
        void submit(FuncType f); // 提交任务
    };

    void SimpleThreadPool::worker_thread_func() {
        while (!done) {
            std::function<void()> task;

            if (work_queue.try_pop(task)) {
                task(); // 当前有任务直接执行
            } else {
                std::this_thread::yield(); // 当前无任务则调度出去
            }

            // work_queue.wait_and_pop(task); // 一直阻塞等待到有任务
            // task(); // 执行任务
        }
    }

    SimpleThreadPool::SimpleThreadPool() : done(false),
                                           threads_joiner(threads) { // 将threads交付给threads_joiner管理，在线程池任务结束时等待所有线程
        unsigned const concurrent_count = std::thread::hardware_concurrency(); // 获取硬件支持的并发数
        try {
            for (unsigned i = 0; i < concurrent_count; i++) {
                threads.emplace_back(&SimpleThreadPool::worker_thread_func, this); // 创建工作线程
            }
        } catch (...) {
            done = true;
            throw;
        }
    }

    SimpleThreadPool::~SimpleThreadPool() {
        done = true;
    }

    template<typename FuncType>
    void SimpleThreadPool::submit(FuncType f) {
        work_queue.push(std::function<void()>(f));
    }
}


#endif //THREADPOOL_SIMPLE_THREAD_POOL_HPP
