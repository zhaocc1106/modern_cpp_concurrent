# 现代c++并发编程
现代c++并发编程

## manageThread-线程基础管理
[startThread.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/manageThread/startThread.cpp): 线程的几种启动方式，参数传递。<br>
[RAIIWaitThread.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/manageThread/RAIIWaitThread.cpp): RAII编程方式wait线程，线程所有权转移，线程标识获取。<br>

## sharedDataBetweenThreads-线程之间安全共享数据
[threadSafeStack.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/sharedDataBetweenThreads/threadSafeStack.cpp): 使用互斥元实现一个线程安全的stack，支持empty，push，pop。<br>
[lockMultiMutex.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/sharedDataBetweenThreads/lockMultiMutex.cpp): 同时锁定多个锁，减少死锁的风险。<br>
[lazyInitialize.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/sharedDataBetweenThreads/lazyIntialize.cpp): “使用互斥元”，“二次检查锁定”（有数据竞争的风险，不推荐），“call-once”用法，“局部静态变量”多种方法保护lazy-initialization。<br>
[recursiveMutex.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/sharedDataBetweenThreads/recursiveMutex.cpp): 递归锁（可重入锁）的使用方法。<br>
[sharedMutex.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/sharedDataBetweenThreads/sharedMutex.cpp): 使用boost库中的共享锁实现读写锁。<br>

## syncConcurrent-同步并发操作
[threadSafeQueue.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/threadSafeQueue.cpp): 使用条件变量实现一个线程安全的队列。<br>
[futrueAsync.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/futureAsync.cpp): future实现异步动作，不同policy的用法。<br>
[packagedTask.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/packagedTask.cpp): 通过packagedTask封裝task（包含可调用对象和future）并在线程之间传递任务。<br>
[promise.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/promise.cpp): promise/future对用法。<br>
[sharedFuture.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/sharedFuture.cpp): 使用sharedFuture实现多个线程同时等待一个线程。<br>
[spuriousWake.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/spuriousWake.cpp): 虚假唤醒测试。<br>
[time.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/time.cpp): 时间段与时间点与时钟节拍。<br>
[parallelQuickSort.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/parallelQuickSort.cpp): 使用future async实现一个并行快排。<br>

## atomic-原子变量与内存时序
[atomicFlagLock.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/atomicFlagLock.cpp): 使用atomicFlag实现一个自旋锁。<br>
[casStack.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/casStack.cpp): 使用compare_exchange_weak实现一个并发安全stack push动作。<br>
[sequentialConsistenOrdering.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/sequentialConsistenOrdering.cpp): 使用sequence consistent memory order保证多个原子变量的访问顺序(happens before)。<br>
[relaxedOrdering.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/relaxedOrdering.cpp): 使用relaxed memory order实现一个并发计数器。<br>
[releaseAcquireOrder.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/releaseAcquireOrder.cpp): 使用release-acquire memory order保证非原子变量的访问顺序。<br>
[memBarriers.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/memBarriers.cpp): 使用内存屏障保障非原子变量的访问顺序。<br>

## threadPool-线程池
[thread_safe_queue.hpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/thread_safe_queue.hpp): 使用链表以及细粒度锁实现一个高并发的线程安全队列。<br>
[threads_joiner.hpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/threads_joiner.hpp): 实现一个线程容器的joiner，在析构时能够join所有的线程。<br>
[simple_thread_pool.hpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/simple_thread_pool.hpp): 实现一个简单的线程池，固定多个工作线程一直在工作，进来任务会被分配给某一个工作线程给执行。<br>
[futured_thread_pool.hpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/futured_thread_pool.hpp): 基于simple_thread_pool开发的可以等待任务结果的线程池。<br>
[parallel_quick_sort.hpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/parallel_quick_sort.hpp): 基于futured_thread_pool开发的并行快排算法，可以控制并发数量。<br>
[multi_queue_thread_pool.hpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/multi_queue_thread_pool.hpp): 每个工作线程都有一个自己的“任务队列”的并且支持“任务窃取”的线程池，能够使得工作线程的并发性更高。<br>
[thread_pool_timer_container.h](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/include/thread_pool_timer_container.h)  [thread_pool_timer_container.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/src/thread_pool_timer_container.cpp)：基于boost线程池实现一个timer，timer callback跑在线程池中，并且支持循环timer。<br>
[my_thread_pool_test.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/src/my_thread_pool_test.cpp): 测试手写的thread pool。<br>
[boost_thread_pool_test.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/threadPool/src/boost_thread_pool_test.cpp): 测试常用的boost thread pool。<br>


## c11NewCharacteristic-c11一些新特性
[funcParam.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/c11NewCharacteristic/funcParam.cpp): 函数名当做参数的几种方式，std::function与std::bind用法。<br>
[allConstructions.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/c11NewCharacteristic/allConstructions.cpp): 六种构造函数实现方式，包括移动构造和移动赋值函数。<br>
[staticCastAndDynamicCast.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/c11NewCharacteristic/staticCastAndDynamicCast.cpp): static_cast与dynamic_cast区别。<br>
[variadicTemplates.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/c11NewCharacteristic/variadicTemplates.cpp): 可变模板参数的使用，包括可变模板参数函数和可变模板参数类以及参数包的展开方式。<br>
[smartPointer.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/c11NewCharacteristic/smartPointer.cpp): 智能指针(shared_ptr, unique_ptr, weak_ptr)的用法。<br>
[rightValueAndMove.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/c11NewCharacteristic/rightValueAndMove.cpp): 右值与移动构造、移动赋值函数。<br>
