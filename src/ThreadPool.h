#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
    private:
        std::vector<std::thread>            threads;    // 线程池
        std::queue<std::function<void()>>   tasks;      // 任务队列
        std::mutex                          tasks_mtx;  // 任务队列锁
        std::condition_variable             cv;         // 条件变量， 用于线程同步
        bool                                stop;       // 线程池是否停止

    public:
        ThreadPool(int size = 16);
        ~ThreadPool();

        void add(std::function<void()>);
        // 添加任务到任务队列
};