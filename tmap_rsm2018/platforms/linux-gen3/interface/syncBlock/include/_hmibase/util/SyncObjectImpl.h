/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_SYNCOBJECTIMPL_H
#define HMIBASE_UTIL_SYNCOBJECTIMPL_H

#include "hmibase/util/SharedMemory.h"
#include "hmibase/util/FixedSizeMutex.h"
#include "hmibase/util/SimpleString.h"

namespace hmibase {
namespace util {

class SyncObjectImpl
{
   public:
      SyncObjectImpl(const char* name, const bool isMaster, int numBytes);
      virtual ~SyncObjectImpl();

      void  lock();
      void  unlock();
      void* getObjectPtr();

   private:
      SimpleString  mName;
      int           mNumBytes;
      bool          mIsMaster;
      SharedMemory* mSharedMemoryPtr;

      FixedSizeMutex* getMutexPtr();
      void*  getDataPtr();

      bool isConnected();
      void connect();
      void disconnect();

      // Thou shalt not copy
      SyncObjectImpl(const SyncObjectImpl&);
      SyncObjectImpl& operator=(const SyncObjectImpl&);
};


} // namespace
} // namespace
#endif
