/* ***************************************************************************************
* FILE:          Macros.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Macros.h is part of HMI-Base util Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */
#ifndef HMIBASE_UTIL_PERMISSIONS_H
#define HMIBASE_UTIL_PERMISSIONS_H

#include "hmibase/util/Macros.h"
HMIBASE_SUPPRESS_GCC_WARNING_BEGIN("-Wunused-variable", "variable might be used or not.")

#ifdef WIN32
typedef int uid_t;
typedef int gid_t;
#else
#include <sys/types.h>
#endif

namespace hmibase {
namespace util {
namespace user {
static const uid_t aid_hmibase_core = 10901;
static const uid_t aid_screenbroker = 10902;
}


namespace group {
static const gid_t aid_hmibase_core = 10901;
static const gid_t aid_screenbroker = 10902;
static const gid_t aid_hmibase = 10903;
}


}
}


#endif
