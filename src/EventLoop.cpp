#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() : ep(nullptr), quit(false)
{
    ep = new Epoll();
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
   }
}

void EventLoop::updateChannel(Channel* ch)
{
    ep->updateChannel(ch);
}