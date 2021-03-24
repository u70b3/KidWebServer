#include "HeapTimer.h"

void HeapTimer::add(int id, int timeout, const TimeoutCallBack &cb)
{
    assert(id >= 0);
    TimerNode node{id, Clock::now() + MS(timeout), cb};
    heap_.push(node);
    ref_[id] = node;
}

void HeapTimer::adjust(int id, int timeout)
{
    /* 调整指定id的结点 */
    assert(!heap_.empty() && ref_.count(id) > 0);
    // 有 bug
    ref_[id].expires = Clock::now() + MS(timeout);
}

void HeapTimer::tick()
{
    /* 清除超时结点 */
    if (heap_.empty())
    {
        return;
    }
    while (!heap_.empty())
    {
        TimerNode node = heap_.top();
        if (std::chrono::duration_cast<MS>(node.expires - Clock::now()).count() > 0)
        {
            break;
        }
        node.cb();
        pop();
    }
}

void HeapTimer::pop()
{
    assert(!heap_.empty());
    heap_.pop();
}

void HeapTimer::clear()
{
    ref_.clear();
    while (!heap_.empty())
    {
        heap_.pop();
    }
}

int HeapTimer::GetNextTick()
{
    tick();
    size_t res = -1;
    if (!heap_.empty())
    {
        res = std::chrono::duration_cast<MS>(heap_.top().expires - Clock::now()).count();
        if (res < 0)
        {
            res = 0;
        }
    }
    return res;
}
