/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_TIMEOUT_H
#define HMIBASE_UTIL_TIMEOUT_H

namespace hmibase {
namespace util {

class Timeout
{
   public:
      Timeout(unsigned int milliseconds);
      virtual ~Timeout();

      bool isReached() const;

   private:
      unsigned long long mEnd;
      unsigned long long getCurrentMilliseconds() const;
};


} // namespace
} // namespace
#endif
