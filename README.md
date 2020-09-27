# c++并发编程
c++并发编程

## manageThread-线程基础管理
startThread.cpp: 线程的几种启动方式，参数传递。<br>
RAIIWaitThread.cpp: RAII编程方式wait线程，线程所有权转移，线程标识获取。<br>

## atomic-原子变量相关编程
atomicFlagLock.cpp: 使用atomicFlag实现一个自旋锁。<br>
casStack.cpp: 使用compare_exchange_weak实现一个并发安全stack。<br>
memBarriers.cpp: 使用内存屏障保障非原子变量的访问顺序。<br>
relaxedOrdering.cpp: 使用relaxed memory order实现一个并发计数器。<br>
releaseAcquireOrder.cpp: 使用release-acquire memory order保证非原子变量的访问顺序。<br>
sequentialConsistenOrdering.cpp: 使用sequence consistent memory order保证多个原子变量的访问顺序(happens before)。<br>