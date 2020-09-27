/**
 * 多种启动线程的编程方式
 */

#include <iostream>
#include <thread>

// 简单函数方式
void do_some_work(int i) {
    std::cout << "do_some_work i: " << i << std::endl;
}

// 函数对象方式
class backgroundThread {
public:
    void operator()(int i) {
        std::cout << " backgroundThread function object i: " << i << std::endl;
    }
};

typedef struct {
    int val;
} type;

class X {
public:
    void do_some_work(type &t) {
        std::cout << "X-do_some_work" << std::endl;
        t.val = 0;
    }
};

int main() {
    std::thread a(do_some_work, 1); // 简单函数
    std::thread b{backgroundThread(), 2}; // 函数对象
    std::thread c([](int i) { std::cout << "lambda function i: " << i << std::endl; }, 3); // lambda构造函数对象
    type t;
    t.val = 10;
    X x;
    std::thread d(&X::do_some_work, &x, std::ref(t)); // 类构造函数方式，显示说明传递的参数是引用类型

    a.join();
    b.join();
    c.join();

    std::cout << t.val << std::endl;
}