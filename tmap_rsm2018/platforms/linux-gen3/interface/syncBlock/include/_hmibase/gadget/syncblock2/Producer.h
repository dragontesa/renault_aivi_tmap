/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_SYNCBLOCK2_PRODUCER_H
#define HMIBASE_GADGET_SYNCBLOCK2_PRODUCER_H

#include "hmibase/gadget/syncblock2/BufferInfo.h"
#include "hmibase/gadget/syncblock2/ProducerType.h"
#include <map>


namespace hmibase {
namespace gadget {
namespace syncblock2 {

class ProducerInst;

class Producer
{
   private:
      int mKey;
      int mCurrentId;
      ProducerType mProducerType;
      std::map<int, ProducerInst*> mInstPtrs;

      void repair();

   public:
      static const char* getVersion();

      Producer(int key, ProducerType producerType = SHM_PRODUCER);
      virtual ~Producer();

      bool attach(int id);
      int exchange();

      void setBufferInfo(const BufferInfo& bufferInfo);

      void addBufHandle(unsigned int bufNum, int bufHandle);
      int  getBufHandle(unsigned int bufNum);

      bool isTerminated();
      bool startTermination();
      bool checkTermination();
};


} // namespace
} // namespace
} // namespace
#endif
