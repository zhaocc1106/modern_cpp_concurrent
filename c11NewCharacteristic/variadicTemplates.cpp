/**
 * 可变模版参数
 * https://www.cnblogs.com/qicosmos/p/4325949.html
 */

#include <iostream>

/* -------------------------------------可变模板参数定义函数------------------------------------- */
/* 简单的可变模板参数函数，该函数接受任意类型和任意个数的参数 */
template<typename... T>
void f(T... args) {
    std::cout << sizeof...(args) << std::endl;
}

/* 使用递归方式展开参数包 */
// 作为递归展开的终止函数
template<typename Head>
void print(Head head) {
    std::cout << head << std::endl;
}

template<typename Head, typename... Args>
void print(Head head, Args... args) {
    std::cout << head << std::endl;
    print(args...);
}

/* 逗号表达式展开参数包 */
template<typename T>
void print_arg(T arg) {
    std::cout << arg << std::endl;
}

template<typename... Args>
void expand(Args... args) {
    // C++11的另外一个特性——初始化列表，通过初始化列表来初始化一个变长数组, {(printarg(args), 0)...}将会展开成((printarg(arg1),0),
    // (printarg(arg2),0), (printarg(arg3),0),  etc... )
    int arr[] = {(print_arg(args), 0)...};
    std::initializer_list<int>{(print_arg(args), 0)...};
}

/* 支持lambda表达式的展开方式 */
template<typename F, typename... Args>
void expand2(F &&func, Args &&...args) {
    std::initializer_list<int>{(func(std::forward<Args>(args)), 0)...}; // 使用完美转发把参数直接传递给f函数
}


/* -------------------------------------可变模板参数定义类------------------------------------- */

/* 三段式模版偏特化和递归方式展开参数包 */
// 前向声明，声明这个sum类是一个可变参数模板类。
template<typename... Args>
class Sum;

// 类的具体定义，定义了一个部分展开的可变模参数模板类，告诉编译器如何递归展开参数包。
template<typename First, typename... Rest>
class Sum<First, Rest...> {
public:
    enum {
        value = Sum<First>::value + Sum<Rest...>::value
    };
};

// 特化的递归终止类，通过这个特化的类来终止递归。
template<typename Last>
class Sum<Last> {
public:
    enum {
        value = sizeof(Last)
    };
};

/* 二段式模版偏特化和递归方式展开参数包 */

// 类的具体定义，定义了一个部分展开的可变模参数模板类，告诉编译器如何递归展开参数包。
template<typename First, typename... Rest>
class Sum2 {
public:
    enum {
        value = Sum2<First>::value + Sum2<Rest...>::value
    };
};

// 特化的递归终止类，通过这个特化的类来终止递归。
template<typename Last>
class Sum2<Last> {
public:
    enum {
        value = sizeof(Last)
    };
};

/* 使用std::integral_constant来消除枚举定义value。利用std::integral_constant可以获得编译期常量的特性。 */
//前向声明
template<typename... Args>
struct Sum3;

//基本定义
template<typename First, typename... Rest>
struct Sum3<First, Rest...> : std::integral_constant<int, Sum3<First>::value + Sum3<Rest...>::value> {
};

//递归终止
template<typename Last>
struct Sum3<Last> : std::integral_constant<int, sizeof(Last)> {
};

/* 通过继承方式展开参数包 */
//整型序列的定义
template<int...>
struct IndexSeq {
};

//继承方式，开始展开参数包
template<int N, int... Indexes>
struct MakeIndexes : MakeIndexes<N - 1, N - 1, Indexes...> {
};

// 模板特化，终止展开参数包的条件
template<int... Indexes>
struct MakeIndexes<0, Indexes...> {
    typedef IndexSeq<Indexes...> type;
};

/* -------------------------------------可变模板参数简化代码写法------------------------------------- */

/* 简化一个工厂类写法 */

class ProductA {
public:
    int x;

    ProductA(int x_) : x(x_) {}
};

class ProductB {
public:
    int x;
    int y;

    ProductB(int x_, int y_) : x(x_), y(y_) {}
};

class ProductC {
public:
    int x;
    int y;
    int z;

    ProductC(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
};

template<typename T>
class Factory {
public:
    template<typename... Args>
    static T *instance(Args... args) {
        return new T(args...);
    }
};

/* 简化一个委托类的写法 */

template<typename T, typename R, typename... Args>
class MyDelegate {
    T *m_t; // 指向一个对象
    R (T::*m_f)(Args...); // 指向一个函数，函数参数是可变模板参数，返回值是R类型
public:
    MyDelegate(T *t, R (T::*f)(Args...)) : m_t(t), m_f(f) {}

    R operator()(Args &&... args) { // 实现()，代表是一个函数对象
        return (m_t->*m_f)(std::forward<Args>(args)...); // 完成函数参数的转发
    }
};

template<typename T, typename R, typename... Args>
MyDelegate<T, R, Args...> create_delegate(T *t, R (T::*f)(Args...)) {
    return MyDelegate<T, R, Args...>(t, f);
}

class A {
public:
    void func1(int i) {
        std::cout << i << std::endl;
    }

    void func2(int i, int j) {
        std::cout << i + j << std::endl;
    }
};

int main() {
    f(1); // sizeof为1
    f(1, 'a'); // sizeof为2
    f("123", 1, 'a'); // sizeof为3

    print(1, 2, 'a', 'b', "hello"); // 递归方式展开参数包

    expand(1, 2, 'a', 'b', "hello"); // 逗号表达式展开参数包

    expand2([](auto i) { std::cout << i << std::endl; }, 1, 2, 'a', 'b', "hello"); // 支持lambda表达式的展开方式

    std::cout << Sum<int, long, long long>::value << std::endl; // 三段式模版偏特化和递归方式展开参数包
    std::cout << Sum2<int, long, long long>::value << std::endl; // 二段式模版偏特化和递归方式展开参数包
    std::cout << Sum3<int, long, long long>::value << std::endl; // std::integral_constant消除枚举value定义

    // 通过继承方式展开参数包
    using T = MakeIndexes<3>::type;
    std::cout << typeid(T).name() << std::endl;

    // 可变模板参数简化工厂类实现代码
    ProductA *a_instance = Factory<ProductA>::instance(1);
    ProductB *b_instance = Factory<ProductB>::instance(1, 2);
    ProductC *c_instance = Factory<ProductC>::instance(1, 2, 3);
    std::cout << a_instance->x << std::endl;
    std::cout << b_instance->x << ", " << b_instance->y << std::endl;
    std::cout << c_instance->x << ", " << c_instance->y << ", " << c_instance->z << std::endl;
    delete a_instance;
    delete b_instance;
    delete c_instance;

    /* 简化一个委托类的写法 */
    A a;
    auto delegate1 = create_delegate(&a, &A::func1);
    auto delegate2 = create_delegate(&a, &A::func2);
    delegate1(1);
    delegate2(1, 2);
}


