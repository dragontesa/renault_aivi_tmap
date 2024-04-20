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

#if defined(VARIANT_S_FTR_ENABLE_ETG_TO_DLT)
// --> VARIANT_S_FTR_ENABLE_ETG_TO_DLT --  ETG_TRACE redirect to hmibase util DltLogger
#undef VARIANT_S_FTR_ENABLE_TRC_GEN
#define TRACE_S_IMPORT_INTERFACE_TYPES
#include "trace_if.h"
#include "hmibase/util/DltLoggerMacros.h"

#elif defined (VARIANT_S_FTR_ENABLE_ETG_CONSOLETRACE)
// --> VARIANT_S_FTR_ENABLE_ETG_CONSOLETRACE -- ETG_TRACE, class filtered with ConsoleTrace.h to WIN32 (->hmibase_win_etg)
#undef VARIANT_S_FTR_ENABLE_TRC_GEN
#include "hmibase/util/mc_trace_mock.h"
#include "hmibase/util/hmibase_win_etg.h"

#elif defined (VARIANT_S_FTR_ENABLE_ETG_QTTRACE)
// --> VARIANT_S_FTR_ENABLE_ETG_QTTRACE -- ETG_TRACE, class filtered with QtCreatorTrace.h to qDebug (->hmibase_win_etg)
#undef VARIANT_S_FTR_ENABLE_TRC_GEN
#include "hmibase/util/mc_trace_mock.h"
#include "hmibase/util/hmibase_win_etg.h"

#else
// --> VARIANT_S_FTR_ENABLE_TRC_GEN    -- ETG_TRACE standard mapping with generated database (->etg.h)
// --> VARIANT_S_FTR_ENABLE_ETG_PRINTF -- EG_TRACE, no class filter, redirection to printf  (->etg.h)
// --> VARIANT_S_FTR_ENABLE_ETG_QTLOG  -- ETG_TRACE mapping to qDebug,qWarning and qInfo  (->etg.h)

#define ETG_S_IMPORT_INTERFACE_GENERIC
#include "etg_if.h"  // ==> include the global trace macro defines like ETG_TRACE_...

#define ETG_ENABLED
// #include "trace_interface.h" // ==> includes the important file "ai_osal_linux\components\bosch_includes\trace\mc_trace.h"
extern "C" void vInitPlatformEtg(void);

#define TRACE_S_IMPORT_INTERFACE_TYPES
#include "trace_if.h"

// TR_TTFIS_UI_APP_HMI_00 is unfortunately is not defined mc_trace.h
#define TR_TTFIS_UI_APP_HMI_00  TR_TTFIS_UI_APP_HMI_PLAYGROUND
#define TR_COMP_UI_APP_HMI_00   TR_COMP_UI_APP_HMI_PLAYGROUND

#ifdef VARIANT_S_FTR_ENABLE_ETG_PRINTF
#include <cstdio>
#endif

#endif // VARIANT_S_FTR_ENABLE_TRC_GEN, ....

#define TR_CLASS_HMI_FW       (TR_COMP_UI+0)    // 0x2800+0 :todo should only be used in framework
#define TR_CLASS_HMI_FW_UTIL  (TR_COMP_UI+1)    // 0x2800+1

#endif // guard HMIBASE_UTIL_TRACE_INCLUDED
