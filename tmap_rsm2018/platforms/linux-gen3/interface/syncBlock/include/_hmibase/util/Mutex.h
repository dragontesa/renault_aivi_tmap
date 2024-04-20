/* ***************************************************************************************
* FILE:          Mutex.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Mutex.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __HMIBASE_UTIL_MUTEX_H_
#define __HMIBASE_UTIL_MUTEX_H_

#include "Macros.h"
#include <stdint.h>
#include <string>

namespace hmibase {
namespace util {

class ScopedMutex;

namespace internal {
class MutexImpl
{
   public:
      virtual ~MutexImpl() {}
      virtual bool lock() = 0;
      virtual void unlock() = 0;
};


}

class Mutex
{
      friend class ScopedMutex;
   public:
      Mutex(bool processShared = false);
      virtual ~Mutex();
      bool lock() const;
      void unlock() const;
   private:
      HMIBASE_UNCOPYABLE(Mutex);
      internal::MutexImpl* _impl;
};


class ScopedMutex
{
   public:
      explicit ScopedMutex(Mutex& cs);
      ~ScopedMutex();
      void unlock();
   private:
      Mutex* _pcs;
};


/////////////////////////////////////////////////////////////////////////////////////

typedef Mutex CriticalSection;
typedef ScopedMutex ScopedCriticalSection;
}


}
#endif // __HMIBASE_UTIL_MUTEX_H_
