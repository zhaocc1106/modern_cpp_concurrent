/**
 * 通过共享锁使得读取数据时能够共享获得锁并访问数据
 */

#include <iostream>
#include <mutex>
#include <map>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/shared_lock_guard.hpp>

class DnsEntry {
public:
    int val;

    DnsEntry() {}

    DnsEntry(int val_) : val(val_) {
    }
};

class DnsCache {
private:
    std::map<std::string, DnsEntry> entries_map;
    boost::shared_mutex entry_mutex; // 共享mutex

public:

    /* 使用共享锁，多个thread能够同时访问数据 */
    DnsEntry find_entry(std::string str) {
        boost::shared_lock_guard<boost::shared_mutex> lock(entry_mutex);
        auto const it = entries_map.find(str);
        return it == entries_map.end() ? DnsEntry() : it->second;
    }

    /* 使用独占锁，在更新数据时不允许其他线程进行读写 */
    void update_or_add_entry(const std::string &str, const DnsEntry &entry) {
        std::lock_guard<boost::shared_mutex> lock(entry_mutex);
        entries_map[str] = entry;
    }
};

int main() {
    DnsCache dns_cache;
    dns_cache.update_or_add_entry("123", DnsEntry(10));
    std::cout << dns_cache.find_entry("123").val << std::endl;
    dns_cache.update_or_add_entry("456", DnsEntry(20));
    std::cout << dns_cache.find_entry("456").val << std::endl;
}