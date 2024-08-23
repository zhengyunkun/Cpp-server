/* 
 * @Author: Zheng Yunkun
 * @Date: 2024-8-1 00:48:45
 * @LastEditTime: 2024-8-21 15:00:00
 * @LastEditors: Larry Kinn
 * @Description: Linux Server Start up
 */

#include <sys/socket.h>
// 套接字接口
#include <arpa/inet.h>
// 绑定套接字到一个IP地址和接口上
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include "util.h"
#include "Epoll.h"
#include "Socket.h"
#include "InetAddress.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setNonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void handleReadEvent(int);

int main()
{
    Socket* server_sock = new Socket();
    InetAddress* server_addr = new InetAddress("127.0.0.1", 8080);
    server_sock->bind(server_addr);
    server_sock->listen();

    Epoll* ep = new Epoll();
    server_sock->setNonBlocking();
    ep->addFd(server_sock->getFd(), EPOLLIN | EPOLLET);

    while (true)
    {
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for (int i = 0; i < nfds; i ++ )
        {
            if (events[i].data.fd == server_sock->getFd())
            // 新客户端连接
            {
                InetAddress* client_addr = new InetAddress();
                Socket* client_sock = new Socket(server_sock->accept(client_addr));
                printf("Client id: %d, Client connected: %s, Port: %d\n", client_sock->getFd(), inet_ntoa(client_addr->addr.sin_addr), ntohs(client_addr->addr.sin_port));
                client_sock->setNonBlocking();
                ep->addFd(client_sock->getFd(), EPOLLIN | EPOLLET);
            } 
            else if (events[i].events & EPOLLIN)
            // 可读事件
            {
                handleReadEvent(events[i].data.fd);
            }
            else
            {
                printf("Unknown event...\n");
            }
        }
    }

    delete server_addr;
    delete server_sock;

    return 0;
}

void handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    // 使用非阻塞IO，读取客户端buffer时，一次读取buf大小的数据直到全部读取完毕
    {
        bzero(&buf, sizeof(buf));
        ssize_t readBytes = read(sockfd, buf, sizeof(buf));
        if (readBytes > 0)
        {
            printf("Message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        }
        else if (readBytes == -1 && errno == EINTR)
        {
            printf("Continue reading\n");
            continue;
        }
        else if (readBytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        // 数据全部读取完毕
        {
            printf("Finish reading, errno: %d\n", errno);
            break;
        }
        else if (readBytes == 0)
        {
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd);
            break;
        }
    }
}