/**
 * 递归锁（可重入锁）的使用，尽量减少这种锁的使用。
 */

#include <iostream>
#include <mutex>

std::recursive_mutex rm; // 该锁在已经持有的情况下可以多次持锁
void func1() {
    std::lock_guard<std::recursive_mutex> lock(rm);
    std::cout << "first lock" << std::endl;
}

void func2() {
    std::lock_guard<std::recursive_mutex> lock(rm);
    std::cout << "lock again" << std::endl;
}

int main() {
    func1();
    func2();
}
