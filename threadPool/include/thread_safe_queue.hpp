/**
 * 线程安全的队列，使用链表和细粒度锁来实现增强并发性能。
 */

#ifndef CONCURRENT_LEARN_THREAD_SAFE_QUEUE_H
#define CONCURRENT_LEARN_THREAD_SAFE_QUEUE_H

#include <memory>
#include <mutex>
#include <condition_variable>

namespace zhaocc {
    template<typename T>
    class ThreadSafeQueue {
    private:
        struct Node {
            std::shared_ptr<T> data; // 保存节点数据部分
            std::unique_ptr<Node> next; // 保存下一个节点的指针
        };

        std::unique_ptr<Node> head; // 头节点指针使用unique_ptr封装，方便被pop出去后被销毁
        Node* tail; // 尾节点指针
        std::mutex head_mutex; // 保护头节点
        std::mutex tail_mutex; // 保护尾节点
        std::condition_variable data_cond; // 用于同步等待数据

        Node* get_tail(); // 线程安全获取尾部指针
        std::unique_ptr<Node> pop_head(); // 线程安全的弹出头部

    public:
        // 默认构造函数生成一个傀儡节点
        ThreadSafeQueue() : head(new Node), tail(head.get()) {}

        // 不允许拷贝构造
        ThreadSafeQueue(const ThreadSafeQueue& other) = delete;

        // 不允许拷贝赋值
        ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;

        // try_pop返回头部数据
        std::shared_ptr<T> try_pop();

        bool try_pop(T& value);

        // wait_and_pop表示阻塞等待有数据并获取头部数据
        std::shared_ptr<T> wait_and_pop();

        void wait_and_pop(T& value);

        // push往尾部添加数据
        void push(T new_value);

        // empty判断队列是否为空
        bool empty();
    };

    template<typename T>
    typename ThreadSafeQueue<T>::Node* ThreadSafeQueue<T>::get_tail() {
        std::lock_guard<std::mutex> lock(tail_mutex); // 先锁定尾部锁
        return tail;
    }

    template<typename T>
    std::unique_ptr<typename ThreadSafeQueue<T>::Node> ThreadSafeQueue<T>::pop_head() {
        std::lock_guard<std::mutex> lock(head_mutex); // 先锁定头部锁

        if (head.get() == get_tail()) { // 判断是否是空队列，这个get_tail里会锁定尾部锁，且不能放到锁定头部锁前
            return nullptr;
        }

        // 更新头部
        std::unique_ptr<Node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    template<typename T>
    std::shared_ptr<T> ThreadSafeQueue<T>::try_pop() {
        std::unique_ptr<Node> old_head = pop_head();
        return old_head ? old_head->data : std::make_shared<T>();
    }

    template<typename T>
    void ThreadSafeQueue<T>::push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value))); // 新的data
        std::unique_ptr<Node> p(new Node); // 新的unique_ptr(tail)
        Node* const new_tail = p.get(); // 新的尾部指针

        std::lock_guard<std::mutex> lock(tail_mutex); // 锁定尾部锁
        tail->data = new_data; // 修改当前尾部傀儡节点的data
        tail->next = std::move(p); // 将当前尾部傀儡节点的next指向新的unique_ptr(tail)
        tail = new_tail;
    }
}

#endif //CONCURRENT_LEARN_THREAD_SAFE_QUEUE_H
