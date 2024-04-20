/* ***************************************************************************************
* FILE:          sys_if.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  sys_if.h is part of HMI-Base util Library
*    COPYRIGHT:  (c) 2018 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __sys_if_
#define __sys_if_

#ifndef WIN32
// Linux
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>
#include <bitset>
#include <list>
#include <sstream>
#include <linux/input.h>
#include <cstring>
#else
#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cassert>
#include <bitset>
#include <list>
#include <sstream>
#include <cstring>
#endif

#endif
