/**
 * 函数名当作参数的不同方式
 */

#include <iostream>
#include <functional>

template<typename Function>
void function(Function &&func, // 万能引用
              const std::function<void(int)> &func2, // std::function
              void (*func3)(int) // 函数指针
) {
    func(10);
    func2(100);
    func3(1000);
}

void function2(int i) {
    std::cout << "function2: " << i << std::endl;
}

void function3(int i) {
    std::cout << "function3: " << i << std::endl;
}

void function4(int i) {
    std::cout << "function4: " << i << std::endl;
}

double my_devide(double x, double y) {
    return x / y;
}

struct Foo {
    void print_sum(int n1, int n2) {
        std::cout << n1 + n2 << '\n';
    }
};

class Foo2 {
public:
    int value = 100;

    void f() { std::cout << "f(" << this->value << ")\n"; }

    void g() { std::cout << "g(" << this->value << ")\n"; }
};

void apply(Foo2 *foo1, Foo2 *foo2,
           void (Foo2::*fun)()) { // 成员函数指针
    (foo1->*fun)();  // call fun on the object foo1
    (foo2->*fun)();  // call fun on the object foo2
}

int main() {
    function(function2, function3, function4);

    // std::bind绑定普通函数，v1表示占位符，普通函数名作为实参，会隐式转换成函数指针
    auto fn_half = std::bind(my_devide, std::placeholders::_1, 2);
    std::cout << fn_half(10) << std::endl;

    // std::bind绑定一个成员函数，必须显示的指定&Foo::print_sum，因为编译器不会将对象的成员函数隐式转换成函数指针，所以必须在Foo::print_sum
    // 前添加&，使用对象成员函数的指针时，必须要知道该指针属于哪个对象，因此第二个参数为对象的地址 &foo。
    Foo foo;
    auto f = std::bind(&Foo::print_sum, &foo, 95, std::placeholders::_1);
    f(5);

    // 成员函数指针用法
    Foo2 foo1{1};
    Foo2 foo2{2};
    apply(&foo1, &foo2, &Foo2::f);
    apply(&foo1, &foo2, &Foo2::g);
}
