/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_SIMPLESTRING_H
#define HMIBASE_UTIL_SIMPLESTRING_H

#include <stdint.h>
#include <stdio.h>
#include <string>

namespace hmibase {
namespace util {

class SimpleString
{
   public:
      SimpleString();
      SimpleString(const SimpleString& value);
      SimpleString(const std::string&  value);
      SimpleString(const char*         value);

      SimpleString(int value) : mStrLen(0), mCharPtr(NULL)
      {
         *this = SimpleString::format("%d", value);
      }
      SimpleString(long value) : mStrLen(0), mCharPtr(NULL)
      {
         *this = SimpleString::format("%d"  , value);
      }
      SimpleString(float value) : mStrLen(0), mCharPtr(NULL)
      {
         *this = SimpleString::format("%.3f", value);
      }
      SimpleString(double value) : mStrLen(0), mCharPtr(NULL)
      {
         *this = SimpleString::format("%.3f", value);
      }

      virtual ~SimpleString();
      void clear();

      bool isEmpty() const
      {
         return mStrLen == 0;
      }
      size_t length() const
      {
         return mStrLen;
      }
      const char* cPtr() const;

      static SimpleString format(const char* format, ...);

      SimpleString& operator= (const SimpleString& right);
      SimpleString& operator+=(const SimpleString& right);
      SimpleString& operator<<(const SimpleString& right);

      SimpleString operator+(const SimpleString& right) const;

      bool operator==(const SimpleString& right) const;
      bool operator!=(const SimpleString& right) const;
      bool operator< (const SimpleString& right) const;
      bool operator> (const SimpleString& right) const;
      bool operator<=(const SimpleString& right) const;
      bool operator>=(const SimpleString& right) const;

      bool startsWith(const SimpleString& str) const;
      bool endsWith(const SimpleString& str) const;

      SimpleString& trim();
      SimpleString& trimFront();
      SimpleString& trimBack();

   private:
      size_t      mStrLen;
      char*       mCharPtr;

      // mCharPtr may contain non-ascii characters.
      // Changing single bytes might result in broken strings,
      // that's why operator[] is not public.
      static char smErrorChar;
      static char smErrorCharConst;
      char&       operator[](size_t pos);
      const char& operator[](size_t pos) const;

      void set(char* charPtr, size_t strLen);
      void copy(const char* charPtr, size_t strLen);

      static bool isWhiteChar(const char c);
};


typedef SimpleString SS;
} // namespace
} // namespace
#endif
