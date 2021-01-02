/**
 * 使用future-async实现异步等待
 */

#include <future>
#include <iostream>

int func(std::string &str) {
    std::cout << "[" << std::this_thread::get_id() << "]--func str: " << str << std::endl;
    str = "hi future";
    return 100;
}

int main() {
    std::string str("hello future");
    // 创建异步动作，默认policy为launch::async|launch::deferred，async代表func在单独线程运行，deferred代表func在显示调用get或wait时才会跑
    std::future<int> res = std::async(func, std::ref(str));

    // 如果wait_for，会使得future async被执行，这里status为std::future_status::ready，相应的下一行代码会输出str转换后的值
    std::cout << "status: " << (int) res.wait_for(std::chrono::milliseconds(100)) << std::endl;

    std::cout << str << std::endl; // 这时异步动作已完成，str为修改后的值
    std::cout << res.get() << "--[" << std::this_thread::get_id() << "]--" << str << std::endl;

    std::cout << std::endl;


    std::string str2("hello future");
    // 创建异步动作，policy为launch::deferred，deferred代表func在显示调用get或wait时才会跑，但是都是在同一个线程跑，如thread::get_id打印
    std::future<int> res2 = std::async(std::launch::deferred, func, std::ref(str2));

    // 如果wait_for，future async因为是在同一个线程执行，所以无法被执行
    // std::cout << "status: " << (int) res.wait_for(std::chrono::milliseconds(100)) << std::endl;

    std::cout << str2 << std::endl; // 这时异步动作没跑，所以str为原来的值
    std::cout << res2.get() << "--[" << std::this_thread::get_id() << "]--" << str2 << std::endl; // get动作使得当前线程等待异步动作结束，所以str值被修改

    std::cout << std::endl;


    std::string str3("hello future");
    // 创建异步动作，policy为launch::async，async代表func在单独线程运行，但是func会被立即执行
    std::future<int> res3 = std::async(std::launch::async, func, std::ref(str3));
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 当前线程主动等待1s
    std::cout << str3 << std::endl; // 这时异步动作已经跑过了，所以str为修改后的值
    std::cout << res3.get() << "--[" << std::this_thread::get_id() << "]--" << str3 << std::endl;
}
