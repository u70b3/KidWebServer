#ifndef __BLOCKING_QUEUE_H_
#define __BLOCKING_QUEUE_H_

#include <mutex>
#include <deque>
#include <condition_variable>
#include <sys/time.h>

template <class T>
class BlockingQueue
{
public:
    explicit BlockingQueue(size_t MaxCapacity = 1000);

    ~BlockingQueue();

    void clear();

    bool empty();

    bool full();

    void Close();

    size_t size();

    size_t capacity();

    T front();

    T back();

    void push_back(const T &item);

    void push_front(const T &item);

    bool pop(T &item);

    bool pop(T &item, int timeout);

    void flush();

private:
    std::deque<T> deq_;

    size_t capacity_;

    std::mutex mtx_;

    bool isClose_;

    std::condition_variable condConsumer_;

    std::condition_variable condProducer_;
};

template <class T>
BlockingQueue<T>::BlockingQueue(size_t MaxCapacity) : capacity_(MaxCapacity)
{
    assert(MaxCapacity > 0);
    isClose_ = false;
}

template <class T>
BlockingQueue<T>::~BlockingQueue()
{
    Close();
};

template <class T>
void BlockingQueue<T>::Close()
{
    {
        std::lock_guard<std::mutex> locker(mtx_);
        deq_.clear();
        isClose_ = true;
    }
    condProducer_.notify_all();
    condConsumer_.notify_all();
};

template <class T>
void BlockingQueue<T>::flush()
{
    condConsumer_.notify_one();
};

template <class T>
void BlockingQueue<T>::clear()
{
    std::lock_guard<std::mutex> locker(mtx_);
    deq_.clear();
}

template <class T>
T BlockingQueue<T>::front()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.front();
}

template <class T>
T BlockingQueue<T>::back()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.back();
}

template <class T>
size_t BlockingQueue<T>::size()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size();
}

template <class T>
size_t BlockingQueue<T>::capacity()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return capacity_;
}

template <class T>
void BlockingQueue<T>::push_back(const T &item)
{
    std::unique_lock<std::mutex> locker(mtx_);
    while (deq_.size() >= capacity_)
    {
        condProducer_.wait(locker);
    }
    deq_.push_back(item);
    condConsumer_.notify_one();
}

template <class T>
void BlockingQueue<T>::push_front(const T &item)
{
    std::unique_lock<std::mutex> locker(mtx_);
    while (deq_.size() >= capacity_)
    {
        condProducer_.wait(locker);
    }
    deq_.push_front(item);
    condConsumer_.notify_one();
}

template <class T>
bool BlockingQueue<T>::empty()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.empty();
}

template <class T>
bool BlockingQueue<T>::full()
{
    std::lock_guard<std::mutex> locker(mtx_);
    return deq_.size() >= capacity_;
}

template <class T>
bool BlockingQueue<T>::pop(T &item)
{
    std::unique_lock<std::mutex> locker(mtx_);
    while (deq_.empty())
    {
        condConsumer_.wait(locker);
        if (isClose_)
        {
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

template <class T>
bool BlockingQueue<T>::pop(T &item, int timeout)
{
    std::unique_lock<std::mutex> locker(mtx_);
    while (deq_.empty())
    {
        if (condConsumer_.wait_for(locker, std::chrono::seconds(timeout)) == std::cv_status::timeout)
        {
            return false;
        }
        if (isClose_)
        {
            return false;
        }
    }
    item = deq_.front();
    deq_.pop_front();
    condProducer_.notify_one();
    return true;
}

#endif // __BLOCKING_QUEUE_H_