/**
 * 使用互斥元实现一个多线程安全栈
 */

#include <iostream>
#include <thread>
#include <exception>
#include <stack>
#include <mutex>

// 定义一个exception，表示栈是空的
struct EmptyStack : std::exception {
    const char *what() const noexcept override {
        return "Empty stack";
    }
};

template<class T>
class ThreadSafeStack {
private:
    std::stack<T> data; // 存放数据的stack
    mutable std::mutex m; // 用于保护stack的互斥元，mutable可以打破函数是const描述的限制
public:
    ThreadSafeStack() {}

    // 拷贝构造函数
    ThreadSafeStack(const ThreadSafeStack &other) {
        std::lock_guard<std::mutex> lock(other.m); // 因为要访问other data，所以锁定other的数据，不用锁定当前stack data，因为它是构造函数
        data = other.data;
    }

    ThreadSafeStack &operator=(const ThreadSafeStack &other) = delete; // 不允许赋值构造

    // 安全push
    void push(T new_val) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_val);
    }

    // 通过引用获取的方式安全pop
    void pop(T &new_val) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            throw EmptyStack();
        }
        new_val = data.top();
        data.pop();
    }

    // 通过智能指针获取的方式安全pop
    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            throw EmptyStack();
        }
        std::shared_ptr<T> const res(std::make_shared<T>(data.top())); // 返回智能指针，智能指针被销毁后自动析构引用的对象，限制当前指针为常量
        data.pop();
        return res;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

int main() {
    ThreadSafeStack<int> st;
    try {
        st.pop();
    } catch (std::exception &ex) {
        std::cout << ex.what() << std::endl;
    }

    st.push(5);
    ThreadSafeStack<int> st2(st); // 拷贝一份

    std::shared_ptr<int> top = st.pop();
    std::cout << *top << std::endl;
    std::cout << *st2.pop() << std::endl;
}
