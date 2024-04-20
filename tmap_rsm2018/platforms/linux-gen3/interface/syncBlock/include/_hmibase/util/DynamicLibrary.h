/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_DYNAMICLIBRARY_H
#define HMIBASE_UTIL_DYNAMICLIBRARY_H

#include "hmibase/util/SimpleString.h"

namespace hmibase {
namespace util {

class DynamicLibrary
{
   public:
      DynamicLibrary(const SimpleString& filepath);
      virtual ~DynamicLibrary();

      bool isLoaded() const;
      void* getFunctionPtr(const SimpleString& name);

   private:
      SimpleString mFilePath;
      void* mLibraryHandle;

      void openLibrary();
      void closeLibrary();
      SimpleString getErrorMessage(const SimpleString& message);
};


} // namespace
} // namespace
#endif
