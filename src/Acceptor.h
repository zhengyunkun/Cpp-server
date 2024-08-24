#pragma once


#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

// 对于每一个事件，不管提供什么样的服务，都首先要调用accept()函数接受TCP连接
// 然后将socket文件描述符添加到epoll中，当这个IO口有事件发生的时候再对此TCP连接提供相应的服务
// 新建的连接逻辑在Acceptor中完成，Acceptor只负责接受新的连接，然后将新的连接交给Server类处理
class Acceptor
{
    private:
        EventLoop* loop;
        Socket* server_sock;
        InetAddress* server_addr;
        Channel* acceptChannel;
    
    public:
        Acceptor(EventLoop* _loop);
        ~Acceptor();
        void acceptConnection();

        // 新建连接的回调函数
        std::function<void(Socket*)> newConnectionCallback;
        void setNewConnectionCallback(std::function<void(Socket*)>); // 新建连接的工作仍然由Server类来完成
};