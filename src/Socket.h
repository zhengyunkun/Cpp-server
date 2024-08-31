#pragma once


class InetAddress;
// 不需要加入InetAddress.h的头文件，因为只需要知道InetAddress这个类的存在，不需要知道其具体实现
// 只有需要具体实现的时候才需要包含头文件InetAddress.h
class Socket
{
    private:
        int fd;
    public:
        Socket();
        Socket(int);
        ~Socket();

        void bind(InetAddress*);
        void listen();
        void setNonBlocking();

        int accept(InetAddress*);
        void connect(InetAddress*);

        int getFd();
};