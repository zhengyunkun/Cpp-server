#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop)
{
    server_sock = new Socket();
    server_addr = new InetAddress("127.0.0.1", 8080);
    server_sock->bind(server_addr);
    server_sock->listen();
    server_sock->setNonBlocking();

    acceptChannel = new Channel(loop, server_sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);    // this 表示是当前成员的成员函数

    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
}

Acceptor::~Acceptor()
{
    delete server_sock;
    delete server_addr;
    delete acceptChannel;
}

void Acceptor::acceptConnection()
{
    newConnectionCallback(server_sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb)
{
    newConnectionCallback = _cb;
}
