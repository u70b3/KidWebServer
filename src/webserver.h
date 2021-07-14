#pragma once

#include "epoller.h"
#include "log.h"
#include "thread_pool.h"
#include "http_connection.h"
#include "heap_timer.h"

#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unordered_map>

#include <cassert>
#include <cerrno>

class WebServer
{
public:
    WebServer(
        int port, int trigMode, int timeoutMS, bool OptLinger,
        int threadNum,
        bool openLog, int logLevel, int logQueSize);

    ~WebServer();
    void Start();

private:
    bool InitSocket_();
    void InitEventMode_(int trigMode);
    void AddClient_(int fd, sockaddr_in addr);

    void DealListen_();
    void DealWrite_(HttpConnection *client);
    void DealRead_(HttpConnection *client);

    void SendError_(int fd, const char *info);
    void ExtentTime_(HttpConnection *client);
    void CloseConn_(HttpConnection *client);

    void OnRead_(HttpConnection *client);
    void OnWrite_(HttpConnection *client);
    void OnProcess(HttpConnection *client);

    static const int MAX_FD = 65536;

    static int SetFdNonBlock(int fd);

    int port_;
    bool openLinger_;
    int timeoutMS_; /* 毫秒MS */
    bool isClose_;
    int listenFd_;
    char *srcDir_;

    uint32_t listenEvent_;
    uint32_t connEvent_;

    std::unique_ptr<HeapTimer> timer_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int, HttpConnection> users_;
};
