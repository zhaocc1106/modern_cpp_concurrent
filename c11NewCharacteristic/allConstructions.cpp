/**
 * 六个构造函数的写法
 */

#include <iostream>
#include <assert.h>
#include <cstring>
#include <vector>

using namespace std;

class Temp {
private:
    char *pData;
public:
    Temp(const char *str = nullptr); // 默认构造函数，因为参数具有默认值
    Temp(Temp &&t) noexcept; // 移动构造函数
    Temp &operator=(Temp &&t) noexcept; // 移动赋值函数
    Temp(const Temp &t); // 拷贝构造函数
    Temp &operator=(const Temp &t); // 赋值函数
    ~Temp(void);
};

Temp::Temp(const char *str) {
    cout << "default construction func." << endl;
    if (str == nullptr) {
        pData = nullptr;
    } else {
        this->pData = new char[strlen(str) + 1];
        strcpy(this->pData, str);
    }
}

Temp::Temp(Temp &&t) noexcept: pData(move(t.pData)) { // 移动构造函数，本对象的属性直接使用复制对象的属性的内存
    cout << "move construction func." << endl;
    t.pData = nullptr; // 防止复制对象析构时将这块内存销毁
}

Temp &Temp::operator=(Temp &&t) noexcept {
    cout << "move assign func." << endl;
    assert(this != &t);
    this->pData = nullptr;
    this->pData = move(t.pData);
    t.pData = nullptr;
    return *this;
}

Temp::Temp(const Temp &t) {
    cout << "copy construction func." << endl;
    if (t.pData == nullptr) {
        pData = nullptr;
    } else {
        this->pData = new char[strlen(t.pData) + 1];
        strcpy(this->pData, t.pData);
    }
}

Temp &Temp::operator=(const Temp &t) {
    cout << "assign func." << endl;
    if (this != &t) {
        delete[] pData; // 必须析构之前内存
        if (t.pData == nullptr) {
            pData = nullptr;
        } else {
            pData = new char[strlen(t.pData) + 1];
            strcpy(pData, t.pData);
        }
    }

    return *this;
}

Temp::~Temp() {
    cout << "deconstruction func." << endl;
    if (pData != nullptr) {
        delete[] pData;
        pData = nullptr;
    }
}

int main() {
    Temp temp; // 默认构造函数
    Temp temp1(temp); // 拷贝构造函数
    temp1 = temp; // 赋值函数
    vector <Temp> v(1); // 拷贝构造函数
    v.push_back(move(Temp())); // 一个默认构造函数，两个移动构造函数，第一个移动构造函数时push_back新的，第二个时移动之前旧的
    Temp temp2(move(Temp("123"))); // 移动构造函数
    Temp temp3("123"); // 默认构造函数
    Temp temp4 = temp3; // 拷贝构造函数
    Temp temp5 = move(temp3); // 移动构造函数
    temp5 = move(temp); // 移动赋值函数
    temp5 = Temp("123"); // 一个默认构造函数，一个移动赋值函数
}