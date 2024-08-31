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
    channel->enableReading();
    channel->useET();
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);
    channel->setUseThreadPool(true);
    readBuffer = new Buffer();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::echo(int sockfd)
{
    char buf[READ_BUFFER];
    bool dataRead = false;
    // printf("Entering echo function for client fd %d\n", sockfd); // 添加调试信息
    while (true)
    {
        bzero(&buf, READ_BUFFER);
        ssize_t readBytes = read(sockfd, buf, READ_BUFFER);
        if (readBytes > 0)
        {
            // printf("Read %zd bytes from client fd %d\n", readBytes, sockfd);
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
            // errIf(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "Socket write failed...\n");
            send(sockfd);
            readBuffer->clear();
            break;
        }
        else if (readBytes == 0)
        {
            printf("Client fd %d disconnected...\n", sockfd);
            deleteConnectionCallback(sockfd);
            break;
        }
        else
        {
            printf("Connection reset by peer...\n");
            deleteConnectionCallback(sockfd);
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb)
{
    deleteConnectionCallback = _cb;
}

// send是用来将readBuffer中的数据发送给客户端的
void Connection::send(int sockfd)
{
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int data_size = readBuffer->size();
    int data_left = data_size;
    while (data_left > 0)
    {
        ssize_t writeBytes = write(sockfd, buf + data_size - data_left, data_left);
        if (writeBytes == -1 && errno == EAGAIN) break;
        data_left -= writeBytes;
    }
}