#include "Socket.h"

#include "../base/Logging.h"
#include "InetAddress.h"
#include "SocketsOps.h"

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>  // snprintf

using namespace muduo;
using namespace muduo::net;

Socket::~Socket()
{
    sockets::close(sockfd_);
}

bool sockets::get()