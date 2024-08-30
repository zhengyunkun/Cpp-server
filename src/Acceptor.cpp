#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <stdio.h>

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop)
{
    server_sock = new Socket();
    InetAddress* server_addr = new InetAddress("127.0.0.1", 8080);
    server_sock->bind(server_addr);
    server_sock->listen();
    server_sock->setNonBlocking();

    acceptChannel = new Channel(loop, server_sock->getFd());
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);    // this 表示是当前成员的成员函数

    acceptChannel->setCallback(cb);
    acceptChannel->enableReading();
    delete server_addr;
}

Acceptor::~Acceptor()
{
    delete server_sock;
    delete acceptChannel;
}

void Acceptor::acceptConnection()
{
    InetAddress *client_addr = new InetAddress();
    int client_fd = server_sock->accept(client_addr);
    Socket* client_sock = new Socket(client_fd);

    char* clientIp = inet_ntoa(client_addr->addr.sin_addr);
    int clientPort = ntohs(client_addr->addr.sin_port);
    printf("Client id: %d, Client connected: %s, Port: %d\n", 
            client_sock->getFd(), clientIp, clientPort);
    client_sock->setNonBlocking();
    newConnectionCallback(server_sock);
    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb)
{
    newConnectionCallback = _cb;
}
