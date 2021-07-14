#include "epoller.h"
#include "log.h"
#include "util.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

const int BUF_SIZE = 1024;
const int EPOLL_SIZE = 16;

int main1(int argc, char *argv[])
{
    Log::Instance()->Init(0, "./log", ".log", 0);
    LOG_INFO("========== Server start init ==========");

    sockaddr_in server_addr, client_addr;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    int server_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sock, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        error_handling("bind() error");
        exit(1);
    }

    if (listen(server_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    LOG_INFO("========== Server listening on %d ==========", server_addr.sin_port);

    auto epoller = new Epoller(EPOLL_SIZE);

    epoller->AddFd(server_sock, EPOLLIN);

    while (1)
    {
        auto event_cnt = epoller->Wait();
        if (event_cnt == -1)
        {
            LOG_ERROR("epoll_wait() error");
            break;
        }

        for (int i = 0; i < event_cnt; ++i)
        {
            int event_fd = epoller->GetEventFd(i);
            if (event_fd == server_sock)
            {
                socklen_t addr_size = sizeof(client_addr);
                int client_sock = accept(server_sock, (sockaddr *)&client_addr, &addr_size);
                setnonblockingmode(client_sock); // 设置client socket为非阻塞模式
                epoller->AddFd(client_sock, EPOLLIN | EPOLLET);
                LOG_INFO("connect client: %d\n", client_sock);
            }
            else
            {
                while (1)
                { // 将数据读完为止
                    ssize_t str_len = read(event_fd, buf, BUF_SIZE);
                    if (str_len == 0)
                    {
                        epoller->DeleteFd(event_fd);
                        close(event_fd);
                        LOG_INFO("closed client: %d\n", event_fd);
                    }
                    else if (str_len < 0)
                    {
                        if (errno == EAGAIN)
                        { // 已无数据
                            break;
                        }
                    }
                    else
                    {
                        const char *buf = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><head></head><body><!--body goes here--></body></html>";
                        auto _ = write(event_fd, buf, 1024);
                    }
                }
            }
        }
    }
    close(server_sock);
    delete epoller;

    return 0;
}
