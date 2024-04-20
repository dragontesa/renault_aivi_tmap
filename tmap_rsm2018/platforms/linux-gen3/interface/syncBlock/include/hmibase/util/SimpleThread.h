/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_SIMPLETHREAD_H
#define HMIBASE_UTIL_SIMPLETHREAD_H

#include "hmibase/util/SimpleString.h"
#include "hmibase/util/Threadable.h"

namespace hmibase {
namespace util {

class SimpleThreadImpl;

/**
This class offers a simpler implementation of thread functionality than hmibase::util::Thread.
While hmibase::util::Thread uses a semaphore to make sure that beforeRunning is complete before onStart is called,
this class simply starts a threaded function where onStart, onRun, onRun,..., onFinish are called.
*/
class SimpleThread
{
   public:
      SimpleThread(Threadable& threadable, const SimpleString& name = "UNNAMED");
      virtual ~SimpleThread();

      void start();
      void finish();
      bool isRunning();

      void sleep(unsigned int milliseconds);

   private:
      SimpleThreadImpl* mImplPtr;
};


} // namespace
} // namespace
#endif
