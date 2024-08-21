#pragma once


#include <sys/epoll.h>
#include <vector>

class Epoll
{
    private:
        int epfd;
        struct epoll_event* events; // epoll_event数组
    public:
        Epoll();
        ~Epoll();

        void addFd(int fd, uint32_t op);
        std::vector<epoll_event> poll(int timeout = -1);
};