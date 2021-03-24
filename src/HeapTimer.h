#ifndef __HEAP_TIMER_H_
#define __HEAP_TIMER_H_

#include <cassert>
#include <ctime>

#include <queue>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <chrono>

#include <arpa/inet.h>

#include "Log.h"

typedef std::function<void()> TimeoutCallBack;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds MS;
typedef Clock::time_point TimeStamp;

struct TimerNode
{
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    friend bool operator<(const TimerNode &lhs, const TimerNode &rhs)
    {
        return lhs.expires < rhs.expires;
    }
};

class HeapTimer
{
public:
    HeapTimer() {}

    ~HeapTimer() { clear(); }

    void adjust(int id, int newExpires);

    void add(int id, int timeOut, const TimeoutCallBack &cb);

    void clear();

    void tick();

    void pop();

    int GetNextTick();

private:
    std::priority_queue<TimerNode,
                        std::vector<TimerNode>,
                        std::less<std::vector<TimerNode>::value_type>>
        heap_;

    std::unordered_map<int, TimerNode> ref_;
};

#endif //__HEAP_TIMER_H_