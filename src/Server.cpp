#include "Server.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "ThreadPool.h"
#include <unistd.h>
#include <functional>

#define READ_BUFFER 1024

Server::Server(EventLoop* _loop) : mainReactor(_loop), acceptor(nullptr)
{
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency(); // 获取CPU核心数
    threadPool = new ThreadPool(size);
    for (int i = 0; i < size; i ++ ) subReactors.push_back(new EventLoop());
    // 每个线程都是一个EventLoop
    for (int i = 0; i < size; i ++ )
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        threadPool->add(sub_loop);
        // 开启所有线程的事件循环
    }
}

Server::~Server()
{
    delete acceptor;
    delete threadPool;
}


// 针对本地服务器的新连接事件处理函数
void Server::newConnection(Socket* sock)
{   if (sock->getFd() != -1)
    {
        int random = sock->getFd() % subReactors.size();
        // 随机调度策略
        Connection* conn = new Connection(subReactors[random], sock);
        std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectionCallback(cb);
        connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(int sockfd)
{
    if (sockfd != -1)
    {
        auto it = connections.find(sockfd);
        if (it != connections.end())
        {
            Connection* conn = connections[sockfd];
            connections.erase(sockfd);
            delete conn;
        }
    }
}
