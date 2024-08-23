#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include <functional>

Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), events(0), revents(0), inEpoll(false)
{
}

Channel::~Channel()
{
}

void Channel::handleEvent()
{
    callback();
}

void Channel::enableReading()
{
    events = EPOLLIN | EPOLLET;
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

uint32_t Channel::getRevents()
{
    return revents;
}

bool Channel::getInEpoll()
{
    return inEpoll;
}

void Channel::setInEpoll()
{
    inEpoll = true;
}

void Channel::setRevents(uint32_t _revents)
{
    revents = _revents;
}

void Channel::setCallback(std::function<void()> _cb)
{
    callback = _cb;
}