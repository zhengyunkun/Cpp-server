#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/Socket.h"

int main()
{
    Socket* sock = new Socket();
    InetAddress* addr = new InetAddress("127.0.0.1", 8080);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer* sendBuffer = new Buffer();
    Buffer* readBuffer = new Buffer();

    while (true)
    {
        sendBuffer->getline();
        ssize_t writeBytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if (writeBytes == -1)
        {
            printf("Socket already disconnected, cannot write anymore...");
            break;
        }
        int alreadyRead = 0;
        char buf[1037];
        while (true)
        {
            bzero(&buf, sizeof(buf));
            ssize_t readBytes = read(sockfd, buf, sizeof(buf));
            if (readBytes > 0)
            {
                readBuffer->append(buf, readBytes);
                alreadyRead += readBytes;
            }
            else if (readBytes == 0)
            {
                printf("Server disconnected...\n");
                exit(EXIT_FAILURE);
            }
            if (alreadyRead >= sendBuffer->size())
            {
                printf("Message from server: %s\n", buf);
                break;
            }
        }
        readBuffer->clear();
    }

    delete addr;
    delete sock;
    return 0;
}