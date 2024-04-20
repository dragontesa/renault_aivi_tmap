/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_PATH_H
#define HMIBASE_UTIL_PATH_H

#include "hmibase/util/SimpleString.h"
#include <vector>
#include <set>

namespace hmibase {
namespace util {


class Path
{
   public:
      Path();
      Path(const Path& path);
      Path(const SimpleString& str);
      virtual ~Path();

      void clear();
      void normalize();

      bool isEmpty() const;
      bool isFile() const;
      bool isDirectory() const;
      bool exists() const;

      SimpleString toString() const;
      SimpleString getName() const;

      Path getParent() const;
      std::set<Path> getChildren() const;

      Path& operator= (const Path& right);
      Path& operator+=(const Path& right);
      Path  operator+ (const Path& right) const;

      bool operator==(const Path& right) const;
      bool operator!=(const Path& right) const;
      bool operator< (const Path& right) const;
      bool operator> (const Path& right) const;
      bool operator<=(const Path& right) const;
      bool operator>=(const Path& right) const;

      void print() const;

   private:
      std::vector<SimpleString> mElements;

      void appendNormalized(const SimpleString& element);

      static int compare(const Path& left, const Path& right);
      static int compare(const Path& left, const Path& right, int level);
};


} // namespace
} // namespace
#endif
