/* ***************************************************************************************
* FILE:          FramedStream.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  FramedStream.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2018 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef FramedStream_UTIL_H
#define FramedStream_UTIL_H

#include "stddef.h"
#include "Macros.h"

namespace hmibase {
namespace util {

class FramedStream
{
   public:
      class IFramedStreamCallback
      {
         public:
            virtual bool onPacketReceived(unsigned char* buf, unsigned cnt) = 0;
            virtual int  onSendFramedData(unsigned char* buf, unsigned cnt) = 0;
      };

   public:
      FramedStream();
      virtual ~FramedStream();
      FramedStream(FramedStream::IFramedStreamCallback* fktPack);
      void addIncommingData(unsigned char* buf, int l);
      int  sendFramedPacket(const void* buffer, int length);

   private:
      struct PackRecvBuf
      {
         unsigned int  _len, _cnt, _esc;
         unsigned char _buf[1024];
         PackRecvBuf()
         {
            init();
         }
         void init();
      } _packbuf;
      HMIBASE_UNCOPYABLE(FramedStream);
      IFramedStreamCallback* _fktPack;
};


}//util
}//hmibase

#endif
