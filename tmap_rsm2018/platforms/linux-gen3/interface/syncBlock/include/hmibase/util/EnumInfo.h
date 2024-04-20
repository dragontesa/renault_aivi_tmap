/* ***************************************************************************************
* FILE:          EnumInfo.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  EnumInfo.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */
#ifndef __HMIBASE_UTIL_ENUM_INFO_H__
#define __HMIBASE_UTIL_ENUM_INFO_H__

namespace hmibase {
namespace util {

template <typename TEnum> class EnumInfo
{
   public:
      static bool convertFromString(const char* name, TEnum& value);
};


}
}


#define HMIBASE_ENUM_INFO_BEGIN(TEnum)\
namespace hmibase { namespace util {\
template <> bool EnumInfo< TEnum > ::convertFromString(const char* _name, TEnum& _value)\
{\
if (_name == NULL) return false;

#define HMIBASE_ENUM_INFO_ENTRY(name, value)\
if (strcmp(_name, name) == 0) { _value = value; return true; }

#define HMIBASE_ENUM_INFO_END()\
return false;\
}}}


#define HMIBASE_ENUM_INFO_CONVERT_STRING(TEnum, name, value) ::hmibase::util::EnumInfo< TEnum > ::convertFromString(name, value)

#endif//__HMIBASE_UTIL_ENUM_INFO_H__
