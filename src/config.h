#pragma once

#include "webserver.h"

class Config
{
public:
    int port;
    int trig_mode;
    int timeout_ms;
    bool opt_linger;
    int thread_num;
    bool open_log;
    int log_level;
    int log_queue_size;
};
