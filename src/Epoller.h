#ifndef __EPOLLER_H_
#define __EPOLLER_H_

#include <sys/epoll.h> //epoll_ctl()
#include <fcntl.h>     // fcntl()
#include <unistd.h>    // close()
#include <assert.h>    // close()
#include <vector>
#include <errno.h>

class Epoller
{
public:
    explicit Epoller(int maxEvent = 1024);

    ~Epoller();

    bool AddFd(int fd, uint32_t events);

    bool ModifyFd(int fd, uint32_t events);

    bool DeleteFd(int fd);

    int Wait(int timeoutMs = -1);

    int GetEventFd(size_t i) const;

    uint32_t GetEvents(size_t i) const;

private:
    int epollFd_;

    std::vector<struct epoll_event> events_;
};

#endif //__EPOLLER_H_
