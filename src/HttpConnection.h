#ifndef __HTTP_CONNECTION_H_
#define __HTTP_CONNECTION_H_

#include <sys/types.h>
#include <sys/uio.h>   // readv/writev
#include <arpa/inet.h> // sockaddr_in
#include <stdlib.h>    // atoi()
#include <errno.h>

#include "Log.h"
#include "Buffer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpConnection
{
public:
    HttpConnection();

    ~HttpConnection();

    void Init(int sockFd, const sockaddr_in &addr);

    ssize_t read(int *saveErrno);

    ssize_t write(int *saveErrno);

    void Close();

    int GetFd() const;

    int GetPort() const;

    const char *GetIP() const;

    sockaddr_in GetAddr() const;

    bool process();

    int ToWriteBytes()
    {
        return iov_[0].iov_len + iov_[1].iov_len;
    }

    bool IsKeepAlive() const
    {
        return request_.IsKeepAlive();
    }

    static bool isET;
    static const char *srcDir;
    static std::atomic<int> userCount;

private:
    int fd_;
    struct sockaddr_in addr_;

    bool isClose_;

    int iovCnt_;
    struct iovec iov_[2];

    Buffer readBuff_;  // 读缓冲区
    Buffer writeBuff_; // 写缓冲区

    HttpRequest request_;
    HttpResponse response_;
};

#endif //__HTTP_CONNECTION_H_
