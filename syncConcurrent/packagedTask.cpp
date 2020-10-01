/**
 * 通过packagedTask（包含可调用对象和future）并在线程之间传递任务，例如gui线程在工作时会接收其他线程的信息并刷新ui，这时可以传递一个task，
 * 其他线程还可以使用future等待它的task运行结束
 */

#include <iostream>
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

static std::deque<std::packaged_task<int()>> task_queue; // 任务队列
static std::mutex queue_mutex; // 保护队列的mutex

void gui_thread() {
    while (true) {
        std::packaged_task<int()> task;
        { // 使用大括号限制锁的存活范围，使得可以自动解锁
            std::lock_guard<std::mutex> lock(queue_mutex); // 锁队列
            if (task_queue.empty()) {
                continue;
            }
            task = std::move(task_queue.front());
            task_queue.pop_front();
        }
        task();
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 10ms执行一个任务
    }
}

// 该函数能够post一个任务给gui线程，并且调用函数即为参数f，返回值为future
template <typename func>
std::future<int> post_msg_to_gui(func f) {
    std::packaged_task<int()> task(f); // 封装一个task
    std::future<int> res_future = task.get_future();
    std::lock_guard<std::mutex> lock(queue_mutex); // 锁队列
    task_queue.push_back(std::move(task));
    return res_future;
}

int main() {
    std::thread t1(gui_thread);
    t1.detach();

    for (int i = 0; i < 10; i++) {
        std::future<int> res_future = post_msg_to_gui([&](){
            std::cout << "task [" << i << "] running..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return 100;
        });
        res_future.wait();
        std::cout << "task [" << i << "] return " << res_future.get() << std::endl;
    }
}