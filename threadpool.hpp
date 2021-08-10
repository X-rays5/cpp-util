//
// Created by X-ray on 8/10/2021.
//
#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <functional>
#include <mutex>

class thread_pool {
public:
    using job_t = std::function<void()>;

    explicit thread_pool(int thread_count) {
        std::lock_guard lock(mutex_);
        for (int i = 0; i < thread_count; i++) {
            threads_.emplace_back([this]{
                while(this->running_) {
                    this->run_tick();
                }
            });
        }
        threads_.shrink_to_fit();
    }

    ~thread_pool() {
        {
            std::lock_guard lock(mutex_);
            running_ = false;
        }
        // give threads time to shutdown
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard lock(mutex_);
        for (auto&& thread : threads_) {
            if (thread.joinable())
                thread.join();
        }
    }

    void add_job(job_t job) {
        try {
            if (job) {
                std::lock_guard lock(mutex_);
                jobs_.emplace(std::move(job));
            }
        } catch(...) {
        }
    }

private:
    std::vector<std::thread> threads_;
    std::queue<job_t> jobs_;
    std::mutex mutex_;
    bool running_ = true;
private:
    void run_tick() {
        try {
            job_t job = nullptr;
            {
                std::lock_guard lock(mutex_);
                if (!jobs_.empty()) {
                    job = jobs_.front();
                    jobs_.pop();
                }
            }
            if (job) {
                std::invoke(job);
            }
        } catch(...) {}
    }
};
