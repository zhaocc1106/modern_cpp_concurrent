/**
 * 测试boost中常用的thread pool
 */

#include <iostream>
#include <boost/asio.hpp>

void task1() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "[thread-" << std::this_thread::get_id() << "] func1 - "
              << std::chrono::duration_cast<std::chrono::seconds>(
                      std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
}

void task2() {
    std::cout << "[thread-" << std::this_thread::get_id() << "] func2" << std::endl;
}

int task3() {
    std::cout << "[thread-" << std::this_thread::get_id() << "] func3" << std::endl;
    return 0;
}

/* boost::asio::thread_pool是一个简单的固定大小的线程池，不支持wait？ */
void test_asio_thread_pool() {
    boost::asio::thread_pool pool(4);
    boost::asio::post(pool, task1);
    boost::asio::dispatch(pool, task2);
    boost::asio::execution::execute(pool.get_executor(), task3);
    pool.join();
}

void task_handler(boost::system::error_code error_code) {
    std::cout << "[thread-" << std::this_thread::get_id() << "] task_handler error_code: " << error_code << std::endl;
}

/* 使用deadline timer实现线程池中执行定时任务 */
void test_timer_task() {
    boost::asio::thread_pool pool(1);
    boost::asio::dispatch(pool, task2);
    boost::asio::deadline_timer timer(pool.get_executor(), boost::posix_time::seconds(1));
    timer.async_wait(task_handler);
    pool.join();
}

int main() {
    test_asio_thread_pool();
    test_timer_task();
}
