#include "http_connection.h"

using namespace std;

const char *HttpConnection::src_dir;
std::atomic<int> HttpConnection::user_count;
bool HttpConnection::is_ET;

HttpConnection::HttpConnection()
{
    fd_ = -1;
    addr_ = {0};
    is_close_ = true;
};

HttpConnection::~HttpConnection()
{
    Close();
};

void HttpConnection::Init(int fd, const sockaddr_in &addr)
{
    assert(fd > 0);
    user_count++;
    addr_ = addr;
    fd_ = fd;
    write_buffer_.RetrieveAll();
    read_buffer_.RetrieveAll();
    is_close_ = false;
    LOG_INFO("Client[%d](%s:%d) in, user_count:%d", fd_, GetIP(), GetPort(), (int)user_count);
}

void HttpConnection::Close()
{
    response_.UnmapFile();
    if (is_close_ == false)
    {
        is_close_ = true;
        user_count--;
        close(fd_);
        LOG_INFO("Client[%d](%s:%d) quit, UserCount:%d", fd_, GetIP(), GetPort(), (int)user_count);
    }
}

int HttpConnection::GetFd() const
{
    return fd_;
};

struct sockaddr_in HttpConnection::GetAddr() const
{
    return addr_;
}

const char *HttpConnection::GetIP() const
{
    return inet_ntoa(addr_.sin_addr);
}

int HttpConnection::GetPort() const
{
    return addr_.sin_port;
}

ssize_t HttpConnection::read(int *save_errno)
{
    ssize_t len = -1;
    do
    {
        len = read_buffer_.ReadFd(fd_, save_errno);
        if (len <= 0)
        {
            break;
        }
    } while (is_ET);
    return len;
}

ssize_t HttpConnection::write(int *save_errno)
{
    ssize_t len = -1;
    do
    {
        len = writev(fd_, iov_, iov_cnt_);
        if (len <= 0)
        {
            *save_errno = errno;
            break;
        }
        if (iov_[0].iov_len + iov_[1].iov_len == 0)
        {
            break;
        } /* 传输结束 */
        else if (static_cast<size_t>(len) > iov_[0].iov_len)
        {
            iov_[1].iov_base = (uint8_t *)iov_[1].iov_base + (len - iov_[0].iov_len);
            iov_[1].iov_len -= (len - iov_[0].iov_len);
            if (iov_[0].iov_len)
            {
                write_buffer_.RetrieveAll();
                iov_[0].iov_len = 0;
            }
        }
        else
        {
            iov_[0].iov_base = (uint8_t *)iov_[0].iov_base + len;
            iov_[0].iov_len -= len;
            write_buffer_.Retrieve(len);
        }
    } while (is_ET || ToWriteBytes() > 10240);
    return len;
}

bool HttpConnection::process()
{
    request_.Init();
    if (read_buffer_.ReadableBytes() <= 0)
    {
        return false;
    }
    else if (request_.parse(read_buffer_))
    {
        LOG_DEBUG("%s", request_.path().c_str());
        response_.Init(src_dir, request_.path(), request_.IsKeepAlive(), 200);
    }
    else
    {
        response_.Init(src_dir, request_.path(), false, 400);
    }

    response_.MakeResponse(write_buffer_);
    /* 响应头 */
    iov_[0].iov_base = const_cast<char *>(write_buffer_.Peek());
    iov_[0].iov_len = write_buffer_.ReadableBytes();
    iov_cnt_ = 1;

    /* 文件 */
    if (response_.FileLen() > 0 && response_.File())
    {
        iov_[1].iov_base = response_.File();
        iov_[1].iov_len = response_.FileLen();
        iov_cnt_ = 2;
    }
    LOG_DEBUG("filesize:%d, %d  to %d", response_.FileLen(), iov_cnt_, ToWriteBytes());
    return true;
}