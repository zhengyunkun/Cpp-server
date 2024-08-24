#pragma once

class EventLoop;
class Acceptor;
class Socket;
class Server
{
    private:
        EventLoop* loop;
        Acceptor* acceptor;
    public:
        Server(EventLoop*);
        ~Server();

        void handleReadEvent(int);
        void newConnection(Socket* server_sock);
};