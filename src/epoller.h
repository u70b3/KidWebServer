#pragma once

#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>

#include <vector>

class Epoller
{
public:
    explicit Epoller(int max_event = 1024);

    ~Epoller();

    bool AddFd(int fd, uint32_t events);

    bool ModifyFd(int fd, uint32_t events);

    bool DeleteFd(int fd);

    int Wait(int timeout_ms = -1);

    int GetEventFd(size_t i) const;

    uint32_t GetEvents(size_t i) const;

private:
    int epoll_fd_;

    std::vector<struct epoll_event> events_;
};
