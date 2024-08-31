#pragma once

#include <sys/epoll.h>
#include <functional>   // 操作函数对象

class EventLoop;
class Channel // 将文件描述符封装成一个Channel类，方便管理
{
    private:
        // Epoll* ep;                        // 每个文件描述符都会被分发到一个Epoll类，用一个ep指针指向这个Epoll类
        EventLoop* loop;
        int fd;                              // fd
        uint32_t events;                     // 希望监听这个文件描述符的哪些事件
        uint32_t ready;                     
        bool inEpoll;                        // 当前Channel是否已经在epoll红黑树中，为了注册Channel的时候方便区分EPOLL_CTL_ADD还是EPOLL_CTL_MOD
        bool useThreadPool;                  // 是否使用线程池
        std::function<void()> readcallback;  // 读回调函数
        std::function<void()> writecallback; // 写回调函数
    
    public:
        Channel(EventLoop* _loop, int _fd);
        ~Channel();

        void handleEvent();     // 调用callback，执行Acceptor的newConnection函数
        void enableReading();

        int getFd();
        uint32_t getEvents();
        uint32_t getReady();
        bool getInEpoll();
        void setInEpoll(bool _in = true);
        void useET();

        void setReady(uint32_t);
        void setReadCallback(std::function<void()>); // 一个函数包装器，可以存储、复制和调用任何符合void()签名的可调用对象
        void setUseThreadPool(bool use = true);
};