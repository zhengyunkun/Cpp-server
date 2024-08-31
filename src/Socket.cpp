#include "Socket.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>   // 提供了控制文件描述符的函数
#include <sys/socket.h>
#include <strings.h>

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
        fd = -1;
    }
}

void Socket::bind(InetAddress* _addr)
{
    struct sockaddr_in addr = _addr->getAddr();
    errIf(::bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "Socket bind failed...");
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

int Socket::accept(InetAddress* _addr)
{
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr);
    int client_sockfd = ::accept(fd, (sockaddr*)&addr, &addr_len);
    errIf(client_sockfd == -1, "Socket accept failed...");
    _addr->setInetAddr(addr);
    return client_sockfd;
}

void Socket::connect(InetAddress* _addr)
{
    struct sockaddr_in addr = _addr->getAddr();
    errIf(::connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1, "Socket connect failed...");
}

int Socket::getFd()
{
    return fd;
}

InetAddress::InetAddress()
{
    bzero(&addr, sizeof(addr));
}

InetAddress::InetAddress(const char* ip, uint16_t port)
{
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);  // 将10进制的ip地址字符串转换成二进制的网络字节序
    addr.sin_port = htons(port);           // 将16位无符号整数转换为网络字节序
}

InetAddress::~InetAddress()
{
}

void InetAddress::setInetAddr(sockaddr_in _addr)
{
    addr = _addr;
}

sockaddr_in InetAddress::getAddr()
{
    return addr;
}