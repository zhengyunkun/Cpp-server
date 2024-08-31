#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
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

        // void add(std::function<void()>);
        // 添加任务到任务队列

        template<class F, class... Args>
        auto add(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
};

//
template<class F, class... Args>
auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>> (
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        if (stop)
            throw std::runtime_error("Enqueue on stopped ThreadPool");
        tasks.emplace([task]() { (*task)(); });
    }

    cv.notify_one();
    return res;
}