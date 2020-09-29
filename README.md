# c++并发编程
c++并发编程

## manageThread-线程基础管理
```
startThread.cpp: 线程的几种启动方式，参数传递。
RAIIWaitThread.cpp: RAII编程方式wait线程，线程所有权转移，线程标识获取。
```

## sharedDataBetweenThreads-线程之间安全共享数据
```
threadSafeStack.cpp: 使用互斥元实现一个线程安全的stack，支持empty，push，pop。
lockMultiMutex.cpp: 同时锁定多个锁，减少死锁的风险。
lazyInitialize.cpp: “使用互斥元”，“二次检查锁定”（有数据竞争的风险，不推荐），“call-once”用法，“局部静态变量”多种方法保护lazy-initialization。
recursiveMutex.cpp: 递归锁（可重入锁）的使用方法。
sharedMutex.cpp: 使用boost库中的共享锁实现读写锁。
```

## syncConcurrent-同步并发操作
```
threadSafeQueue.cpp: 使用条件变量实现一个线程安全的队列
```


## atomic-原子变量相关编程
```
atomicFlagLock.cpp: 使用atomicFlag实现一个自旋锁。
casStack.cpp: 使用compare_exchange_weak实现一个并发安全stack push动作。
memBarriers.cpp: 使用内存屏障保障非原子变量的访问顺序。
relaxedOrdering.cpp: 使用relaxed memory order实现一个并发计数器。
releaseAcquireOrder.cpp: 使用release-acquire memory order保证非原子变量的访问顺序。
sequentialConsistenOrdering.cpp: 使用sequence consistent memory order保证多个原子变量的访问顺序(happens before)。
```
