#ifndef MUDUO_TEST_ECHO_H
#define MUDUO_TEST_ECHO_H
#include "net/TcpServer.h"

class EchoServer
{
    public:
        EchoServer(muduo::net::EventLoop* loop , const muduo::net::InetAddress& listenaddr);

        void start();

    private:
        void onConnection(const muduo::net::TcpConnectionPtr& conn);

        void onMessage(const muduo::net::TcpConnectionPtr& conn , muduo::net::Buffer* buf , muduo::Timestamp time);

        muduo::net::TcpServer server_;

};

#endif