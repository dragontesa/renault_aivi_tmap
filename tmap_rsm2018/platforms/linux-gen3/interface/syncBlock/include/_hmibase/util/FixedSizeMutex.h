/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_FIXEDSIZEMUTEX_H
#define HMIBASE_UTIL_FIXEDSIZEMUTEX_H

#include "hmibase/util/SimpleString.h"
#include <stddef.h>
#include <pthread.h>

namespace hmibase {
namespace util {

/** This class has a fixed size and can be used in memcpy. */
class FixedSizeMutex
{
   public:
      FixedSizeMutex();
      virtual ~FixedSizeMutex();

      void lock();
      bool tryLock();
      bool tryLock(int timeoutMilliseconds);
      void unlock();

   private:
      pthread_mutex_t mMutexStruct;

      SimpleString getErrorMessage(int errorCode) const;
};


} // namespace
} // namespace
#endif
