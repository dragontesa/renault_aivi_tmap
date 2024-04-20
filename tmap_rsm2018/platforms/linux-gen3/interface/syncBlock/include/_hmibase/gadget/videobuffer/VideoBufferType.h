/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_VIDEOBUFFER_VIDEOBUFFERTYPE_H
#define HMIBASE_GADGET_VIDEOBUFFER_VIDEOBUFFERTYPE_H

#if defined(LinuxX86Make)

#include "hmibase/gadget/videobuffer/ShmBuffer.h"
namespace hmibase {
namespace gadget {
namespace videobuffer {
typedef hmibase::gadget::videobuffer::ShmBuffer VideoBufferTypeBase;
} // namespace
} // namespace
} // namespace


#elif defined(GEN3ARM)

#include "hmibase/gadget/videobuffer/DrmFdBuffer.h"
namespace hmibase {
namespace gadget {
namespace videobuffer {
typedef hmibase::gadget::videobuffer::DrmFdBuffer VideoBufferTypeBase;
} // namespace
} // namespace
} // namespace


#else

#include "hmibase/gadget/videobuffer/DummyBuffer.h"
namespace hmibase {
namespace gadget {
namespace videobuffer {
typedef hmibase::gadget::videobuffer::DummyBuffer VideoBufferTypeBase;
} // namespace
} // namespace
} // namespace


#endif


namespace hmibase {
namespace gadget {
namespace videobuffer {
class VideoBufferType : public hmibase::gadget::videobuffer::VideoBufferTypeBase
{
   public:
      VideoBufferType(uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp)
         : VideoBufferTypeBase(width, height, depth, bpp)
      {}
      virtual ~VideoBufferType()
      {}
};


} // namespace
} // namespace
} // namespace


#endif
