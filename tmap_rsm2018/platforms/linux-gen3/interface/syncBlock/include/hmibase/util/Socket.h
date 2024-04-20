/* ***************************************************************************************
* FILE:          Socket.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Socket.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef SOCKET_UTIL_H
#define SOCKET_UTIL_H

#include "stddef.h"

namespace hmibase {
namespace util {
namespace socket {

class Socket
{
   public:
      virtual ~Socket() {};

      virtual void close() = 0;

      virtual int send(const void* buffer, size_t length) = 0;
      virtual int receive(void* buffer, size_t length, bool waitUntilLengthBytesReceived) = 0;
};


class Acceptor
{
   public:
      virtual ~Acceptor() {}

      virtual void close() = 0;

      virtual Socket* accept() = 0;
};


class Manager
{
   public:
      virtual ~Manager() {}

      virtual Acceptor* bind(unsigned short port) = 0;
      virtual Socket* connect(const char* address, unsigned short port) = 0;

      static Manager& getInstance();
};


}//socket
}//util
}//hmibase


#endif
