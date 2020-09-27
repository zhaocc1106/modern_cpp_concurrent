/**
 * 使用compare_exchange_weak实现支持并发stack的push动作
 */

#include <atomic>

template<typename T>
class lock_free_stack {
private:
    struct node {
        T data;
        node *next;

        node(T const &data_) :
                data(data_) {}
    };

    std::atomic<node *> head;
public:
    void push(T const &data) {
        node *const new_node = new node(data);
        new_node->next = head.load();  //如果head更新了，这条语句要重来一遍
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }
};