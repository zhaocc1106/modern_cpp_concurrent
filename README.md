# c++并发编程
c++并发编程

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
[promise.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/promise.cpp): promise/future对用法。
[sharedFuture.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/syncConcurrent/sharedFuture.cpp): 使用sharedFuture实现多个线程同时等待一个线程。

## atomic-原子变量相关编程
[atomicFlagLock.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/atomicFlagLock.cpp): 使用atomicFlag实现一个自旋锁。<br>
[casStack.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/casStack.cpp): 使用compare_exchange_weak实现一个并发安全stack push动作。<br>
[sequentialConsistenOrdering.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/sequentialConsistenOrdering.cpp): 使用sequence consistent memory order保证多个原子变量的访问顺序(happens before)。<br>
[relaxedOrdering.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/relaxedOrdering.cpp): 使用relaxed memory order实现一个并发计数器。<br>
[releaseAcquireOrder.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/releaseAcquireOrder.cpp): 使用release-acquire memory order保证非原子变量的访问顺序。<br>
[memBarriers.cpp](https://github.com/zhaocc1106/cpp_concurrent_program/blob/master/atomic/memBarriers.cpp): 使用内存屏障保障非原子变量的访问顺序。<br>
