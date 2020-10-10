/**
 * 右值与std::move
 * https://zhuanlan.zhihu.com/p/94588204
 */

#include <iostream>
#include <string>

using namespace std;

struct RValue {
    RValue() : sources("hello!!!") {}

    // 移动构造函数
    RValue(RValue &&a) {
        sources = std::move(a.sources);
        cout << "&& RValue" << endl;
    }

    // 拷贝构造函数
    RValue(const RValue &a) {
        sources = a.sources;
        cout << "& RValue" << endl;
    }

    // 移动赋值函数
    RValue &operator=(RValue &&a) {
        sources = std::move(a.sources);
        cout << "&& ==" << endl;
        return *this;
    }

    // 赋值函数
    RValue &operator=(const RValue &a) {
        sources = a.sources;
        cout << "& ==" << endl;
        return *this;
    }

    string sources;;
};

RValue get() {
    RValue a;
    a.sources = "123";
    return a;
}

void put(RValue) {}

int main() {
    // 左值是指表达式结束后依然存在的持久化对象，右值是指表达式结束时就不再存在的临时对象。所有的具名变量或者对象都是左值，而右值不具名。
    int i; // i是左值
    int &&j = i++; // i++会先返回一个右值再自加一
    // int&& k = ++i; // ++i会先加一再返回一个左值，所以编译失败
    // int& m = i++; // i++会先返回一个右值再自加一，所以编译失败
    int &l = ++i;

    RValue tmp;
    tmp = RValue(); // 会调用一次移动赋值函数

    // 后面这三句，在不添加-fno-elide-constructors参数编译时，经过编译优化能够防止临时变量的生成与拷贝。
    // 在添加-fno-elide-constructors参数编译防止对构造函数进行优化，让临时变量能够生成，从而会有移动构造函数的调用。
    RValue a = get(); // 会调用两次移动构造函数，第一次是return对象移动构造临时变量，第二次是将临时变量移动构造到a上。
    cout << "---------------" << endl;
    put(RValue()); // 调用一次移动构造函数，临时变量移动构造函数参数。

    // 下面这句即使不添加-fno-elide-constructors参数也会调用一次移动构造函数，std::move会强行把对象转成右值引用，这样就导致编译优化失效，是不可取操作。
    // 可见std::move并不是对所有临时对象添加都合适。
    put(std::move(RValue()));
    return 0;
}