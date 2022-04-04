#include "SocketsOps.h"

#include "../base/Logging.h"
#include "../base/Types.h"
#include "Endian.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>  // snprintf
#include <sys/socket.h>
#include <sys/uio.h>  // readv
#include <unistd.h>

using namespace muduo;
using namespace muduo::net;

namespace 
{
    typedef struct sockaddr SA;
    #if VALGRIND || defined (NO_ACCEPT4)
    void setNonBlockAndCloseOnExec(int sockfd)
    {
    // non-block
    int flags = ::fcntl(sockfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockfd, F_SETFL, flags);
    // FIXME check

    // close-on-exec
    flags = ::fcntl(sockfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockfd, F_SETFD, flags);
    // FIXME check

    (void)ret;
    }
    #endif
}
