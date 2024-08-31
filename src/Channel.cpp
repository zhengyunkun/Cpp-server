#include "Channel.h"
#include "EventLoop.h"
#include <functional>
#include <unistd.h>

Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), events(0), ready(0), inEpoll(false), useThreadPool(true)
{
}

Channel::~Channel()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Channel::handleEvent()
{
    // loop->addThread(callback);  // 通过线程池处理事件
    // callback();
    if (ready & (EPOLLIN | EPOLLPRI))
    {
        if (useThreadPool) loop->addThread(readcallback);
        else readcallback();
    }
    if (ready & (EPOLLOUT))
    {
        if (useThreadPool) loop->addThread(writecallback);
        else writecallback();
    }
}

void Channel::enableReading()
{
    events |= EPOLLIN | EPOLLPRI;
    loop->updateChannel(this);
}

void Channel::useET()
{
    events |= EPOLLET;
    loop->updateChannel(this);
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}

uint32_t Channel::getReady()
{
    return ready;
}

bool Channel::getInEpoll()
{
    return inEpoll;
}

void Channel::setInEpoll(bool _in)
{
    inEpoll = _in;
}

void Channel::setReady(uint32_t _ev)
{
    ready = _ev;
}

void Channel::setReadCallback(std::function<void()> _cb)
{
    readcallback = _cb;
}

void Channel::setUseThreadPool(bool use)
{
    useThreadPool = use;
}