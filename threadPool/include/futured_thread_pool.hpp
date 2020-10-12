/**
 * 基于simple_thread_pool开发的可以等待任务结果的线程池。
 */

#ifndef THREADPOOL_FUTURED_THREAD_POOL_HPP
#define THREADPOOL_FUTURED_THREAD_POOL_HPP

#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <future>

#include "thread_safe_queue.hpp"
#include "threads_joiner.hpp"

namespace zhaocc {
    class FuturedThreadPool {
    private:
        /* 对任意类型的可调用对象进行封装，只支持move，不支持copy，主要用于对packaged_task封装 */
        class FunctionWrapper {
            /* 封装可调用对象基类 */
            class ImplBase {
            public:
                virtual void call() = 0; // 纯虚函数
                virtual ~ImplBase() {} // 定义基类析构为虚函数防止内存泄露
            };

            std::unique_ptr<ImplBase> impl;

            /* 封装任意类型的可调用对象 */
            template<typename F>
            class ImplType : public ImplBase {
            private:
                F f; // 保存可调用对象
            public:
                ImplType(F&& f_) : f(std::move(f_)) {} // 使用万能引用来接受任意参数

                void call() {
                    f();
                }
            };

        public:
            template<typename F>
            FunctionWrapper(F&& f) : impl(new ImplType<F>(std::move(f))) {} // 万能引用接受任意参数

            void operator()() { // 定义为函数对象类型
                impl->call();
            }

            FunctionWrapper() = default; // 默认的构造函数

            FunctionWrapper(FunctionWrapper&& other) noexcept { // 移动构造函数
                impl = std::move(other.impl);
            }

            FunctionWrapper& operator=(FunctionWrapper&& other) noexcept { // 移动复制函数
                impl = std::move(other.impl);
                return *this;
            }

            // 禁止拷贝
            FunctionWrapper(const FunctionWrapper&) = delete;
            FunctionWrapper& operator=(const FunctionWrapper&) = delete;
        };

        std::atomic<bool> done; // 线程池所有任务是否将结束
        zhaocc::ThreadSafeQueue<FunctionWrapper> work_queue; // 任务队列
        std::vector<std::thread> threads; // 所有工作线程
        zhaocc::ThreadsJoiner threads_joiner; // threads joiner，帮助在线程池析构时能够等待所有线程工作结束，必须放到threads后面，这样析构的时候先析构它

        void worker_thread_func(); // 工作线程执行的函数

    public:
        FuturedThreadPool(); // 构造函数
        ~FuturedThreadPool(); // 析构函数

        template<typename FuncType>
        std::future<typename std::result_of<FuncType()>::type> submit(FuncType&& f); // 提交任务
    };

    void FuturedThreadPool::worker_thread_func() {
        while (!done) {
            FunctionWrapper task;

            if (work_queue.try_pop(task)) {
                task(); // 当前有任务直接执行
            } else {
                std::this_thread::yield(); // 当前无任务则调度出去
            }

            // work_queue.wait_and_pop(task); // 一直阻塞等待到有任务
            // task(); // 执行任务
        }
    }

    FuturedThreadPool::FuturedThreadPool() : done(false),
                                             threads_joiner(threads) { // 将threads交付给threads_joiner管理，在线程池任务结束时等待所有线程
        unsigned const concurrent_count = std::thread::hardware_concurrency(); // 获取硬件支持的并发数
        try {
            for (unsigned i = 0; i < concurrent_count; i++) {
                threads.emplace_back(&FuturedThreadPool::worker_thread_func, this); // 创建工作线程
            }
        } catch (...) {
            done = true;
            throw;
        }
    }

    FuturedThreadPool::~FuturedThreadPool() {
        done = true;
    }

    template<typename FuncType>
    std::future<typename std::result_of<FuncType()>::type> FuturedThreadPool::submit(FuncType&& f) { // 万能引用
        using res_type = typename std::result_of<FuncType()>::type;

        std::packaged_task<res_type()> task(std::forward<FuncType>(f)); // 完美转移
        std::future<res_type> future_res(task.get_future());

        work_queue.push(std::move(task));
        return future_res;
    }
}


#endif //THREADPOOL_FUTURED_THREAD_POOL_HPP
