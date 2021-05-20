// Copyright 2021 netease. All rights reserved.
// File   thread_pool_timer.h
// Author zhaochaochao@corp.netease.com
// Date   2021/5/18 20:55
// Brief  One timer container which timer callback run in thread pool.

#ifndef THREADPOOL_INCLUDE_THREAD_POOL_TIMER_H_
#define THREADPOOL_INCLUDE_THREAD_POOL_TIMER_H_

#include <functional>
#include <unordered_map>
#include <utility>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private declarations for a class
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;
#endif

class ThreadPoolTimerContainer {
 public:

  enum ContainerState {
    STARTED,
    STOPPED
  };

  struct TimerItem {
    std::unique_ptr<boost::asio::deadline_timer> timer_ptr_; // The timer obj ptr.
    std::function<void(void *)> timer_cb_; // Timer callback function like void func(void* args).
    void *args_; // The args of timer callback.
    int expired_ms_; // The expired duration(ms).
    bool repeated_; // If timer is repeated.

    TimerItem(std::unique_ptr<boost::asio::deadline_timer> timer_ptr, std::function<void(void *)> timer_cb, void *args,
              int expired_ms, bool repeated)
        : timer_ptr_(std::move(timer_ptr)), timer_cb_(std::move(timer_cb)), args_(args), expired_ms_(expired_ms),
          repeated_(repeated) {}

    ~TimerItem() {
      std::cout << "Timer Item destruction function." << std::endl;
    }
  };

  /**
   * The constructor function.
   * @param worker_th_count: The worker thread count in thread pool.
   */
  explicit ThreadPoolTimerContainer(int worker_th_count = 2);

  /**
   * Start the thread pool timer.
   *
   * @return if start successfully.
   */
  bool Start(void);

  /**
   * Add one new timer into container.
   * @param timer_cb: Timer callback function like void func(void* args).
   * @param args: The args of timer callback.
   * @param expired_ms: The expired duration(ms).
   * @param repeat: If timer will be repeated.
   *
   * @return timer id which can used when cancel this timer.
   */
  int64_t AddTimer(std::function<void(void *)> timer_cb, void *args, int expired_ms, bool repeated);

  /**
   * Cancel one timer.
   * @param timer_id: Timer indicated by timer id will be canceled.
   * @return If cancel successfully.
   */
  bool CancelTimer(int64_t timer_id);

  /**
   * Stop the thread pool timer.
   * @return If stop successfully.
   */
  bool Stop(void);

  ~ThreadPoolTimerContainer() = default;

 private:
  DISALLOW_COPY_AND_ASSIGN(ThreadPoolTimerContainer);

  /**
   * Internal callback of timer.
   * @param err: error code, if cancel, error code will be boost::asio::error::operation_aborted.
   */
  void InternalTimerCb(boost::system::error_code err, int64_t timer_id);

  ContainerState state_; // Current container state.
  std::mutex state_mutex_; // Used to protect state_ object in multi threads.
  int worker_th_count_; // The worker thread count in thread pool.
  boost::asio::io_service io_service_; // The io service of thread pool.
  boost::asio::io_service::work io_work_; // The work class is used to inform the io_service when work starts.
  boost::thread_group thread_group_; // Thread group used to create worker thread.
  std::unordered_map<int64_t, std::unique_ptr<TimerItem>> timers_; // Used to save all timers. Key is timer id.
  std::mutex timers_mutex_; // Used to protect timers_ object in multi threads.
};

#endif //THREADPOOL_INCLUDE_THREAD_POOL_TIMER_H_
