#pragma once

#include "log.h"

#include <arpa/inet.h>

#include <cassert>
#include <ctime>

#include <queue>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <chrono>

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode
{
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator<(const TimerNode &t)
    {
        return expires < t.expires;
    }
};
class HeapTimer
{
public:
    HeapTimer() { heap_.reserve(64); }

    ~HeapTimer() { clear(); }

    void adjust(int id, int new_expires);

    void add(int id, int timeout, const TimeoutCallBack &cb);

    void DoWork(int id);

    void clear();

    void tick();

    void pop();

    int GetNextTick();

private:
    void del_(size_t i);

    void siftup_(size_t i);

    bool siftdown_(size_t index, size_t n);

    void SwapNode_(size_t i, size_t j);

    std::vector<TimerNode> heap_;

    std::unordered_map<int, size_t> ref_;
};
