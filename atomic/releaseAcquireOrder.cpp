/**
 * 释放-获取次序(release-acquire ordering)
 */

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <cassert>

using namespace std;

atomic<string *> ptr;
static int data = 0;

void produce() {
    data = 100;
    static string *tmp = new string("abc");
    ptr.store(tmp, memory_order_release);
}

void consume() {
    string *ptr2;
    while (!(ptr2 = ptr.load(memory_order_acquire))) {

    }

    cout << data << endl;
    cout << *ptr2 << endl;

    assert(data = 100); // 因为release操作前的动作不能优化到release后，acquire之后的操作不能优化到acquire之前，所以这里必为100
    assert(*ptr2 == "abc");
}

int main() {
    thread t1(produce);
    thread t2(consume);
    t1.join();
    t2.join();
}