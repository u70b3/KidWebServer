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
    auto threedPool = new ThreadPool();
    for (size_t i = 0; i < 31; i++)
    {
        threedPool->AddTask(std::bind(Print, i, i * 100));
    }
    return 0;
}
