//
// Created by X-ray on 11/03/22.
//

#include "thread_pool.hpp"

#include <utility>

ThreadPool::ThreadPool() {
    CreateThreads(std::thread::hardware_concurrency());
  }

  ThreadPool::ThreadPool(std::size_t thread_count) {
    if (thread_count == 0) {
      CreateThreads(std::thread::hardware_concurrency());
    } else {
      CreateThreads(thread_count);
    }
  }

  ThreadPool::~ThreadPool() {
    running_ = false;
    job_notify_.notify_all();

    for (auto& thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  void ThreadPool::AddJob(ThreadPool::job_t task, ThreadPool::cb_t callback) {
    Job job{std::move(task), std::move(callback)};
    {
      std::unique_lock<std::mutex> lock(mtx_);
      tasks_.push(job);
    }
    job_notify_.notify_one();
  }

  std::size_t ThreadPool::GetThreadCount() const {
    return threads_.size();
  }

  void ThreadPool::WorkerLoop(ThreadPool* pool) {
    while (pool->running_) {
      std::unique_lock<std::mutex> lock(pool->mtx_);
      pool->job_notify_.wait(lock, [&pool] {
        return !pool->tasks_.empty() || !pool->running_;
      });

      if (!pool->running_) {
        return;
      }

      if (pool->tasks_.empty()) {
        continue;
      }

      auto task = std::move(pool->tasks_.front());
      pool->tasks_.pop();
      lock.unlock();

      std::invoke(task.job);
      if (task.cb) {
        std::invoke(task.cb);
      }
    }
  }

  void ThreadPool::CreateThreads(std::size_t thread_count) {
    for (std::size_t i = 0; i < thread_count; i++) {
      threads_.emplace_back(std::thread(WorkerLoop, this));
    }
  }