/* 
 * @Author: Zheng Yunkun
 * @Date: 2024-8-1 00:48:45
 * @LastEditTime: 2024-8-21 15:00:00
 * @LastEditors: Larry Kinn
 * @Description: Linux Server Start up
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "Socket.h"
#include "Channel.h"
#include "InetAddress.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void handleReadEvent(int);

int main()
{
    Socket* server_sock = new Socket(); // 调用Socket的构造函数，创建一个socket对象
    InetAddress* server_addr = new InetAddress("127.0.0.1", 8080);

    server_sock->bind(server_addr);
    server_sock->listen();
    server_sock->setNonBlocking();

    Epoll* ep = new Epoll();
    // ep->addFd(server_sock->getFd(), EPOLLIN | EPOLLET);
    Channel* serverChannel = new Channel(ep, server_sock->getFd());
    serverChannel->enableReading();

    while (true)
    {
        std::vector<Channel*> activeChannels = ep->poll();
        int nfds = activeChannels.size();

        for (int i = 0; i < nfds; i ++ )
        {
            int chfd = activeChannels[i]->getFd();
            if (chfd == server_sock->getFd())
            {
                InetAddress* client_addr = new InetAddress();
                int client_fd = server_sock->accept(client_addr);
                Socket* client_sock = new Socket(client_fd);
                client_sock->setNonBlocking();

                char* clientIp = inet_ntoa(client_addr->addr.sin_addr);
                int clientPort = ntohs(client_addr->addr.sin_port);
                printf("Client id: %d, Client connected: %s, Port: %d\n", 
                        client_sock->getFd(), clientIp, clientPort);
                
                // 引入Channel类，将client_sock的fd和epoll进行绑定，封装部分epoll的操作
                Channel* clientChannel = new Channel(ep, client_sock->getFd());
                clientChannel->enableReading();
            }
            else if (activeChannels[i]->getRevents() & EPOLLIN)
            {
                handleReadEvent(chfd);
            }
            else
            {
                printf();
            }
        }
    }
    
    delete server_sock;
    delete server_addr;
    delete ep;
    return 0;
}

void handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    bool dataRead = false;
    while (true)
    {
        bzero(&buf, sizeof(buf));
        ssize_t readBytes = read(sockfd, buf, sizeof(buf));
        if (readBytes > 0)
        {
            printf("Message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
            bool dataRead = true;
        }
        else if (readBytes == -1 && errno == EINTR) // 被外部信号中断，继续读取
        {
            printf("Continuing reading...\n");
            continue;
        }
        else if (readBytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            if (!dataRead) printf("Finish reading, errno: %d\n", errno);
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