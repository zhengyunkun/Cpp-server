#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>   // 提供了控制文件描述符的函数
#include <sys/socket.h>

Socket::Socket() : fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errIf(fd == -1, "Socket create failed...");
}

Socket::Socket(int _fd) : fd(_fd)
{
    errIf(fd == -1, "Socket create failed...");
}

Socket::~Socket()
{
    // 检查fd是否有效，保证在Socket对象销毁时关闭文件描述符
    if (fd != -1)
    {
        close(fd);
    }
}

void Socket::bind(InetAddress* server_addr)
{
    errIf(::bind(fd, (sockaddr*)&server_addr->addr, server_addr->addr_len) == -1, "Socket bind failed...");
}

// ::bind表示调用全局命名空间的bind函数，而不是Socket类的bind函数
// bind函数传入地址的len是为了确保访问的实际地址结构体大小不会超出内存区域，避免出现段错误

void Socket::listen()
{
    errIf(::listen(fd, SOMAXCONN) == -1, "Socket listen failed...");
}

void Socket::setNonBlocking()
{
    fcntl(fd, F_SETFL, (fcntl(fd, F_GETFL) | O_NONBLOCK));
}

int Socket::accept(InetAddress* client_addr)
{
    int client_fd = ::accept(fd, (sockaddr*)&client_addr->addr, &client_addr->addr_len);
    errIf(client_fd == -1, "Socket accept failed...");
    return client_fd;
}

int Socket::getFd()
{
    return fd;
}