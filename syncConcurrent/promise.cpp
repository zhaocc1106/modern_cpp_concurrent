/**
 * promise/future对的用法
 */

#include <iostream>
#include <thread>
#include <future>

void thread1_func(std::promise<int> &&promise) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    try {
        // throw std::logic_error("logic error"); // promise制造一个异常

        // promise设置值时，对应的future就会获取到值。
        // 如果这句注释掉的话，会在future中保存一份异常。
        // 因为promise是右值引用，所以promise生命周期在该函数中会结束，如果promise被析构时仍然没有对future赋值，就会触发异常。
        promise.set_value(100);
    } catch (std::exception err) {
        promise.set_exception(std::current_exception()); // promise设置当前的异常内容
    }
}

int main() {
    std::promise<int> prom;
    std::future<int> res_future = prom.get_future(); // 构成promise-future对
    std::thread t1(thread1_func, std::move(prom));
    t1.detach();
    try {
        res_future.wait();
        std::cout << "res_future: " << res_future.get() << std::endl; // 如果promise引发异常，在get时能够捕获异常
    } catch (std::exception err) {
        std::cout << "res_future has exception: " << err.what() << std::endl;
    }
}