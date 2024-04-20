/* ***************************************************************************************
* FILE:          Singleton.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Singleton.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef SINGLETON_H
#define SINGLETON_H

#define HMIBASE_UNCOPYABLE_CLASS(_name_)             \
   private:                                          \
      _name_()                                       \
      {                                              \
      }                                              \
      _name_(const _name_& );                        \
      _name_& operator=(const _name_&);

#define HMIBASE_SINGLETON_CLASS(_name_)              \
   public:                                           \
      static const _name_* GetInstance()   \
      {                                              \
         static const _name_ l_name_;                \
         return &l_name_;                            \
      }                                              \
      HMIBASE_UNCOPYABLE_CLASS(_name_)               \


namespace hmibase {
namespace util {

template<class T>
class Singleton : public T
{
   public:
      static Singleton& getInstance()
      {
         static Singleton smInstance;
         return smInstance;
      }
   private:
      Singleton() : T() {}
      virtual ~Singleton() {}

      Singleton(Singleton&);
      Singleton& operator=(const Singleton&);
};


} // namespace
} // namespace

#endif
