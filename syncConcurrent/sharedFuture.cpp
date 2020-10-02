/**
 * future是可移动不可复制对象，并且get动作只能执行一次。
 * 使用shared_future实现future被拷贝多份，多个线程可以同步等待一个线程的一个future。
 */

#include <iostream>
#include <thread>
#include <future>

void wait_thread_func(std::shared_future<int> local_future) { // 强行让shared_future拷贝一份，这样每个线程都有一个单独的shared_future，防止同时访问一个shared_future出现数据竞争
    std::cout << &local_future << std::endl; // 每个线程的shared_future不是同一个

    std::future_status status = local_future.wait_for(std::chrono::milliseconds(1000)); // 设置超时等待时间
    std::cout << "future status: " << (int) status << std::endl;

    std::cout << "tid [" << std::this_thread::get_id() << "] res: " << local_future.get() << std::endl;
}

int main() {
    std::promise<int> prom;
    std::shared_future<int> sf = prom.get_future().share();
    std::cout << &sf << std::endl;

    std::thread t1(wait_thread_func, sf);
    std::thread t2(wait_thread_func, sf);
    std::thread t3(wait_thread_func, sf);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    prom.set_value(100);

    t1.join();
    t2.join();
    t3.join();
}

