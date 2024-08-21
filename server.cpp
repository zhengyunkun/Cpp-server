/* 
 * @Author: Zheng Yunkun
 * @Date: 2024-8-1 00:48:45
 * @LastEditTime: 2024-8-21 19:28:32
 * @LastEditors: Larry Kinn
 * @Description: Start up
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*
     *@AF_INET: IPv4 (Address Family Internet -- AF_INET)
     *@SOCK_STREAM: TCP (Stream socket) 表示流格式，适用于TCP协议
     *@0： 使用默认传输协议
     */
    errIf(sockfd == -1, "socket create failed...");

    struct sockaddr_in server_addr;
    // 服务器地址结构体, socketaddr_in用于存储ipv4地址信息
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080); // 端口号让同一网络设备能够同时处理多个网络连接

    errIf((bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1), "socket bind failed...");
    // Give the socket FD the local address ADDR
    errIf((listen(sockfd, SOMAXCONN) == -1), "socket listen failed...");
    // 操作系统会分配一个连接队列，大小是SOMAXCONN，当有连接请求到达时，内核会将其放入队列中

    // 接受一个客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr); // accept需要写入客户端socket长度
    bzero(&client_addr, client_addr_len);
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    // client_sockfd表示一个已经建立连接的客户端
    errIf(client_sockfd == -1, "socket accept failed...");

    // 接受完请求后执行
    printf("Client fd: %d, Client connected: %s, Port: %d...\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // 当我们建立socket连接之后，就可以使用<unistd.h>中的read和write进行网络接口的数据读写操作
    // read write
    while(true)
    {
        char buf[1024];
        // Define buffer
        bzero(&buf, sizeof(buf)); // 清空缓冲区
        ssize_t readBytes = read(client_sockfd, buf, sizeof(buf));
        // 网络传输 -> 内核缓冲区 -> read

        if (readBytes > 0)
        {
            printf("Message from client fd %d: %s\n", client_sockfd, buf);
            write(client_sockfd, buf, sizeof(buf));
            // 将相同的数据写入客户端
        }
        else if (readBytes == 0)
        {
            printf("Client fd %d disconnected...\n", client_sockfd);
            close(client_sockfd);
            break;
        } else if (readBytes == -1)
        {
            close(client_sockfd);
            errIf(true, "socket read failed...");
        }
    }
    close(sockfd);
    return 0;
}