/* 
 * @Author: Zheng Yunkun
 * @Date: 2024-8-1 00:48:45
 * @LastEditTime: 2024-8-21 15:00:00
 * @LastEditors: Larry Kinn
 * @Description: Start up
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

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd)
{
    // fcntl获取当前文件状态标志
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK); // 添加非阻塞标志
    // 设置非阻塞
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*
     *@AF_INET: IPv4 (Address Family Internet -- AF_INET)
     *@SOCK_STREAM: TCP (Stream socket) 表示流格式，适用于TCP协议
     *@0： 传输协议，根据前面的两个参数自动推导协议类型，设置为IPPROTO_TCP或者IPPTOTO_UDP，分别表示TCP和UDP，TCP是面向连接的协议，UDP无需连接
     */
    errIf(sockfd == -1, "socket create failed...");

    struct sockaddr_in server_addr;
    // 服务器地址结构体, socketaddr_in用于存储ipv4地址信息
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    errIf((bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1), "socket bind failed...");
    // Give the socket FD the local address ADDR
    errIf((listen(sockfd, SOMAXCONN) == -1), "socket listen failed...");
    // 监听这个socket端口，SOMAXCONN表示最大连接数

    int epfd = epoll_create1(0); // epoll实例的文件描述符epfd
    errIf(epfd == -1, "epoll create failed...");

    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&ev, sizeof(ev));
    bzero(events, sizeof(events));

    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLET; // 当有数据可读时触发事件，边缘触发模式
    setnonblocking(sockfd); // 设置非阻塞，如果进行IO操作时不能立即完成，会返回
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

    while(true)
    {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); 
        // 把发生的事件放入events数组中，-1表示一直等待
        // 返回发生事件的文件描述符数量
        errIf(nfds == -1, "epoll wait failed...");
        for (int i = 0; i < nfds; i ++ )
        {
            if (events[i].data.fd == sockfd)  // 监听的sockfd上有新的连接请求
            {
                struct sockaddr_in client_addr;
                bzero(&client_addr, sizeof(client_addr));
                socklen_t client_addr_len = sizeof(client_addr);

                int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
                errIf(client_sockfd == -1, "socket accept failed...");
                printf("Client id: %d, Client connected: %s, Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                // 重新设置ev，将新的client_sockfd加入epoll监听
                bzero(&ev, sizeof(ev));
                ev.data.fd = client_sockfd;
                ev.events = EPOLLIN | EPOLLET; // 客户端有数据发送到本地服务器，套接字监听到可读事件
                setnonblocking(client_sockfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
            }
            else if (events[i].events & EPOLLIN) //是否是可读事件，有数据从客户端发送到服务器
            {
                char buf[READ_BUFFER];
                while (true)
                {
                    bzero(buf, sizeof(buf));
                    ssize_t readBytes = read(events[i].data.fd, buf, sizeof(buf));
                    // 内核为每个fd分配内核缓冲区，read从内核缓冲区读取数据到buf中
                    // read函数会阻塞，除非有数据可读或者客户端断开连接
                    if (readBytes > 0)
                    {
                        // 如果接受到了数据就把数据重新发送给客户端
                        printf("Message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, strlen(buf));
                    }
                    else if (readBytes == -1 && errno == EINTR) // 如果读取的过程中被信号中断，进入这里的处理逻辑
                    {
                        printf("Continue reading...\n");
                        continue;
                    }
                    else if (readBytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
                    // 没有读取到数据，EADAIN表示操作要重试，EWOULDBLOCK表示操作不能立即完成，需要等待
                    {
                        printf("Finish reading, errno: %d\n", errno);
                        break;
                    }
                    else if (readBytes == 0)
                    {
                        printf("EOF, client fd %d disconnected...\n", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }
            }
            else
            {
                printf("Unknown event...\n");
            }
        }
    }
    close(sockfd);
    return 0;
}