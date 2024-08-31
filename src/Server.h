#pragma once


#include <map>
#include <vector>

class EventLoop;
class Acceptor;
class Socket;
class Connection;
class ThreadPool;
class Server
{
    private:
        EventLoop*                      mainReactor;
        Acceptor*                       acceptor;
        std::map<int, Connection*>      connections; // 保存所有的连接, key是sockfd, value是Connection对象
        std::vector<EventLoop*>         subReactors;
        ThreadPool* threadPool;
    public:
        Server(EventLoop*);
        ~Server();

        void handleReadEvent(int);
        void newConnection(Socket* sock);
        void deleteConnection(int sockfd);
};