/* ***************************************************************************************
* FILE:          Condition.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Condition.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __HMIBASE_UTIL_Condition_H_
#define __HMIBASE_UTIL_Condition_H_

#include "Macros.h"
#include <stdint.h>

namespace hmibase {
namespace util {

typedef int EventRef;

namespace internal {
class ConditionImpl
{
   public:
      virtual ~ConditionImpl() {}
      virtual bool signal(EventRef evRef) = 0;
      virtual EventRef wait(unsigned long timeout) = 0;
      virtual EventRef addEvent() = 0;
};


}

class Condition
{
   public:
      Condition();
      virtual ~Condition();
      bool signal(EventRef evRef = 1 /* 1...n */) const; // rerurn true if signaled
      EventRef wait(unsigned long timeout) const; // returns 1..n for any event, 0=timeout, -1= ERROR occurred
      EventRef addEvent() const; // returns 1..n for any event, 0=undefined, -1= ERROR occurred
   private:
      HMIBASE_UNCOPYABLE(Condition);
      internal::ConditionImpl* _impl;
};


} // namespace util
} // namespace hmibase

#endif // __HMIBASE_UTIL_Condition_H_
