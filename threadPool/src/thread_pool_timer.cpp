// Copyright 2021 netease. All rights reserved.
// File   thread_pool_timer.cpp
// Author zhaochaochao@corp.netease.com
// Date   2021/5/19 23:02
// Brief  One timer container which timer callback run in thread pool.

#include <iostream>
#include <memory>

#include "thread_pool_timer.h"

ThreadPoolTimerContainer::ThreadPoolTimerContainer(int worker_th_count) : worker_th_count_(worker_th_count),
                                                                          io_work_(io_service_), state_(STOPPED) {
}

bool ThreadPoolTimerContainer::Start() {
  std::lock_guard<std::mutex> state_lock(state_mutex_);
  if (state_ == STARTED) {
    std::cout << "ThreadPoolTimerContainer have been started." << std::endl;
    return true;
  }

  for (int i = 0; i < worker_th_count_; i++) {
    thread_group_.create_thread([this]() {
      std::cout << "Thread-[" << boost::this_thread::get_id() << "] Start\n";
      this->io_service_.run();
      std::cout << "Thread-[" << boost::this_thread::get_id() << "] Finish\n";
    });
  }

  state_ = STARTED;
  return true;
}

void ThreadPoolTimerContainer::InternalTimerCb(boost::system::error_code err, int64_t timer_id) {
  // std::cout << "InternalTimerCb thread_id: " << boost::this_thread::get_id() << ", err: " << err.message()
  //           << ", timer_id: " << timer_id << std::endl;
  if (err) { // Timer have been canceled.
    std::cout << "Timer [" << timer_id << "] have been canceled." << std::endl;
    return;
  }

  std::lock_guard<std::mutex> timers_lock(timers_mutex_);
  if (timers_.count(timer_id) <= 0) {
    std::cout << "Timer id [" << timer_id << "] not existed." << std::endl;
    return;
  }

  timers_[timer_id]->timer_cb_(timers_[timer_id]->args_); // Invoke user layer timer callback.

  if (timers_[timer_id]->repeated_) { // If timer is repeated, start next timer countdown.
    // Update expired time.
    timers_[timer_id]->timer_ptr_->expires_at(timers_[timer_id]->timer_ptr_->expires_at()
                                                  + boost::posix_time::millisec(timers_[timer_id]->expired_ms_));;
    timers_[timer_id]->timer_ptr_->async_wait(
        boost::bind(&ThreadPoolTimerContainer::InternalTimerCb,
                    this,
                    boost::placeholders::_1,
                    timer_id));
  } else { // If not repeated, clear old timer item.
    timers_.erase(timer_id);
  }
}

int64_t ThreadPoolTimerContainer::AddTimer(std::function<void(void *)> timer_cb, void *args, int expired_ms,
                                           bool repeated) {
  auto timer_ptr = std::make_unique<boost::asio::deadline_timer>(io_service_, boost::posix_time::millisec(expired_ms));
  auto timer_id = (int64_t) timer_ptr.get(); // Use timer_ptr pointer as timer id.

  /* Start timer countdown. */
  timer_ptr->async_wait(boost::bind(&ThreadPoolTimerContainer::InternalTimerCb,
                                    this,
                                    boost::placeholders::_1,
                                    timer_id));

  /* Save timer item into map. */
  auto timer_item_ptr = std::make_unique<TimerItem>(std::move(timer_ptr), timer_cb, args, expired_ms, repeated);
  std::lock_guard<std::mutex> timers_lock(timers_mutex_);
  timers_[timer_id] = std::move(timer_item_ptr);

  return timer_id;
}

bool ThreadPoolTimerContainer::CancelTimer(int64_t timer_id) {
  std::lock_guard<std::mutex> timers_lock(timers_mutex_);
  if (timers_.count(timer_id) <= 0) {
    std::cout << "Timer id [" << timer_id << "] not existed." << std::endl;
    return false;
  }

  std::cout << "CancelTimer timer_id: " << timer_id << std::endl;
  timers_[timer_id]->timer_ptr_->cancel_one();
  timers_.erase(timer_id);
  return true;
}

bool ThreadPoolTimerContainer::Stop() {
  std::lock_guard<std::mutex> state_lock(state_mutex_);
  if (state_ == STOPPED) {
    std::cout << "ThreadPoolTimerContainer have been stopped." << std::endl;
    return true;
  }

  io_service_.stop();
  thread_group_.join_all();
  state_ = STOPPED;
  return true;
}

