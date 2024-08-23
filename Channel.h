#pragma once

#include <sys/epoll.h>

class Epoll;
class Channel // 将文件描述符封装成一个Channel类，方便管理
{
    private:
        Epoll* ep;          // 每个文件描述符都会被分发到一个Epoll类，用一个ep指针指向这个Epoll类
        int fd;             // fd
        uint32_t events;    // 希望监听这个文件描述符的哪些事件
        uint32_t revents;   // 表示在epoll返回该Channel时文件描述符正在发生的事件
        bool inEpoll;       // 当前Channel是否已经在epoll红黑树中，为了注册Channel的时候方便区分EPOLL_CTL_ADD还是EPOLL_CTL_MOD
    
    public:
        Channel(Epoll* _ep, int _fd);
        ~Channel();

        void enableReading();

        int getFd();
        uint32_t getEvents();
        uint32_t getRevents();
        bool getInEpoll();
        void setInEpoll();

        void setRevents(uint32_t);
};