#include <cstdio>
#include <cstdlib>

#include "Log.h"

void error_handling(const char *message)
{
    LOG_ERROR("%s", message);
    exit(1);
}
