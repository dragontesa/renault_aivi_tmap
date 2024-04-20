/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_VIDEOBUFFER_SHMBUFFER_H
#define HMIBASE_GADGET_VIDEOBUFFER_SHMBUFFER_H

#include "hmibase/gadget/videobuffer/VideoBuffer.h"
#include "hmibase/util/SimpleString.h"
#include "hmibase/util/SharedMemory.h"

using namespace hmibase::util;


namespace hmibase {
namespace gadget {
namespace videobuffer {

class ShmBuffer: public VideoBuffer
{
   public:
      ShmBuffer(uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp);
      virtual ~ShmBuffer();

      virtual int32_t create();
      virtual void attach(int32_t id, uint32_t size);

      virtual uint8_t* getDataPtr() const;
      virtual uint32_t getDataSize() const;

   private:
      static int32_t smNextId;
      static SS      getName(int32_t id);

      int32_t       mId;
      SharedMemory* mShmPtr;
};


} // namespace
} // namespace
} // namespace

#endif
