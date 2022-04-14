cmake_minimum_required(VERSION 2.8)

project(server_test)


SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -pthread")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(
        server_test
        base/Timestamp.h
        base/Timestamp.cc

        base/FileUtil.h
        base/FileUtil.cc

        base/LogFile.h
        base/LogFile.cc

        base/Logging.h
        base/Logging.cc

        base/LogStream.h
        base/LogStream.cc

        base/AsyncLogging.h
        base/AsyncLogging.cc

        base/Condition.h
        base/Condition.cc

        base/CountDownLatch.h
        base/CountDownLatch.cc

        base/CurrentThread.h
        base/CurrentThread.cc

        base/Date.h
        base/Date.cc

        base/Exception.h
        base/Exception.cc
        
        base/ProcessInfo.h
        base/ProcessInfo.cc

        base/Thread.h
        base/Thread.cc

        base/ThreadPool.h
        base/ThreadPool.cc

        base/TimeZone.h
        base/TimeZone.cc


        net/Acceptor.h
        net/Acceptor.cc

        net/Buffer.h
        net/Buffer.cc

        net/Channel.h
        net/Channel.cc

        net/Connector.h
        net/Connector.cc

        net/EventLoop.h
        net/EventLoop.cc

        net/EventLoopThread.h
        net/EventLoopThread.cc

        net/EventLoopThreadPool.h
        net/EventLoopThreadPool.cc

        net/InetAddress.h
        net/InetAddress.cc

        net/Poller.h
        net/Poller.cc

        net/poller/DefaultPoller.cc

        net/poller/EPollPoller.h
        net/poller/EPollPoller.cc

        net/poller/PollPoller.h
        net/poller/PollPoller.cc

        net/Socket.h
        net/Socket.cc

        net/SocketsOps.h
        net/SocketsOps.cc

        net/TcpClient.h
        net/TcpClient.cc

        net/TcpConnection.h
        net/TcpConnection.cc

        net/TcpServer.h
        net/TcpServer.cc

        net/Timer.h
        net/Timer.cc

        net/TimerQueue.h
        net/TimerQueue.cc

        net/http/HttpServer.h
        net/http/HttpServer.cc

        net/http/HttpContext.h
        net/http/HttpContext.cc

        net/http/HttpRequest.h

        net/http/HttpResponse.h
        net/http/HttpResponse.cc

    

        net/http/test/HttpServer_test.cc
    )