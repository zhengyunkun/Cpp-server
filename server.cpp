/* 
 * @Author: Zheng Yunkun
 * @Date: 2024-8-1 00:48:45
 * @LastEditTime: 2024-8-20 19:54:32
 * @LastEditors: Larry Kinn
 * @Description: Start up
 */

#include <sys/socket.h>
// 套接字接口
#include <arpa/inet.h>
// 绑定套接字到一个IP地址和接口上
#include <string.h>
#include <stdio.h>

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    /*
     *@AF_INET: IPv4 (Address Family Internet -- AF_INET)
     *@SOCK_STREAM: TCP (Stream socket) 表示流格式，适用于TCP协议
     *@0： 传输协议，根据前面的两个参数自动推导协议类型，设置为IPPROTO_TCP或者IPPTOTO_UDP，分别表示TCP和UDP，TCP是面向连接的协议，UDP无需连接
     */

    struct sockaddr_in server_addr;
    // 服务器地址结构体, socketaddr_in用于存储ipv4地址信息
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    // Give the socket FD the local address ADDR

    listen(sockfd, SOMAXCONN);
    // 监听这个socket端口，SOMAXCONN表示最大连接数

    // 接受一个客户端连接
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr); // accept需要写入客户端socket长度
    bzero(&client_addr, client_addr_len);
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

    // 接受完请求后执行
    printf("Client fd: %d, Client connected: %s, Port: %d...\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
}