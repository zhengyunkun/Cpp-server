#include "Server.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Acceptor.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* _loop) : loop(_loop), acceptor(nullptr)
{
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete acceptor;
}

// 针对客户端的读事件处理函数
void Server::handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    bool dateRead = false;
    while (true)
    {
        bzero(&buf, READ_BUFFER);
        ssize_t readBytes = read(sockfd, buf, READ_BUFFER);
        if (readBytes > 0)
        {
            printf("Message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, READ_BUFFER);
            dateRead = true;
        }
        else if (readBytes == -1 && errno == EINTR)
        {
            printf("Continuing reading...\n");
            continue;
        }
        else if (readBytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            if(!dateRead) printf("Finish reading, errno: %d\n", errno);
            break;
        }
        else if (readBytes == 0)
        {
            printf("Client fd %d disconnected...\n", sockfd);
            close(sockfd);
            break;
        }
    }
}

// 针对本地服务器的新连接事件处理函数
void Server::newConnection(Socket* server_sock)
{
    InetAddress* client_addr = new InetAddress();
    int client_fd = server_sock->accept(client_addr);
    Socket* client_sock = new Socket(client_fd);
    client_sock->setNonBlocking();

    char* clientIp = inet_ntoa(client_addr->addr.sin_addr);
    int clientPort = ntohs(client_addr->addr.sin_port);
    printf("Client id: %d, Client connected: %s, Port: %d\n", 
            client_sock->getFd(), clientIp, clientPort);


    Channel* clientChannel = new Channel(loop, client_sock->getFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, client_sock->getFd());
    clientChannel->setCallback(cb);
    clientChannel->enableReading();
}
