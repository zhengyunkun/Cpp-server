#include "InetAddress.h"
#include <string.h>

InetAddress::InetAddress() : addr_len(sizeof(addr)) // 初始化成员列表
{
    bzero(&addr, sizeof(addr));
}

InetAddress::InetAddress(const char* ip, uint16_t port) : addr_len(sizeof(addr))
{
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip); //10进制ip地址转换为网络字节序
    addr.sin_port = htons(port); //16位主机字节序转换为网络字节序
    addr_len = sizeof(addr);
}

InetAddress::~InetAddress()
{
}
