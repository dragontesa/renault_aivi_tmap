/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_DLTNAME_H
#define HMIBASE_UTIL_DLTNAME_H

namespace hmibase {
namespace util {

/** This is a 4 character string, which is used as DLT-ApplicationId and DLT-ContextId.
 * If you pass longer strings in the constructor or assignemnt operator,
 * the will be automatically truncated. */
class DltName
{
   public:
      DltName();
      DltName(const char*    name);
      DltName(const DltName& name);
      virtual ~DltName();

      const char* cPtr() const;

      void clear();

      DltName& operator= (const char*    right);
      DltName& operator= (const DltName& right);

      bool     operator==(const DltName& right) const;
      bool     operator!=(const DltName& right) const;
      bool     operator<=(const DltName& right) const;
      bool     operator>=(const DltName& right) const;
      bool     operator< (const DltName& right) const;
      bool     operator> (const DltName& right) const;

   private:
      char mName[5];

      int compare(const DltName& right) const;
};


} // namespace
} // namespace

#endif
