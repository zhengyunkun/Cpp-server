#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <functional>
#include <string.h>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* _loop) : loop(_loop)
{
    Socket* server_sock = new Socket();
    InetAddress* server_addr = new InetAddress("127.0.0.1", 8080);
    server_sock->bind(server_addr);
    server_sock->listen();
    server_sock->setNonBlocking();

    Channel* serverChannel = new Channel(loop, server_sock->getFd());
    std::function<void()> cb = std::bind(&Server::newConnection, this, server_sock);
    // 将Server类的newConnection函数绑定到当前对象this和server_sock上
    serverChannel->setCallback(cb);
    serverChannel->enableReading();
}

Server::~Server()
{
}

void Server::handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    {
        bzero(&buf, READ_BUFFER);
        ssize_t readBytes = read(sockfd, buf, READ_BUFFER);
        if (readBytes > 0)
        {
            // printf("Client id: %d, Client connected: %s, Port: %d\n", );
        }
    }
}
