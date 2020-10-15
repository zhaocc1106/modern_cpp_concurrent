/**
 * 使用futured_thread_pool实现一个并行快速排序
 */

#ifndef THREADPOOL_PARALLEL_QUICK_SORT_HPP
#define THREADPOOL_PARALLEL_QUICK_SORT_HPP

#include <list>
#include <algorithm>
#include <iostream>

#include "futured_thread_pool.hpp"

namespace zhaocc {

    template<typename T, typename ThreadPoolType>
    class ParallelQuickSort {
    private:
        ThreadPoolType thread_pool; // 线程池类型作为模板参数传入

    public:

        /**
         * 构造函数
         * @param concurrent_count 线程池中并发线程数量
         */
        explicit ParallelQuickSort(unsigned concurrent_count = std::thread::hardware_concurrency()) : thread_pool(
                concurrent_count) {};

        std::list<T> do_sort(std::list<T>& chunk_data);
    };

    template<typename T, typename ThreadPoolType>
    std::list<T> ParallelQuickSort<T, ThreadPoolType>::do_sort(std::list<T>& chunk_data) {
        if (chunk_data.empty()) {
            return chunk_data;
        }

        std::list<T> result;
        result.splice(result.begin(), chunk_data, chunk_data.begin());
        T const& pivot = *result.begin(); // 找到中间值

        // 小于中间值放到一块，不小于中间值放到一块
        auto divide_point = std::partition(chunk_data.begin(), chunk_data.end(), [&](T const& t) { return t < pivot; });

        std::list<T> lower_part;
        lower_part.splice(lower_part.end(), chunk_data, chunk_data.begin(), divide_point);

        // 递归排序小区域部分，并且放到线程池中来做
        std::future<std::list<T>> sorted_lower_future = thread_pool.submit(
                std::bind(&ParallelQuickSort<T, ThreadPoolType>::do_sort, this, std::move(lower_part)) // 使用bind函数绑定局部参数
        );

        // 递归排序大区域部分，在本线程中作
        auto sorted_higher(do_sort(chunk_data));
        result.splice(result.end(), sorted_higher);

        while (sorted_lower_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
            thread_pool.run_pending_task(); // 在本线程执行线程池中阻塞的任务，防止死锁发生
        }

        result.splice(result.begin(), sorted_lower_future.get()); // 等待异步线程执行完毕

        std::cout << "[thread-" << std::this_thread::get_id() << "] sub sort task done." << std::endl;
        return result;
    }
}

#endif //THREADPOOL_PARALLEL_QUICK_SORT_HPP
