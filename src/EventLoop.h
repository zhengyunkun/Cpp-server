#pragma

#include <functional>

class Epoll;
class Channel;
class EventLoop     
// 事件循环类，添加需要监听的Channel，然后调用loop()函数开始监听
// 如果有事件发生就通知返回Channel，然后根据不同的描述符、事件类型进行处理
{
    private:
        Epoll* ep;
        bool quit;
    public:
        EventLoop();
        ~EventLoop();

        void loop();    // 事件驱动关键函数
        void updateChannel(Channel*);
        void addThread(std::function<void()>);
};