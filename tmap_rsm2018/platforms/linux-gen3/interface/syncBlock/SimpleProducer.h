/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_SYNCBLOCK2_SIMPLEPRODUCER_H
#define HMIBASE_GADGET_SYNCBLOCK2_SIMPLEPRODUCER_H

#include "platforms/linux-gen3/interface/syncBlock/include/hmibase/gadget/syncblock2/Producer.h"
#include "platforms/linux-gen3/interface/syncBlock/include/hmibase/gadget/videobuffer/VideoBuffer.h"

//#include "hmibase/gadget/syncblock2/Producer.h"
//#include "hmibase/gadget/videobuffer/VideoBuffer.h"

#include <unistd.h>
#include <map>
//#include <Producer.h>
//#include <VideoBuffer.h>

class SimpleProducer
{
   public:
      static const char* getVersion();
      static void initialize(int key, hmibase::gadget::syncblock2::ProducerType producerType = hmibase::gadget::syncblock2::SHM_PRODUCER);
      static void terminate();

      SimpleProducer(int instanceId, uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp);
      virtual ~SimpleProducer();

      const hmibase::gadget::syncblock2::BufferInfo& getBufferInfo() const;

      hmibase::gadget::videobuffer::VideoBuffer* exchange();
      hmibase::gadget::videobuffer::VideoBuffer* getBuffer();

      bool isTerminated();
      void startTermination();

   private:
      int mInstanceId;
      hmibase::gadget::syncblock2::BufferInfo mBufferInfo;
      std::map<int, hmibase::gadget::videobuffer::VideoBuffer*> mBuffers;

      static hmibase::gadget::syncblock2::Producer* smProducerPtr;
      int mNextBufferId = -1;
};


#endif
