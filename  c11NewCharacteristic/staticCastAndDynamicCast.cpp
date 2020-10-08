/**
 * static_cast与dynamic_cast的区别
 */

#include <iostream>

class Base {
public:
    int val;

    Base() : val(0) {
    }

    explicit Base(int val_) : val(val_) {
    }

    virtual void print() const {
        std::cout << "Base print val: " << val << std::endl;
    }

    virtual ~Base() {
        std::cout << "Base destruction." << std::endl;
    }
};

class Derived : public Base {
private:
    int val;

public:
    Derived() : Base(1), val(0) {
    }

    Derived(int val_) : Base(val_ - 1), val(val_) {
    }

    void print() const override {
        Base::print();
        std::cout << "Derived print val: " << val << std::endl;
    }

    void print2() const {
        std::cout << "Derived print2" << std::endl;
    }

    ~Derived() override {
        std::cout << "Derived destruction." << std::endl;
    }
};

class Derived2 : public Base {
private:
    int val;

public:
    Derived2() : Base(1), val(0) {
    }

    Derived2(int val_) : Base(val_ - 1), val(val_) {
    }

    void print() const override {
        Base::print();
        std::cout << "Derived2 print val: " << val << std::endl;
    }

    void print2() const {
        std::cout << "Derived2 print2" << std::endl;
    }

    ~Derived2() override {
        std::cout << "Derived2 destruction." << std::endl;
    }
};

int main() {
    Base *ptr = new Derived();
    ptr->print();
    delete ptr;

    // static_cast向上转换是安全的
    Derived *ptr2 = new Derived();
    Base *ptr3 = static_cast<Base *>(ptr2);
    ptr3->print();
    delete ptr3;

    // static_cast向下转换不是安全的，不报错，不要这样转换
    Base *ptr4 = new Derived();
    Derived2 *ptr5 = static_cast<Derived2 *>(ptr4);
    std::cout << ptr5 << std::endl;
    ptr5->print2();
    delete ptr5;

    // dynamic_cast向上转换是安全的
    Derived *ptr6 = new Derived();
    Base *ptr7 = dynamic_cast<Base *>(ptr6);
    ptr7->print();
    delete ptr7;

    // dynamic_cast向下转换是安全的，转换出错返回0
    Base *ptr8 = new Derived();
    Derived2 *ptr9 = dynamic_cast<Derived2 *>(ptr8);
    if (ptr9) {
        ptr9->print2();
        delete ptr9;
    } else {
        delete ptr8;
    }

    // dynamic_cast向下转换时安全的，如果是引用转换，转换出错会throw bad_cast异常
    Base base;
    try {
        Derived &derived = dynamic_cast<Derived &>(base);
        derived.print2();
    } catch (std::bad_cast err) {
        std::cout << err.what() << std::endl;
    }
}
