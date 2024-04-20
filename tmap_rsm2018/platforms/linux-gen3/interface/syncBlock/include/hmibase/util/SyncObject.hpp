/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_SYNCOBJECT_H
#define HMIBASE_UTIL_SYNCOBJECT_H

#include "hmibase/util/SyncObjectImpl.h"

namespace hmibase {
namespace util {

template <class PayloadType>
class SyncObject
{
   public:
      SyncObject(const char* name, const bool isMaster)
         : mImpl(name, isMaster, sizeof(PayloadType))
      {
         if (isMaster)
         {
            void* objectPtr = mImpl.getObjectPtr();
            new(objectPtr) PayloadType();
         }
      }

      virtual ~SyncObject()
      {
      }

      void lock()
      {
         mImpl.lock();
      }

      void unlock()
      {
         mImpl.unlock();
      }

      PayloadType* getPayloadPtr()
      {
         return (PayloadType*)(mImpl.getObjectPtr());
      }

   protected:
      SyncObjectImpl mImpl;
};


} // namespace
} // namespace
#endif
