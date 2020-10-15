/**
 * 每个工作线程都有一个自己的“任务队列”的并且支持“任务窃取”的线程池，能够使得工作线程的并发性更高。
 */

#ifndef THREADPOOL_MULTI_QUEUE_THREAD_POOL_HPP
#define THREADPOOL_MULTI_QUEUE_THREAD_POOL_HPP

#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include <vector>
#include <future>

#include "thread_safe_queue.hpp"
#include "threads_joiner.hpp"

namespace zhaocc {
    class MultiQueueThreadPool {
    private:
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
        ThreadSafeQueue <FunctionWrapper> main_work_queue; // 主任务队列，用于所有工作线程公用
        std::vector<std::unique_ptr<ThreadSafeQueue < FunctionWrapper>>>
        sub_work_queues; // 子任务队列，对于每一个工作线程都有一个单独的任务队列
        std::vector<std::thread> threads; // 所有工作线程
        ThreadsJoiner threads_joiner; // threads joiner，帮助在线程池析构时能够等待所有线程工作结束，必须放到threads后面，这样析构的时候先析构它

        static thread_local ThreadSafeQueue <FunctionWrapper>* local_work_queue; // 当前工作线程的任务队列指针
        static thread_local unsigned my_index; // 当前工作线程的任务队列在sub_work_queues中的位置

        void worker_thread_func(unsigned my_index_); // 工作线程执行的函数

        bool pop_task_from_local_queue(FunctionWrapper& task); // 从工作线程的任务队列中获取任务
        bool pop_task_from_main_queue(FunctionWrapper& task); // 从线程池的主任务队列中获取任务
        bool pop_task_from_other_thread_queue(FunctionWrapper& task); // 从其他工作线程的任务队列中窃取任务

    public:

        /**
         * 构造函数
         * @param concurrent_count: 线程池中并发线程数量
         */
        explicit MultiQueueThreadPool(unsigned concurrent_count = std::thread::hardware_concurrency());

        ~MultiQueueThreadPool(); // 析构函数

        /**
         * 提交任务
         * @tparam FuncType: 函数类型
         * @param f: 可调用对象
         * @return 与可调用对象返回值相关联的future
         */
        template<typename FuncType>
        std::future<typename std::result_of<FuncType()>::type> submit(FuncType&& f);

        /**
         * 提供一个接口可以在调用者线程上执行任务
         */
        void run_pending_task();
    };

    thread_local ThreadSafeQueue <MultiQueueThreadPool::FunctionWrapper>* MultiQueueThreadPool::local_work_queue = nullptr;
    thread_local unsigned MultiQueueThreadPool::my_index = 0;

    void MultiQueueThreadPool::worker_thread_func(unsigned my_index_) {
        // 当前工作线程获取对应的任务队列
        my_index = my_index_;
        local_work_queue = sub_work_queues[my_index_].get();

        while (!done) {
            run_pending_task();
        }
    }

    MultiQueueThreadPool::MultiQueueThreadPool(unsigned concurrent_count)
            : done(false), threads_joiner(threads) { // 将threads交付给threads_joiner管理，在线程池任务结束时等待所有线程
        try {
            for (unsigned i = 0; i < concurrent_count; i++) {
                sub_work_queues.emplace_back(
                        std::make_unique<ThreadSafeQueue<FunctionWrapper>>()); // 每个工作线程都对应一个工作队列
                threads.emplace_back(std::thread(&MultiQueueThreadPool::worker_thread_func, this, i)); // 创建工作线程
            }
        } catch (...) {
            done = true;
            throw;
        }
    }

    MultiQueueThreadPool::~MultiQueueThreadPool() {
        done = true;
    }

    template<typename FuncType>
    std::future<typename std::result_of<FuncType()>::type> MultiQueueThreadPool::submit(FuncType&& f) { // 万能引用
        using res_type = typename std::result_of<FuncType()>::type;

        std::packaged_task<res_type()> task(std::forward<FuncType>(f)); // 完美转移
        std::future<res_type> future_res(task.get_future());

        if (local_work_queue) { // 如果当前线程有工作队列，将任务放到本线程的工作队列中
            local_work_queue->push(std::move(task));
        } else {
            main_work_queue.push(std::move(task));
        }
        return future_res;
    }

    bool MultiQueueThreadPool::pop_task_from_local_queue(FunctionWrapper& task) {
        return local_work_queue && local_work_queue->try_pop(task);
    }

    bool MultiQueueThreadPool::pop_task_from_main_queue(FunctionWrapper& task) {
        return main_work_queue.try_pop(task);
    }

    bool MultiQueueThreadPool::pop_task_from_other_thread_queue(FunctionWrapper& task) {
        for (unsigned i = 0; i < sub_work_queues.size(); i++) { // 尝试从每一个其他工作线程中窃取任务
            unsigned const ind = (my_index + i + 1) % sub_work_queues.size();
            if (sub_work_queues[ind]->try_pop(task)) {
                std::cout << "[thread-" << std::this_thread::get_id() << " index-" << my_index
                          << "] got task from other task queue with index " << ind << "." << std::endl;
                return true;
            }
        }
        return false;
    }

    void MultiQueueThreadPool::run_pending_task() {
        FunctionWrapper task;

        if (pop_task_from_local_queue(task)) {
            std::cout << "[thread-" << std::this_thread::get_id() << " index-" << my_index
                      << "] got task from local queue." << std::endl;
            task(); // 当前有任务直接执行
        } else if (pop_task_from_main_queue(task)) {
            std::cout << "[thread-" << std::this_thread::get_id() << " index-" << my_index
                      << "] got task from main queue." << std::endl;
            task();
        } else if (pop_task_from_other_thread_queue(task)) {
            task();
        } else {
            std::this_thread::yield(); // 当前无任务则调度出去
        }
    }
};

#endif //THREADPOOL_MULTI_QUEUE_THREAD_POOL_HPP
