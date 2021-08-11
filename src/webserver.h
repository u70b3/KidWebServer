#pragma once

#include "config.h"
#include "epoller.h"
#include "heap_timer.h"
#include "http_connection.h"
#include "log.h"
#include "thread_pool.h"

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
    WebServer(const Config &conf);

    ~WebServer();
    void Start();

private:
    bool InitSocket_();
    void InitEventMode_(int trig_mode);
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
    bool open_linger_;
    int timeout_ms_; /* 毫秒MS */
    bool is_close_;
    int listen_fd_;
    char *src_dir_;

    uint32_t listen_event_;
    uint32_t conn_event_;

    std::unique_ptr<HeapTimer> timer_;
    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int, HttpConnection> users_;
};
