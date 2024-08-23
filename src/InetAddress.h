#pragma once


#include <arpa/inet.h>

class InetAddress
{
    public:
        struct sockaddr_in addr;
        socklen_t addr_len;
        InetAddress();
        InetAddress(const char* ip, uint16_t port);  
        // uint16_t是16位无符号整数，范围0x0000-0xFFFF正好用来表示端口
        // _t表示C/C++中的标准类型
        ~InetAddress();
};