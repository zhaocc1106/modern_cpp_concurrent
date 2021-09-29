// Copyright 2021 netease. All rights reserved.
// File   thread_pool_timer.cpp
// Author zhaochaochao@corp.netease.com
// Date   2021/5/19 23:02
// Brief  One timer container which timer callback run in thread pool.

#include <memory>

#include "glog/logging.h"
#include "common/util/thread_pool_timer_container.h"

common::ThreadPoolTimerContainer::ThreadPoolTimerContainer(int worker_th_count) : state_(STOPPED),
                                                                                  worker_th_count_(worker_th_count),
                                                                                  io_work_(io_service_) {
}

bool common::ThreadPoolTimerContainer::Start() {
  std::lock_guard<std::mutex> state_lock(state_mutex_);
  if (state_ == STARTED) {
    LOG(WARNING) << "ThreadPoolTimerContainer have been started.";
    return false;
  }

  for (int i = 0; i < worker_th_count_; i++) {
    thread_group_.create_thread([this]() {
      LOG(INFO) << "Thread-[" << boost::this_thread::get_id() << "] Start\n";
      this->io_service_.run();
      LOG(INFO) << "Thread-[" << boost::this_thread::get_id() << "] Finish\n";
    });
  }

  state_ = STARTED;
  return true;
}

void common::ThreadPoolTimerContainer::InternalTimerCb(boost::system::error_code err, int64_t timer_id) {
  // LOG(INFO) << "InternalTimerCb thread_id: " << boost::this_thread::get_id() << ", err: " << err.message()
  //           << ", timer_id: " << timer_id;
  if (err) { // Timer have been canceled.
    LOG(WARNING) << "Timer [" << timer_id << "] have been canceled.";
    return;
  }

  std::lock_guard<std::mutex> timers_lock(timers_mutex_);
  if (timers_.count(timer_id) <= 0) {
    LOG(WARNING) << "Timer id [" << timer_id << "] not existed.";
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

int64_t common::ThreadPoolTimerContainer::AddTimer(std::function<void(void *)> timer_cb, void *args, int expired_ms,
                                                   bool repeated) {
  std::unique_lock<std::mutex> state_lock(state_mutex_);
  if (state_ == STOPPED) {
    LOG(WARNING) << "ThreadPoolTimerContainer should be started firstly.";
    return false;
  }
  state_lock.unlock();

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

bool common::ThreadPoolTimerContainer::CancelTimer(int64_t timer_id) {
  std::unique_lock<std::mutex> state_lock(state_mutex_);
  if (state_ == STOPPED) {
    LOG(WARNING) << "ThreadPoolTimerContainer should be started firstly.";
    return false;
  }
  state_lock.unlock();

  std::lock_guard<std::mutex> timers_lock(timers_mutex_);
  if (timers_.count(timer_id) <= 0) {
    LOG(WARNING) << "Timer id [" << timer_id << "] not existed.";
    return false;
  }

  LOG(INFO) << "CancelTimer timer_id: " << timer_id;
  timers_[timer_id]->timer_ptr_->cancel_one();
  timers_.erase(timer_id);
  return true;
}

bool common::ThreadPoolTimerContainer::Stop() {
  std::lock_guard<std::mutex> state_lock(state_mutex_);
  if (state_ == STOPPED) {
    LOG(WARNING) << "ThreadPoolTimerContainer have been stopped.";
    return false;
  }

  io_service_.stop();
  thread_group_.join_all();
  state_ = STOPPED;
  return true;
}
