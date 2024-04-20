/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_SIMPLETHREADIMPL_H
#define HMIBASE_UTIL_SIMPLETHREADIMPL_H

#ifdef WIN32
#include "hmibase/util/Mutex.h"
#else
#include "hmibase/util/FixedSizeMutex.h"
#endif

#include "hmibase/util/SimpleString.h"
#include "hmibase/util/Threadable.h"

namespace hmibase {
namespace util {

class SimpleThreadImpl
{
   public:
      SimpleThreadImpl(Threadable& threadable, const SimpleString& name);
      virtual ~SimpleThreadImpl();

      void start();
      void finish();
      bool isRunning();

   protected:
      virtual bool createThread() = 0;
      virtual bool joinThread() = 0;
      virtual void sleep(unsigned int milliseconds) = 0;

      void localThreadRun();
      const SimpleString& getName() const;

   private:
      SimpleString mName;
      Threadable&  mThreadable;
      bool mKeepRunning;

#ifdef WIN32
      Mutex mStateMutex;
#else
      FixedSizeMutex mStateMutex;
#endif
      enum State
      {
         NONE         = 0,
         INITIALIZING = 1,
         STARTING     = 2,
         RUNNING      = 3,
         FINISHING    = 4,
         FINISHED     = 5,
         JOINED       = 6
      } mState;

      State getState();
      void  setState(State state);

      SimpleString createMessage(const SimpleString& message);
};


} // namespace
} // namespace
#endif
