/**
 * 使用RAII方式等待线程，防止异常发生时忘记join
 */

#include <iostream>
#include <thread>

void function(std::string const &i) {
    for (int j = 0; j < 10; j++) {
        std::cout << "function " << i << std::endl;
    }
}

class thread_guard {
private:
    std::thread &t;
public:
    explicit thread_guard(std::thread &t_) : t(t_) {
        // t只是引用了t_，所以两个都可以获取到id值
        std::cout << "thread guard construction t_: " << t_.get_id() << ", t: " << t.get_id() << std::endl;
    }

    // 对象析构时调用join，保证肯定能走到
    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }

    // 防止被拷贝
    thread_guard(const thread_guard &) = delete;
    thread_guard &operator=(const thread_guard &) = delete;
};

// 在thread_guard基础上，同时使它获取到线程的所有权（通过move转移得到了对线程的所有权）
class scoped_thread {
private:
    std::thread t;
public:
    explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
        // t_线程所有权转移给了t,所以t_.get_id()得到的是thread::id of a non-executing thread，而t.get_id()获得的是具体id值
        std::cout << "scoped thread construction t_: " << t_.get_id() << ", t: " << t.get_id() << std::endl;
        if (!t.joinable()) {
            throw std::logic_error("No thread");
        }
    }

    // 对象析构时调用join，保证肯定能走到
    ~scoped_thread() {
        std::cout << "scoped thread destruction" << std::endl;
        if (t.joinable()) {
            t.join();
        }
    }

    // 防止被拷贝
    scoped_thread(const scoped_thread &) = delete;
    scoped_thread &operator=(const scoped_thread &) = delete;
};

int main() {
    std::thread a(function, "local");
    thread_guard tg(a);
    std::thread b(function, "local2");
    scoped_thread st(std::move(b));
}