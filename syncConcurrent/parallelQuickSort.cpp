/**
 * FP(Functional Programming)风格实现一个并行快速排序
 */

#include <iostream>
#include <list>
#include <future>
#include <algorithm>

template<typename F, typename A>
std::future<typename std::result_of<F(A &&)>::type> spawn_task(F &&f, A &&a) { // 参数均使用万能引用
    using result_type = typename std::result_of<F(A &&)>::type;
    std::packaged_task<result_type(A &&)> task(std::forward<F>(f)); // 使用完美转发转移参数
    std::future<result_type> res(task.get_future());
    std::thread t(std::move(task), std::forward<A>(a)); // 使用完美转发转移参数
    t.detach();
    return res;
}

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const &pivot = *result.begin(); // 找到中间值

    // 小于中间值放到一块，不小于中间值放到一块
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const &t) { return t < pivot; });

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    // 递归排序小区域部分，并且放到单独线程中来做
    // std::future<std::list<T>> sorted_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));
    std::future<std::list<T>> sorted_lower_future = spawn_task(&parallel_quick_sort<T>, std::move(lower_part));

    // 递归排序大区域部分，在本线程中作
    auto sorted_higher(parallel_quick_sort(std::move(input)));

    result.splice(result.end(), sorted_higher);
    result.splice(result.begin(), sorted_lower_future.get()); // 等待异步线程执行完毕

    return result;
}

int main() {
    std::list<int> nums = {2, 3, 1, 5, 4, 2};
    std::list<int> res = parallel_quick_sort(nums);
    for (auto &num : res) {
        std::cout << num << ", ";
    }
}
