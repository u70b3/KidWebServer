#include <unistd.h>

#include "webserver.h"
#include "config.h"

int main()
{
    /* 守护进程 后台运行 */
    //daemon(1, 0);
    Config conf;
    conf.port = 65535;          //监听端口
    conf.trig_mode = 3;         //触发模式 ET
    conf.timeout_ms = 60000;    //超时时间
    conf.opt_linger = false;    //优雅退出
    conf.thread_num = 12;       //线程池数量
    conf.open_log = true;       //日志开关
    conf.log_level = 0;         //日志等级DEBUG
    conf.log_queue_size = 1024; //日志异步队列容量

    WebServer server(conf);
    server.Start();
}
