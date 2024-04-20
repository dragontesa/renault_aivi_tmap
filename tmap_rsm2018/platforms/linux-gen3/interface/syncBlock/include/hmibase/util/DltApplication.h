/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_DLTAPPLICATION_H
#define HMIBASE_UTIL_DLTAPPLICATION_H

#include "hmibase/util/DltName.h"

namespace hmibase {
namespace util {

class DltApplication
{
   public:
      DltApplication(const char* name = "NONE", const char* description = "No Description");
      virtual ~DltApplication();

      // DltName getNamexxx();

   private:
      static bool checkLibraryVersion();
      static void registerApp(const DltName& name, const char* description);
      static void unregisterApp();
};


} // namespace
} // namespace

#endif
