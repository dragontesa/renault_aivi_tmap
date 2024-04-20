/* ***************************************************************************************
* FILE:          Thread.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Thread.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __HMIBASE_UTIL_THREAD_H_
#define __HMIBASE_UTIL_THREAD_H_

#include <stdint.h>
#include <string>
#include "Condition.h"
#include "Mutex.h"

namespace hmibase {
namespace util {
namespace internal {
class ThreadImpl;
}


class Thread
{
   public:
      class IThreadFunction
      {
         public:

            const EventRef wait_timeout;
            const EventRef wait_error;

            IThreadFunction(): wait_timeout(0), wait_error(-1), _shouldLeave(false)
            {
            }
            /*
                        virtual ~IThreadFunction()
                        {
                        }
            */
            // called before sync semaphore is posted
            virtual void beforeRunning() {};

            // called directly before thread func will start, no further sync involved
            virtual void onStart() {}

            // the thread func
            virtual bool threadWorkProc() = 0;

            // called after thread func ended
            virtual void onTerminate() {}

            virtual bool isRunning() const
            {
               return _shouldLeave == false;
            }
            EventRef wait(unsigned long millisecs)
            {
               return _cond.wait(millisecs);
            }
            EventRef sleep()
            {
               return wait(0xFFFFFFFF);
            }
            bool sleepInterupt() const
            {
               return _cond.signal();
            }

         public:
            void setThreadState(bool leaveState)
            {
               _shouldLeave = leaveState;
               if (leaveState)
               {
                  sleepInterupt();
               }
            }
         private:
            bool _shouldLeave;
            hmibase::util::Condition _cond;
      };
   public:
      Thread();
      Thread(Thread::IThreadFunction* fktThread, uint32_t nStackSize, const char* name);
      virtual ~Thread();
      bool create(Thread::IThreadFunction* fktThread, uint32_t nStackSize = 0, const char* name = NULL);
      bool leave(uint32_t dwTimeoutMsec = 5);
      bool join();
      void setName(const char* name);
   public:
      static void sleep(unsigned long millisecs);
   private:
      void terminate();
      void setImplementation();
      HMIBASE_UNCOPYABLE(Thread);
      ::hmibase::util::internal::ThreadImpl* _impl;
};


namespace internal {
class ThreadImpl
{
   public:
      virtual ~ThreadImpl() {}
      virtual bool create(Thread::IThreadFunction* fktThread, uint32_t nStackSize) = 0;
      virtual bool leave(uint32_t dwTimeoutMsec) = 0;
      virtual bool join() = 0;
      virtual void setName(const char* name) = 0;
      virtual void terminate() = 0;;
};


}
}


}

#endif // __HMIBASE_UTIL_THREAD_H_
