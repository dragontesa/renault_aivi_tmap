/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_ERROR_H
#define HMIBASE_UTIL_ERROR_H

#include "hmibase/util/SimpleString.h"

namespace hmibase {
namespace util {

/**
   If you have a function which sets errno, do this:
   Error::reset();
   functionWhichSetsErrno();
   if(Error::is())
   {
      cout << Error::getMessage().cPtr();
   }

   If you have function which returns standard errorCodes, do this:
   int errorCode = functionWhichSetsErrno();
   if(errorCode != 0)
   {
      cout << Error::getMessage(errorCode).cPtr();
   }
*/
class Error
{
   public:
      /** reset errno to 0 */
      static void reset();

      /** true if errno != 0 */
      static bool is();

      /** Get the value of errno */
      static int get();

      /** Get the value of strerror(errno) */
      static SimpleString getString();
      static SimpleString getString(int errorCode);

      /** Get formatted message: "error=35 (Resource deadlock avoided)" */
      static SimpleString getMessage();
      static SimpleString getMessage(int errorCode);
};


} // namespace
} // namespace
#endif
