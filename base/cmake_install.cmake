# Install script for directory: /home/erfenjiao/MUDUO/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/erfenjiao/MUDUO/base/libmuduo_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/home/erfenjiao/MUDUO/base/AsyncLogging.h"
    "/home/erfenjiao/MUDUO/base/Atomic.h"
    "/home/erfenjiao/MUDUO/base/BlockingQueue.h"
    "/home/erfenjiao/MUDUO/base/BoundedBlockingQueue.h"
    "/home/erfenjiao/MUDUO/base/Condition.h"
    "/home/erfenjiao/MUDUO/base/CountDownLatch.h"
    "/home/erfenjiao/MUDUO/base/CurrentThread.h"
    "/home/erfenjiao/MUDUO/base/Date.h"
    "/home/erfenjiao/MUDUO/base/Exception.h"
    "/home/erfenjiao/MUDUO/base/FileUtil.h"
    "/home/erfenjiao/MUDUO/base/LogFile.h"
    "/home/erfenjiao/MUDUO/base/LogStream.h"
    "/home/erfenjiao/MUDUO/base/Logging.h"
    "/home/erfenjiao/MUDUO/base/Mutex.h"
    "/home/erfenjiao/MUDUO/base/ProcessInfo.h"
    "/home/erfenjiao/MUDUO/base/StringPiece.h"
    "/home/erfenjiao/MUDUO/base/Thread.h"
    "/home/erfenjiao/MUDUO/base/ThreadPool.h"
    "/home/erfenjiao/MUDUO/base/TimeZone.h"
    "/home/erfenjiao/MUDUO/base/Timestamp.h"
    "/home/erfenjiao/MUDUO/base/Types.h"
    "/home/erfenjiao/MUDUO/base/WeakCallback.h"
    "/home/erfenjiao/MUDUO/base/copyable.h"
    "/home/erfenjiao/MUDUO/base/noncopyable.h"
    "/home/erfenjiao/MUDUO/base/tempCodeRunnerFile.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/erfenjiao/MUDUO/base/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
