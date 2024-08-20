#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "util.h"

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errIf(sockfd == -1, "socket create failed...");

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    errIf(connect(sockfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1, "socket connect failed...");

    while (true)
    {
        char buf[1024];
        bzero(&buf, sizeof(buf));
        scanf("%s", buf);
        ssize_t writeBytes = write(sockfd, buf, sizeof(buf));
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