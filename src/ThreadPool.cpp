#include "ThreadPool.h"

// 存在大量的拷贝操作，任务队列的添加、取出都存在拷贝操作，性能低下
// 正确的做法是使用右值移动、完美转发等阻止拷贝操作

ThreadPool::ThreadPool(int size) : stop(false)
{
    for (int i = 0; i < size; i ++ )
    {
        threads.emplace_back(std::thread([this]() {
            // thread的参数是一个lambda表达式，[this]表示可以访问当前ThreadPool的成员比如tasks, stop, cv等
            // thread利用这个lambda表达式创建一个线程，线程的执行函数是这个lambda表达式
            while (true)
            {
                std::function<void()> task;
                // 用于存储从任务队列中取出的任务
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    // 任务队列锁，在{}内部持有锁，出了{}自动释放锁
                    cv.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });
                    if (stop && tasks.empty()) return;
                    task = tasks.front();
                    tasks.pop();
                }
                // 执行任务
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for (std::thread &th : threads) 
    {
        if (th.joinable()) 
            th.join();
    }
}