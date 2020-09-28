/**
 * 锁定多个互斥元的用法
 */

#include <iostream>
#include <mutex>

class BigData {
public:
    int val;

    BigData() {
        val = 0;
    }

    BigData(int val_) : val(val_) {
    }

    BigData(const BigData &other) {
        val = other.val;
    }
};

void swap_data(BigData &l, BigData &r) {
    int t = l.val;
    l.val = r.val;
    r.val = t;
}

class X {
private:
    std::mutex m;

public:
    BigData data;

    X(const BigData &data_) : data(data_) {
    }

    friend void swap(X &l, X &r);
    friend void swap2(X &l, X &r);
};

// 使用lock + lock_guard锁定与释放
void swap(X &l, X &r) {
    if (&l == &r) { // 如果是同一个实例，则直接返回
        return;
    }

    std::lock(l.m, r.m); // 同时锁定两个互斥元，要不两个同时锁定，要不同时不锁定，不能只锁定一个，减少死锁的可能性
    std::lock_guard<std::mutex> lock_l(l.m, std::adopt_lock); // adopt_lock表示lock_guard直接沿用互斥元已有的锁
    std::lock_guard<std::mutex> lock_r(r.m, std::adopt_lock);
    swap_data(l.data, r.data);
    // l.m.unlock();
    // r.m.unlock();
}

// 使用unique_lock + lock锁定和释放
void swap2(X &l, X &r) {
    if (&l == &r) { // 如果是同一个实例，则直接返回
        return;
    }

    std::unique_lock<std::mutex> lock_l(l.m, std::defer_lock); // defer_lock表示防止在构造时锁定
    std::unique_lock<std::mutex> lock_r(r.m, std::defer_lock);
    std::lock(lock_l, lock_r); // 同时锁定两个锁，注意这里传递的参数是两个unique_lock对象
    swap_data(l.data, r.data);
    // lock_l.unlock();
    // lock_r.unlock();
}

int main() {
    X a(BigData(5));
    X b(BigData(4));
    swap(a, b);
    std::cout << a.data.val << ", " << b.data.val << std::endl;
    swap2(a, b);
    std::cout << a.data.val << ", " << b.data.val << std::endl;
    swap2(a, b);
    std::cout << a.data.val << ", " << b.data.val << std::endl;
}
