#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock), channel(nullptr), inBuffer(new std::string()), readBuffer(nullptr)
{
    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setCallback(cb);
    channel->enableReading();
    readBuffer = new Buffer();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd)
{
    char buf[READ_BUFFER];
    bool dataRead = false;
    while (true)
    {
        bzero(&buf, READ_BUFFER);
        ssize_t readBytes = read(sockfd, buf, READ_BUFFER);
        if (readBytes > 0)
        {
            readBuffer->append(buf, readBytes);
            dataRead = true;
        }
        else if (readBytes == -1 && errno == EINTR)
        {
            printf("Continuing reading...\n");
            continue;
        }
        else if (readBytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            if (!dataRead) printf("Finish reading, errno: %d\n", errno);
            printf("Message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            errIf(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "Socket write failed...\n");
            readBuffer->clear();
            break;
        }
        else if (readBytes == 0)
        {
            printf("Client fd %d disconnected...\n", sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> _cb)
{
    deleteConnectionCallback = _cb;
}