/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_DLTLOGGER_H
#define HMIBASE_UTIL_DLTLOGGER_H

#include "hmibase/util/DltName.h"
#include "hmibase/util/SimpleString.h"
#include <map>

namespace hmibase {
namespace util {

class DltLogger
{
   public:
      static DltLogger& getInstance(const DltName& name, const char* description);
      static void destroyAllInstances();

      void debug(const SimpleString& str);
      void info(const SimpleString& str);
      void warning(const SimpleString& str);
      void error(const SimpleString& str);
      void fatal(const SimpleString& str);

      enum LogLevel
      {
         LL_FATAL,
         LL_ERROR,
         LL_WARNING,
         LL_INFO,
         LL_DEBUG
      };
      void log(const LogLevel logLevel, const SimpleString& str);

   private:
      static std::map<DltName, DltLogger*> smInstancePtrs;

      DltLogger(const DltName& name, const char* description);
      virtual ~DltLogger();

      void* mDltContextPtr;

      DltName      mName;
      SimpleString mDescription;
};


} // namespace
} // namespace

#endif
