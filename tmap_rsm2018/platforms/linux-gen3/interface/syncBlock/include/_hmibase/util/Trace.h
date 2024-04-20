/* ***************************************************************************************
* FILE:          Trace.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Trace.h is part of HMI-Base util Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef HMIBASE_UTIL_TRACE_INCLUDED
#define HMIBASE_UTIL_TRACE_INCLUDED


#if !defined(WIN32) && !defined(VARIANT_S_FTR_ENABLE_ETG_QTTRACE) && !defined(VARIANT_S_FTR_ENABLE_ETG_CONSOLETRACE) && !defined (VARIANT_S_FTR_ENABLE_ETG_TO_DLT) // TARGET,LSIM

#define ETG_S_IMPORT_INTERFACE_GENERIC
#include "etg_if.h"  // ==> includes the important file "ai_osal_linux\components\bosch_includes\trace\mc_trace.h"

// include the global trace maco defines like ETG_TRACE_...
#define ETG_ENABLED
#include "trace_interface.h"

#ifdef VARIANT_S_FTR_ENABLE_ETG_PRINTF
#include <cstdio>
#endif // VARIANT_S_FTR_ENABLE_ETG_PRINTF

#elif defined (VARIANT_S_FTR_ENABLE_ETG_TO_DLT)
#include "hmibase/util/DltLoggerMacros.h"
#else

#define TR_COMP_UI 0x2800
#include "hmibase/util/hmibase_win_etg.h"

#endif

#define TR_CLASS_HMI_FW                         TR_COMP_UI   // 0x2800 HMI_Framework

#endif
