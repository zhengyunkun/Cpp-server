#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

Socket::Socket() : fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errIf(fd == -1, "socket create failed...");
}

Socket::Socket(int _fd) : fd(_fd)
{
    errIf(fd == -1, "socket create failed...");
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress* addr)
{
    errIf(::bind(fd, (sockaddr*)&addr->addr, addr->addr_len) == -1, "socket bind failed...");
}

void Socket::listen()
{
    errIf(::listen(fd, SOMAXCONN) == -1, "socket listen failed...");
}

void Socket::setNonbinding()
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr)
{
    int client_sockfd = ::accept(fd, (sockaddr*)&addr->addr, &addr->addr_len);
    errIf(client_sockfd == -1, "socket accept failed...");
    return client_sockfd;
}

int Socket::getFd()
{
    return fd;
}