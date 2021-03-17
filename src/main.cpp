#include <unistd.h>

#include "WebServer.h"

int main()
{
    /* 守护进程 后台运行 */
    //daemon(1, 0);

    WebServer server(
        65535, 3, 60000, false, /* 端口 ET模式 timeoutMs 优雅退出  */
        6, true, 1, 1024);     /* 线程池数量 日志开关 日志等级 日志异步队列容量 */
    server.Start();
}
