/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_VIDEOBUFFER_VIDEOBUFFER_H
#define HMIBASE_GADGET_VIDEOBUFFER_VIDEOBUFFER_H

#include <stdint.h>

namespace hmibase {
namespace gadget {
namespace videobuffer {

class VideoBuffer
{
   public:
      VideoBuffer(uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp);
      virtual ~VideoBuffer();

      virtual int32_t create() = 0;
      virtual void attach(int32_t id, uint32_t size) = 0;

      virtual uint8_t* getDataPtr() const = 0;
      virtual uint32_t getDataSize() const = 0;

      uint16_t getWidth() const;
      uint16_t getHeight() const;
      uint16_t getDepth() const;
      uint16_t getBpp() const;

   private:
      uint16_t mWidth;
      uint16_t mHeight;
      uint16_t mDepth;
      uint16_t mBpp;
};


} // namespace
} // namespace
} // namespace

#endif
