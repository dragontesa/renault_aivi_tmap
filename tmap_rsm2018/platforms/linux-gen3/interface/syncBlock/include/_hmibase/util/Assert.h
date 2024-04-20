/* ***************************************************************************************
* FILE:          Assert.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Assert.h is part of HMI-Base util Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#if !defined(__hmibase_util_assert_h_)
#define __hmibase_util_assert_h_

#ifdef __cplusplus


namespace hmibase {
namespace util {

void Assert(const char* expression, const char* filename, unsigned int linenumber, bool bAbort);
}


}

#endif // __cplusplus

#endif   // include guard
