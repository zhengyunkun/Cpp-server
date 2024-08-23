#pragma once

class EventLoop;
class Socket;
class Server
{
    private:
        EventLoop* loop;
    public:
        Server(EventLoop* loop);
        ~Server();

        void handleReadEvent(int);
        void newConnection(Socket* server_sock);
};