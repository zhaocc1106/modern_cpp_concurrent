/**
 * 通过条件变量实现一个线程安全的队列
 */

#include <iostream>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

template<class T>
class ThreadSafeQueue {
private:
    std::queue<T> data_queue;
    mutable std::mutex queue_mutex; // 队列互斥元，mutable代表永久可变
    std::condition_variable queue_cond; // 队列条件变量

public:
    ThreadSafeQueue() {}

    ThreadSafeQueue(const ThreadSafeQueue &other) {
        std::lock_guard<std::mutex> lock(other.queue_mutex); // 锁定被拷贝对象的数据队列
        data_queue = other.data_queue;
    }

    /* 返回队列是否为空，const表示该函数无法修改任何对象的属性，queue_mutex用mutable描述，代表它可变。 */
    bool empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex);
        return data_queue.empty();
    }

    /* 队尾增加元素 */
    void push(T new_val) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        data_queue.push(new_val);
        queue_cond.notify_one(); // 随机唤醒一个线程
    }


    /* 使用引用方式获取队头的元素，如果队头无元素则阻塞等待 */
    void pop(T &new_val) {
        std::unique_lock<std::mutex> lock(queue_mutex); // 这里锁定不能使用lock_guard，因为lock_guard锁定后无法灵活的解锁，只能析构时解锁
        queue_cond.wait(lock, [this]() { return !data_queue.empty(); }); // 锁会在wait过程中解锁，一直等到lambda表达式成立
        new_val = data_queue.front();
        data_queue.pop();
    }

    /* 使用智能指针获取对头的元素，如果队头无元素则阻塞等待 */
    std::shared_ptr<T> pop() {
        std::unique_lock<std::mutex> lock(queue_mutex); // 这里锁定不能使用lock_guard，因为lock_guard锁定后无法灵活的解锁，只能析构时解锁
        queue_cond.wait(lock, [this]() { return !data_queue.empty(); }); // 锁会在wait过程中解锁，一直等到lambda表达式成立
        std::shared_ptr<T> res(data_queue.front());
        data_queue.pop();
        return res;
    }

    /* 使用引用方式获取队头的元素，如果队头无元素则立即返回 */
    bool try_pop(T &new_val) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (data_queue.empty()) {
            return false;
        }
        new_val = data_queue.front();
        data_queue.pop();
        return true;
    }

    /* 使用智能指针获取对头的元素，如果队头无元素则立即返回 */
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (data_queue.empty()) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> res(data_queue.front());
        data_queue.pop();
        return res;
    }
};

int main() {
    ThreadSafeQueue<int> thread_safe_queue;
    std::thread t1([&]() {
        for (int i = 0; i < 1000; i++) {
            thread_safe_queue.push(i);
        }
    });

    std::thread t2([&]() {
        while (true) {
            int res;
            thread_safe_queue.pop(res);
            std::cout << std::this_thread::get_id() << "--" << res << std::endl;
        }
    });

    std::thread t3([&]() {
        while (true) {
            int res;
            thread_safe_queue.pop(res);
            std::cout << std::this_thread::get_id() << "--" << res << std::endl;
        }
    });

    t1.join();
    t2.join();
    t3.join();
}