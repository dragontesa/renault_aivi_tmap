/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_TYPECOUNTER_H
#define HMIBASE_UTIL_TYPECOUNTER_H


#include "hmibase/util/Mutex.h"
#include "hmibase/util/SimpleString.h"
#include "hmibase/util/TypeCountable.h"
#include <map>
#include <set>

namespace hmibase {
namespace util {

/**
This class should be used via Singleton<TypeCounter>::getInstance().
It will count the instances of classes derived from TypeCountable.
To get overview of the current number of instances, call getTypeCounts().
To get the current number of instances of a specific type, call getTypeCount(typeName).
The names of types are pretty cryptic and change from system to system and compiler to compiler.
To get the typename of a specific object, call its TypeCountable::getTypeName().
EG:
class A: public TypeCountable
{
   A() : TypeCountable() {}
   virtual ~A() {}
};


A* aPtr = new A();

// aTypeName = "N7hmibase4util4test11typecounter1AE" <-- This is how type names might look like
SimpleString aTypeName = aPtr->getTypeName();

TypeCounter& typeCounter = Singleton Singleton<TypeCounter>::getInstance()
int aTypeCount = typeCounter.getTypeCount(aTypeName); // aTypeCount = 1;

// Alternatively, you can also ask an object how many other instances of its type exist.
int aTypeCount = aPtr->getTypeCount(); // aTypeCount = 1;
*/
class TypeCounter
{
   public:
      TypeCounter();
      virtual ~TypeCounter();

      void clear();
      void registerObject(TypeCountable* objectPtr);
      void releaseObject(TypeCountable* objectPtr);

      int getTypeCount(const SimpleString& typeName);
      std::map<SimpleString, int> getTypeCounts();
      void print();

   private:
      Mutex mMutex;
      std::set<TypeCountable*> mObjectPtrs;

      // Thou shalt not copy me
      TypeCounter(const TypeCounter&);
      TypeCounter& operator=(const TypeCounter&);
};


} // namespace
} // namespace
#endif
