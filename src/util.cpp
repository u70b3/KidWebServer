#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#include "Log.h"
#include "Util.h"

void error_handling(const char *message)
{
    LOG_ERROR("%s", message);
    exit(1);
}

// 将文件描述符设置为非阻塞
void setnonblockingmode(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag | O_NONBLOCK);
}
