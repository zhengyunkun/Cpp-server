#pragma once


#include <map>

class EventLoop;
class Acceptor;
class Socket;
class Connection;
class Server
{
    private:
        EventLoop* loop;
        Acceptor* acceptor;
        std::map<int, Connection*> connections; // 保存所有的连接, key是sockfd, value是Connection对象
    public:
        Server(EventLoop*);
        ~Server();

        void handleReadEvent(int);
        void newConnection(Socket* sock);
        void deleteConnection(int sockfd);
};