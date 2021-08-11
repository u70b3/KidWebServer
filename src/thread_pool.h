#pragma once

#include <cassert>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>

class ThreadPool
{
public:
    explicit ThreadPool(size_t thread_num) : pool_(std::make_shared<Pool>())
    {
        assert(thread_num > 0);
        for (size_t i = 0; i < thread_num; i++)
        {
            std::thread([pool = pool_]
                        {
                            std::unique_lock<std::mutex> locker(pool->mtx);
                            while (true)
                            {
                                if (!pool->tasks.empty())
                                {
                                    auto task = std::move(pool->tasks.front());
                                    pool->tasks.pop();
                                    locker.unlock();
                                    task();
                                    locker.lock();
                                }
                                else if (pool->is_closed)
                                    break;
                                else
                                    pool->cv.wait(locker);
                            }
                        })
                .detach();
        }
    }

    ThreadPool() : ThreadPool(std::thread::hardware_concurrency())
    {
        ;
    }

    ThreadPool(ThreadPool &&) = default;

    ~ThreadPool()
    {
        if (static_cast<bool>(pool_))
        {
            {
                std::lock_guard<std::mutex> locker(pool_->mtx);
                pool_->is_closed = true;
            }
            pool_->cv.notify_all();
        }
    }

    template <class F>
    void AddTask(F &&task)
    {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cv.notify_one();
    }

private:
    struct Pool
    {
        std::mutex mtx;
        std::condition_variable cv;
        bool is_closed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};
