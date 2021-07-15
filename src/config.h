#pragma once

#include "webserver.h"

class Config
{
public:
    int port;
    int trigMode;
    int timeoutMS;
    bool OptLinger;
    int threadNum;
    bool openLog;
    int logLevel;
    int logQueSize;
};
