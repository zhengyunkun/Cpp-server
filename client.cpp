#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "util.h"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errIf(sockfd == -1, "socket create failed...");

    struct sockaddr_in server_addr; // 服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    // sockaddr和sockaddr_in大小相同，sockaddr_in有padding，都是16字节
    errIf(connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1, "socket connect failed...");
    /*
     *@sockfd: 套接字文件描述符
     *@server_addr: 服务器地址和端口等信息
     *@sizeof(server_addr): 信息长度
     */

    while (true)
    {
        char buf[1024];
        bzero(&buf, sizeof(buf));
        scanf("%s", buf); // 客户端阻塞等待输入
        ssize_t writeBytes = write(sockfd, buf, sizeof(buf));
        // 客户端发送数据: write -> 内核发送缓冲区 -> 网络协议栈 -> 网络传输
        if (writeBytes == -1)
        {
            printf("Socket already disconnected, cannot write anymore...");
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t readBytes = read(sockfd, buf, sizeof(buf));
        if (readBytes > 0)
        {
            printf("Message from server: %s\n", buf);
        }
        else if (readBytes == -1)
        {
            close(sockfd);
            errIf(true, "socket read failed...");
        }
    }

    close(sockfd);
    return 0;
}