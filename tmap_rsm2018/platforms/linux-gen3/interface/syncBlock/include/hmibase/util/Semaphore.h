/* ***************************************************************************************
* FILE:          Semaphore.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Semaphore.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __HMIBASE_UTIL_SEMAPHORE_H_
#define __HMIBASE_UTIL_SEMAPHORE_H_

#include "Macros.h"
#include <stdint.h>
#include <string>

namespace hmibase {
namespace util {

namespace internal {
class SemaphoreImpl
{
   public:
      virtual ~SemaphoreImpl() {}
      virtual bool create(unsigned int nCount, const char* name) = 0;
      virtual void destroy() = 0;
      virtual bool wait() = 0;
      virtual bool wait(unsigned long timeout) = 0;
      virtual bool waitTry() = 0;
      virtual bool post() = 0;
};


}

class Semaphore
{
   public:
      /** Create an instance for a semaphore. */
      Semaphore();
      Semaphore(unsigned int nCount, const char* name);

      virtual ~Semaphore();

      bool create(unsigned int nCount, const char* name);
      void destroy();
      /** Decrements the value of the semaphore. The value of a semaphore cannot be negative.
          If it is 0 and wait is called, the method will wait until some other instance has incremented the value. */
      bool wait();
      bool wait(unsigned long timeout);
      bool waitTry();
      /** increment, release one's share of the semaphore's count */
      bool post();
      /** If process was killed and the destructor of the master instance was not called,
        * the created semaphore will continue to exist (in /dev/shm).
        * Purge will clean up the garbage semaphore from a previous process run. */
      static void purge(const char* name);
   private:
      void setImplementation();
      HMIBASE_UNCOPYABLE(Semaphore);
      ::hmibase::util::internal::SemaphoreImpl* _impl;
};


}
}


#endif // __HMIBASE_UTIL_SEMAPHORE_H_
