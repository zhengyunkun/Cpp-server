#pragma once

#include <sys/epoll.h>
#include <vector>

class Epoll
{
    private:
        int epfd;                       // epoll_create返回的文件描述符
        struct epoll_event* events;     // 用于保存epoll_wait返回的触发的事件

    public:
        Epoll();
        ~Epoll();

        void addFd(int fd, uint32_t op);
        std::vector<epoll_event> poll(int timeout = -1);
        // timeout = -1表示一直等待
};