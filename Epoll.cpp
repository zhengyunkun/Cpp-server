#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024

Epoll::Epoll()
{
    epfd = epoll_create1(0);
    errIf(epfd == -1, "Epoll create failed...");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(epoll_event) * MAX_EVENTS);
}

Epoll::~Epoll()
{
    if (epfd != -1)
    {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.events = op;
    ev.data.fd = fd;
    errIf(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "Epoll add fd failed...");
}

// 返回epoll中触发的事件
std::vector<Channel*> Epoll::poll(int timeout)
{
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errIf(nfds == -1, "Epoll wait failed...");
    
    for (int i = 0; i < nfds; i ++ )
    {
        Channel* ch = (Channel*)events[i].data.ptr;
        ch->setRevents(events[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}


void Epoll::updateChannel(Channel* channel)
{
    int fd = channel->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));

    ev.data.ptr = channel;
    ev.events = channel->getEvents();

    if (!channel->getInEpoll())
    {
        errIf(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "Epoll add failed...");
        channel->setInEpoll();
    }
    else errIf(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "Epoll mod failed...");
}
