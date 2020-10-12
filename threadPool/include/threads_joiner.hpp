/**
 * 使用RAII编程实现一个守护vector<thread>一组线程的joiner，当析构时能够自动join所有线程
 */

#ifndef THREADPOOL_THREADS_JOINER_HPP
#define THREADPOOL_THREADS_JOINER_HPP

#include <vector>
#include <thread>

namespace zhaocc {
    class ThreadsJoiner {
    private:
        std::vector<std::thread>& threads; // 引用一个线程容器
    public:
        ThreadsJoiner(std::vector<std::thread>& threads_) : threads(threads_) {}

        ~ThreadsJoiner() {
            for (auto& th : threads) {
                if (th.joinable()) {
                    th.join();
                }
            }
        }
    };
}


#endif //THREADPOOL_THREADS_JOINER_HPP
