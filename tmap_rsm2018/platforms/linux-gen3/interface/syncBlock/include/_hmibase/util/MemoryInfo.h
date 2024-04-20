/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_MEMORYINFO_H
#define HMIBASE_UTIL_MEMORYINFO_H

namespace hmibase {
namespace util {

class MemoryInfoImpl;

class MemoryInfo
{
   public:
      enum Unit
      {
         BYTE       = 0,
         KILO_BYTE  = 1,
         MEGA_BYTE  = 2,
         GIGA_BYTE  = 3,
      };

      MemoryInfo();
      virtual ~MemoryInfo();

      void refresh();

      unsigned long long getTotalMemory(Unit unit = BYTE) const;
      unsigned long long getTotalUsedMemory(Unit unit = BYTE) const;
      unsigned long long getProcessUsedMemory(Unit unit = BYTE) const;

   private:
      MemoryInfoImpl* mImplPtr;

      unsigned long long calcUnits(unsigned long long bytes, Unit unit) const;
};


} // namespace
} // namespace
#endif
