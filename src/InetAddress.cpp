#include "InetAddress.h"
#include <string.h>

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