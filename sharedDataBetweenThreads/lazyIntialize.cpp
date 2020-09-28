/**
 * 对于一些只需要在初始化时赋值的对象，之后再也不用更新，可以在进行初始化的时候进行保护，初始化动作很可能发生在用的时候（lazy-initialization），
 * 保护的方法包含“使用互斥元”，“二次检查锁定”（有数据竞争的风险，不推荐），“call-once”用法，“局部静态变量”（安全单例模式的实现）。
 */

#include <iostream>
#include <mutex>
#include <memory>

/* 需要被保护的资源 */
class Resource {
private:
    int val;
public:
    Resource(int val_) : val(val_) {}

    void do_some_thing() {
        std::cout << "do_some_thing " << val << std::endl;
    }
};

/* 使用互斥元保护lazy-initialization，即使资源已经被初始化，仍然要持锁，造成性能损失 */
std::mutex resource_mutex;
std::shared_ptr<Resource> res_ptr;
void lazy_initial_with_mutex() {
    std::unique_lock<std::mutex> lock(resource_mutex); // 使用灵活的unique_lock，可以灵活的释放锁
    if (!res_ptr) {
        std::cout << "lazy-initialize" << std::endl;
        res_ptr.reset(new Resource(10));
    }
    lock.unlock();
    res_ptr->do_some_thing();
}

/* 二次检查锁定（有数据竞争的风险，不推荐）保护lazy-initialization */
void lazy_initial_with_double_check() {
    if (!res_ptr) { // 本意是防止在已经初始化后每次都持锁，但是不能完全保证res_ptr不为空的时候就能安全的访问它，有数据竞争的风险
        std::unique_lock<std::mutex> lock(resource_mutex); // 使用灵活的unique_lock，可以灵活的释放锁
        if (!res_ptr) {
            std::cout << "lazy-initialize" << std::endl;
            res_ptr.reset(new Resource(10));
        }
        lock.unlock();
    }
    res_ptr->do_some_thing();
}

/* call-once保护lazy-initialization，安全，性能损失小，推荐该方式 */
std::once_flag resource_flag;

void init_resource() {
    std::cout << "lazy-initialize" << std::endl;
    res_ptr.reset(new Resource(10));
}

void lazy_initial_with_call_once() {
    std::call_once(resource_flag, init_resource);
    res_ptr->do_some_thing();
}

// lazy-init单例（magic static）--局部静态变量
class Singleton {
private:
    Singleton() {
        std::cout << "construction func" << std::endl;
    }

    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton &) = delete;

public:
    ~Singleton() {
        std::cout << "destruction func" << std::endl;
    }

    static Singleton &getInstance() {
        static Singleton instance; // c11保证多个线程只有一个线程能够对静态变量初始化，所以线程安全
        return instance;
    }
};

int main() {
    // lazy_initial_with_mutex();
    // lazy_initial_with_mutex();

    // lazy_initial_with_double_check();
    // lazy_initial_with_double_check();
    lazy_initial_with_call_once();
    lazy_initial_with_call_once();
}