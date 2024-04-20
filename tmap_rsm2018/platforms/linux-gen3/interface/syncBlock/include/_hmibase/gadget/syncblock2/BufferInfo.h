/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_SYNCBLOCK2_BUFFERINFO_H
#define HMIBASE_GADGET_SYNCBLOCK2_BUFFERINFO_H

#include <stdint.h>

namespace hmibase {
namespace gadget {
namespace syncblock2 {

class BufferInfo
{
   public:
      BufferInfo();
      BufferInfo(const BufferInfo& right);
      BufferInfo(uint32_t size, uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp);
      virtual ~BufferInfo();

      bool operator==(const BufferInfo& right) const;
      bool operator!=(const BufferInfo& right) const;

      BufferInfo& operator=(const BufferInfo& right);

      uint32_t getSize() const;
      uint16_t getWidth() const;
      uint16_t getHeight() const;
      uint16_t getDepth() const;
      uint16_t getBpp() const;

   private:
      uint32_t mSize;
      uint16_t mWidth;
      uint16_t mHeight;
      uint16_t mDepth;
      uint16_t mBpp;
};


} // namespace
} // namespace
} // namespace

#endif
