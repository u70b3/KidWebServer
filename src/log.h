#pragma once

#include "blocking_queue.h"
#include "buffer.h"

#include <sys/time.h>
#include <sys/stat.h> //mkdir

#include <cassert>
#include <cstdarg> // vastart va_end
#include <cstring>

#include <mutex>
#include <string>
#include <thread>

class Log
{
public:
    void Init(int level, const char *path = "./log",
              const char *suffix = ".log",
              int max_queue_capacity = 1024);

    static Log *Instance();
    static void FlushLogThread();

    void write(int level, const char *format, ...);
    void flush();

    int GetLevel();
    void SetLevel(int level);
    bool IsOpen() { return is_open_; }

private:
    Log();
    void AppendLogLevelTitle_(int level);
    virtual ~Log();
    void AsyncWrite_();

private:
    static const int LOG_PATH_LEN = 256;
    static const int LOG_NAME_LEN = 256;
    static const int MAX_LINES = 50000;

    const char *path_;
    const char *suffix_;

    int MAX_LINES_;

    int line_count_;
    int to_day_;

    bool is_open_;

    Buffer buff_;
    int level_;
    bool is_async_;

    FILE *fp_;
    std::unique_ptr<BlockingQueue<std::string>> deque_;
    std::unique_ptr<std::thread> write_thread_;
    std::mutex mtx_;
};

#define LOG_BASE(level, format, ...)                   \
    do                                                 \
    {                                                  \
        Log *log = Log::Instance();                    \
        if (log->IsOpen() && log->GetLevel() <= level) \
        {                                              \
            log->write(level, format, ##__VA_ARGS__);  \
            log->flush();                              \
        }                                              \
    } while (0);

#define LOG_DEBUG(format, ...)             \
    do                                     \
    {                                      \
        LOG_BASE(0, format, ##__VA_ARGS__) \
    } while (0);
#define LOG_INFO(format, ...)              \
    do                                     \
    {                                      \
        LOG_BASE(1, format, ##__VA_ARGS__) \
    } while (0);
#define LOG_WARN(format, ...)              \
    do                                     \
    {                                      \
        LOG_BASE(2, format, ##__VA_ARGS__) \
    } while (0);
#define LOG_ERROR(format, ...)             \
    do                                     \
    {                                      \
        LOG_BASE(3, format, ##__VA_ARGS__) \
    } while (0);
