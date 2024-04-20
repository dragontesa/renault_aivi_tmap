/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_VIDEOBUFFER_DRMFDBUFFER_H
#define HMIBASE_GADGET_VIDEOBUFFER_DRMFDBUFFER_H

#include "hmibase/gadget/videobuffer/VideoBuffer.h"

namespace hmibase {
namespace gadget {
namespace videobuffer {

class ApxInstance;

class ApxContext;

class DrmFdBuffer: public VideoBuffer
{
   public:
      DrmFdBuffer(uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp);
      virtual ~DrmFdBuffer();

      virtual int32_t create();
      virtual void attach(int32_t fd, uint32_t size);

      virtual uint8_t* getDataPtr() const;
      virtual uint32_t getDataSize() const;

   private:
      ApxInstance* mApxPtr;
      int32_t      mFd;
      bool         mIsOwner;
      uint8_t*     mDataPtr;
      uint32_t     mDataSize;

      // Thou shalt not copy
      DrmFdBuffer(const DrmFdBuffer&);
      DrmFdBuffer& operator=(const DrmFdBuffer&);
};


} // namespace
} // namespace
} // namespace

#endif
