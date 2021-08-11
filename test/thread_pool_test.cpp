#include "src/thread_pool.h"
#include <iostream>

void Print(int i, int cnt)
{
    while (cnt--)
    {
        std::cout << i << "print " << std::endl;
    }
}

int main()
{
    auto threed_pool = new ThreadPool();
    for (size_t i = 0; i < 31; i++)
    {
        threed_pool->AddTask(std::bind(Print, i, i * 100));
    }
    return 0;
}
