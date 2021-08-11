#include "log.h"
#include "util.h"

#include <fcntl.h>

#include <cstdio>
#include <cstdlib>

void ErrorHandler(const char *message)
{
    LOG_ERROR("%s", message);
    exit(1);
}

// 将文件描述符设置为非阻塞
void SetNonBlockingMode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}
