/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_THREADABLE_H
#define HMIBASE_UTIL_THREADABLE_H

namespace hmibase {
namespace util {


/**
Classes deriving from Threadable can be passed to a SimpleThread instance.
Its thread function will call onStart, onRun,...,onRun, onFinish.
The onRun function  will be called until it returns false, or the Thread itself wants to terminate.
*/
class Threadable
{
   public:
      Threadable();
      virtual ~Threadable();
      virtual void onStart(const bool& keepRunning);
      virtual bool onRun(const bool& keepRunning);
      virtual void onFinish(const bool& keepRunning);
};


} // namespace
} // namespace
#endif
