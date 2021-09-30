// Copyright 2021 netease. All rights reserved.
// File   thread_pool_timer.h
// Author zhaochaochao@corp.netease.com
// Date   2021/5/18 20:55
// Brief  One timer container which timer callback run in thread pool.
//
// Use like:
// {
//   ThreadPoolTimerContainer thread_pool_timer_container(4); // Create one thread pool timer container with 4 worker thread.
//   thread_pool_timer_container.Start(); // Start thread pool timer container.
//   int64_t timer_id = thread_pool_timer_container.AddTimer(Func1, // function name.
//                                                           nullptr, // args.
//                                                           1000, // expired duration.
//                                                           nullptr, // if specify expired duration ptr, the expired duration can be changed dynamically.
//                                                           common::ThreadPoolTimerContainer::S, // duration precision is second.
//                                                           true); // if repeated.
//   ***
//   thread_pool_timer_container.CancelTimer(timer_id); // Cancel one timer in container.
//   ***
//   thread_pool_timer_container.Stop(); // Stop thread pool timer container.
// }

#ifndef PREDICTION_COMMON_UTIL_THREAD_POOL_TIMER_CONTAINER_H_
#define PREDICTION_COMMON_UTIL_THREAD_POOL_TIMER_CONTAINER_H_

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

namespace common {

class ThreadPoolTimerContainer {
 public:

  enum ContainerState {
    STARTED,
    STOPPED
  };

  // Precision of time.
  enum TimePrecision {
    MS, // Mill second.
    S, // Second.
    MINUTE // Minute.
  };

  struct TimerItem {
    std::unique_ptr<boost::asio::deadline_timer> timer_ptr_; // The timer obj ptr.
    std::function<void(void*)> timer_cb_; // Timer callback function like void func(void* args).
    void* args_; // The args of timer callback.
    int expired_; // The expired duration.
    int* expired_ptr_; // The expired duration ptr. Which can be changed by external.
    TimePrecision precision_; // Precision of expired duration.
    bool repeated_; // If timer is repeated.

    TimerItem(std::unique_ptr<boost::asio::deadline_timer> timer_ptr, std::function<void(void*)> timer_cb, void* args,
              int expired, int* expired_ptr, TimePrecision precision, bool repeated)
        : timer_ptr_(std::move(timer_ptr)), timer_cb_(std::move(timer_cb)), args_(args), expired_(expired),
          expired_ptr_(expired_ptr), precision_(precision), repeated_(repeated) {}

    // ~TimerItem() {
    //   std::cout << "Timer Item destruction function." << std::endl;
    // }
  };

  /**
   * The constructor function.
   * @param worker_th_count: The worker thread count in thread pool.
   */
  explicit ThreadPoolTimerContainer(int worker_th_count = 1);

  /**
   * Start the thread pool timer.
   *
   * @return if start successfully.
   */
  bool Start();

  /**
   * Add one new timer into container.
   * @param timer_cb: Timer callback function like void func(void* args). Can be func pointer, std::bind, std::func, or lambda.
   * @param args: The args of timer callback.
   * @param expired: The expired duration.
   * @param expired_ptr: The expired duration pointer. When not null, will use expired_ptr preferentially. The
   * expired duration can be changed by external.
   * @param repeat: If timer will be repeated.
   *
   * @return timer id which can used when cancel this timer.
   */
  int64_t AddTimer(std::function<void(void*)> timer_cb, void* args, int expired, int* expired_ptr,
                   TimePrecision precision, bool repeated);

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
  bool Stop();

  ~ThreadPoolTimerContainer();

 private:
  DISALLOW_COPY_AND_ASSIGN(ThreadPoolTimerContainer);

  /**
   * Internal callback of timer.
   * @param err: error code, if cancel, error code will be boost::asio::error::operation_aborted.
   * @param timer_id: The timer id of current timer callback.
   */
  void InternalTimerCb(boost::system::error_code err, int64_t timer_id);

  /**
   * Get expired mill seconds.
   * @param precision: Time precision.
   * @param duration: Duration.
   * @return Expired mill seconds.
   */
  inline boost::posix_time::millisec GetExpiredMs(TimePrecision precision, int duration) {
    switch (precision) {
      case MS:return boost::posix_time::millisec((long long) (duration));
      case S:return boost::posix_time::millisec((long long) (duration * 1e3));
      case MINUTE:return boost::posix_time::millisec((long long) (duration * 1e3 * 60));
      default:return boost::posix_time::millisec((long long) (0));
    }
  }

  ContainerState state_; // Current container state.
  std::mutex state_mutex_; // Used to protect state_ object in multi threads.
  int worker_th_count_; // The worker thread count in thread pool.
  boost::asio::io_service io_service_; // The io service of thread pool.
  boost::asio::io_service::work io_work_; // The work class is used to inform the io_service when work starts.
  boost::thread_group thread_group_; // Thread group used to create worker thread.
  std::unordered_map<int64_t, std::unique_ptr<TimerItem>> timers_; // Used to save all timers. Key is timer id.
  std::mutex timers_mutex_; // Used to protect timers_ object in multi threads.
};

}; // namespace common.

#endif //PREDICTION_COMMON_UTIL_THREAD_POOL_TIMER_CONTAINER_H_
