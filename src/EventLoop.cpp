#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), threadPool(nullptr), quit(false)
{
    ep = new Epoll();
    threadPool = new ThreadPool();
}

EventLoop::~EventLoop()
{
    delete ep;
}

// 事件驱动，不断循环监听
// 实际上就是原来调用epoll_wait()的地方，现在封装成了一个loop()函数
void EventLoop::loop()
{
   while (!quit)
   {
        std::vector<Channel*> chs;
        chs = ep->poll(); // 从epoll中获取活跃的Channel
        for (auto t = chs.begin(); t != chs.end(); t ++ ) (*t)->handleEvent();  
        // 两种事件，一种是新连接，一种是连接后客户端数据到来，在具体Channel创建时区分
   }
}

void EventLoop::updateChannel(Channel* ch)
{
    ep->updateChannel(ch);
}

void EventLoop::addThread(std::function<void()> func)
{
    threadPool->add(func);
}