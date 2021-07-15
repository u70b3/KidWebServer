#include <unistd.h>

#include "webserver.h"
#include "config.h"

int main()
{
    /* 守护进程 后台运行 */
    //daemon(1, 0);
    Config conf;
    conf.port = 65535;      //监听端口
    conf.trigMode = 3;      //触发模式 ET
    conf.timeoutMS = 60000; //超时时间
    conf.OptLinger = false; //优雅退出
    conf.threadNum = 6;     //线程池数量
    conf.openLog = true;    //日志开关
    conf.logLevel = 0;      //日志等级DEBUG
    conf.logQueSize = 1024; //日志异步队列容量
    
    WebServer server(conf);
    server.Start();
}
