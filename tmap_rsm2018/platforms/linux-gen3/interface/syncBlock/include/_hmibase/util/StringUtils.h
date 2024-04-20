/* ***************************************************************************************
* FILE:          StringUtils.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  StringUtils.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <vector>
#include <sstream>

namespace hmibase {
namespace util {

int utilsnprintf(char* buffer, size_t buffersize, const char* lpszFormat, ...);
std::vector<std::string> split(const std::string& s, char delim);
}


}
#endif
