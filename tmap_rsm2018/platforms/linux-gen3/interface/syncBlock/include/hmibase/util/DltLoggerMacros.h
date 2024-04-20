/* ***************************************************************************************
* FILE:          DltLoggerMacros.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  DltLoggerMacros.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifdef VARIANT_S_FTR_ENABLE_ETG_TO_DLT

#ifndef DLT_CONTEXT_NAME
#define DLT_CONTEXT_NAME "NONE"
#endif

#ifndef DLT_CONTEXT_DESCRIPTION
#define DLT_CONTEXT_DESCRIPTION "No Description"
#endif

#include "hmibase/util/DltApplication.h"
#include "hmibase/util/DltLogger.h"

//// #define vInitPlatformEtg()    // not used in DLT mode
#define ETG_I_REGISTER_FILE() // not used in DLT mode
#define ETG_I_REGISTER_CHN(a) // not used in DLT mode
#define ETG_I_CMD_DEFINE(a)   // not used in DLT mode
inline unsigned char etg_bIsTraceActive(unsigned short /*u16CompID*/, unsigned short /*u16LevelId*/)
{
   return 1;
}


inline unsigned char etg_bIsTraceActiveShort(unsigned long /*CompAndLevelId*/)
{
   return 1;
}


inline unsigned char etg_bIsTraceActiveDouble(unsigned long /*CompAndLevelId*/, unsigned short /*u16LevleId2*/)
{
   return 1;
}


#define _ETG_TRACE_STD__USR1(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_STD__USR4(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).debug   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_STD__COMP(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).warning (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_STD__ERR(args, ...)   hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).error   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_STD__FATAL(args, ...) hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).fatal   (hmibase::util::SS::format(args, ## __VA_ARGS__));

#define _ETG_TRACE_DCL__USR1(TraceClass,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_DCL__USR4(TraceClass,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).debug   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_DCL__COMP(TraceClass,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).warning (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_DCL__ERR(TraceClass,args, ...)   hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).error   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_DCL__FATAL(TraceClass,args, ...) hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).fatal   (hmibase::util::SS::format(args, ## __VA_ARGS__));

#define _ETG_TRACE_CLS__USR1(TraceClass,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS__USR4(TraceClass,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).debug   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS__COMP(TraceClass,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).warning (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS__ERR(TraceClass,args, ...)   hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).error   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS__FATAL(TraceClass,args, ...) hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).fatal   (hmibase::util::SS::format(args, ## __VA_ARGS__));

#define _ETG_TRACE_CLS_DCL__USR1(t1cls,t2dcl,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS_DCL__USR4(t1cls,t2dcl,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).debug   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS_DCL__COMP(t1cls,t2dcl,args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).warning (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS_DCL__ERR(t1cls,t2dcl,args, ...)   hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).error   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define _ETG_TRACE_CLS_DCL__FATAL(t1cls,t2dcl,args, ...) hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).fatal   (hmibase::util::SS::format(args, ## __VA_ARGS__));

#define ETG_CENUM(cEnumName,u8Val) (u8Val)

#define ETG_TRACE_ERRMEM(printfArgs)                                             _ETG_TRACE_STD__FATAL printfArgs
#define ETG_TRACE_ERRMEM_CLS(u16TraceClass_PrintfArgs)                           _ETG_TRACE_CLS__FATAL u16TraceClass_PrintfArgs
#define ETG_TRACE_ERRMEM_DCL(u16DerivedClass_printfArgs)                         _ETG_TRACE_DCL__FATAL u16DerivedClass_printfArgs
#define ETG_TRACE_ERRMEM_CLS_DCL(printfArgs)                                     _ETG_TRACE_CLS_DCL__FATAL printfArgs

#define ETG_TRACE_FATAL(printfArgs)                                              _ETG_TRACE_STD__FATAL printfArgs
#define ETG_TRACE_FATAL_CLS(u16TraceClass_PrintfArgs)                            _ETG_TRACE_CLS__FATAL u16TraceClass_PrintfArgs
#define ETG_TRACE_FATAL_DCL(u16DerivedClass_printfArgs)                          _ETG_TRACE_DCL__FATAL u16DerivedClass_printfArgs
#define ETG_TRACE_FATAL_CLS_DCL(printfArgs)                                      _ETG_TRACE_CLS_DCL__FATAL printfArgs

#define ETG_TRACE_ERR(printfArgs)                                                _ETG_TRACE_STD__ERR printfArgs
#define ETG_TRACE_ERR_CLS(u16TraceClass_PrintfArgs)                              _ETG_TRACE_CLS__ERR u16TraceClass_PrintfArgs
#define ETG_TRACE_ERR_DCL(u16DerivedClass_printfArgs)                            _ETG_TRACE_DCL__ERR u16DerivedClass_printfArgs
#define ETG_TRACE_ERR_CLS_DCL(printfArgs)                                        _ETG_TRACE_CLS_DCL__ERR printfArgs

#define ETG_TRACE_SYS_MIN(printfArgs)                                            _ETG_TRACE_STD__COMP printfArgs
#define ETG_TRACE_SYS_MIN_CLS(u16TraceClass_PrintfArgs)                          _ETG_TRACE_CLS__COMP u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_MIN_DCL(u16DerivedClass_printfArgs)                        _ETG_TRACE_DCL__COMP u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_MIN_CLS_DCL(printfArgs)                                    _ETG_TRACE_CLS_DCL__COMP printfArgs

#define ETG_TRACE_SYS(printfArgs)                                                _ETG_TRACE_STD__COMP printfArgs
#define ETG_TRACE_SYS_CLS(u16TraceClass_PrintfArgs)                              _ETG_TRACE_CLS__COMP u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_DCL(u16DerivedClass_printfArgs)                            _ETG_TRACE_DCL__COMP u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_CLS_DCL(printfArgs)                                        _ETG_TRACE_CLS_DCL__COMP printfArgs

#define ETG_TRACE_COMP(printfArgs)                                               _ETG_TRACE_STD__COMP printfArgs
#define ETG_TRACE_COMP_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__COMP u16TraceClass_PrintfArgs
#define ETG_TRACE_COMP_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__COMP u16DerivedClass_printfArgs
#define ETG_TRACE_COMP_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__COMP printfArgs

#define ETG_TRACE_USR1(printfArgs)                                               _ETG_TRACE_STD__USR1 printfArgs
#define ETG_TRACE_USR1_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__USR1 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR1_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__USR1 u16DerivedClass_printfArgs
#define ETG_TRACE_USR1_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__USR1 printfArgs

#define ETG_TRACE_USR2(printfArgs)                                               _ETG_TRACE_STD__USR1 printfArgs
#define ETG_TRACE_USR2_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__USR1 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR2_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__USR1 u16DerivedClass_printfArgs
#define ETG_TRACE_USR2_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__USR1 printfArgs

#define ETG_TRACE_USR3(printfArgs)                                               _ETG_TRACE_STD__USR1 printfArgs
#define ETG_TRACE_USR3_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__USR1 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR3_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__USR1 u16DerivedClass_printfArgs
#define ETG_TRACE_USR3_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__USR1 printfArgs

#define ETG_TRACE_USR4(printfArgs)                                               _ETG_TRACE_STD__USR4 printfArgs
#define ETG_TRACE_USR4_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__USR4 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR4_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__USR4 u16DerivedClass_printfArgs
#define ETG_TRACE_USR4_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__USR4 printfArgs


#define ETG_TRACE_LVL(printfArgs)                                                // not supportet for WIN32
#define ETG_TRACE_LVL_CLS(u16TraceClass_PrintfArgs)                              // not supportet for WIN32
#define ETG_TRACE_LVL_THR(printfArgs)                                            // not supportet for WIN32
#define ETG_TRACE_LVL_CLS_THR(u16TraceClass_PrintfArgs)                          // not supportet for WIN32
#define ETG_TRACE_LVL_DCL(u16DerivedClass_printfArgs)                            // not supportet for WIN32
#define ETG_TRACE_LVL_CLS_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)          // not supportet for WIN32
#define ETG_TRACE_LVL_THR_DCL(u16DerivedClass_printfArgs)                        // not supportet for WIN32
#define ETG_TRACE_LVL_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)      // not supportet for WIN32

#define ETG_TRACE_ERRMEM_THR(printfArgs)                                         _ETG_TRACE_STD__FATAL printfArgs
#define ETG_TRACE_ERRMEM_CLS_THR(u16TraceClass_PrintfArgs)                       _ETG_TRACE_CLS__FATAL u16TraceClass_PrintfArgs
#define ETG_TRACE_ERRMEM_THR_DCL(u16DerivedClass_printfArgs)                     _ETG_TRACE_DCL__FATAL u16DerivedClass_printfArgs
#define ETG_TRACE_ERRMEM_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)   _ETG_TRACE_CLS_DCL__FATAL u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_FATAL_THR(printfArgs)                                          _ETG_TRACE_STD__FATAL printfArgs
#define ETG_TRACE_FATAL_CLS_THR(u16TraceClass_PrintfArgs)                        _ETG_TRACE_CLS__FATAL u16TraceClass_PrintfArgs
#define ETG_TRACE_FATAL_THR_DCL(u16DerivedClass_printfArgs)                      _ETG_TRACE_DCL__FATAL u16DerivedClass_printfArgs
#define ETG_TRACE_FATAL_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)    _ETG_TRACE_CLS_DCL__FATAL u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_ERR_THR(printfArgs)                                            _ETG_TRACE_STD__ERR printfArgs
#define ETG_TRACE_ERR_CLS_THR(u16TraceClass_PrintfArgs)                          _ETG_TRACE_CLS__ERR u16TraceClass_PrintfArgs
#define ETG_TRACE_ERR_THR_DCL(u16DerivedClass_printfArgs)                        _ETG_TRACE_DCL__ERR u16DerivedClass_printfArgs
#define ETG_TRACE_ERR_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)      _ETG_TRACE_CLS_DCL__ERR u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_SYS_MIN_THR(printfArgs)                                        _ETG_TRACE_STD__COMP printfArgs
#define ETG_TRACE_SYS_MIN_CLS_THR(u16TraceClass_PrintfArgs)                      _ETG_TRACE_CLS__COMP u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_MIN_THR_DCL(u16DerivedClass_printfArgs)                    _ETG_TRACE_DCL__COMP u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_MIN_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)  _ETG_TRACE_CLS_DCL__COMP u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_SYS_THR(printfArgs)                                            _ETG_TRACE_STD__COMP printfArgs
#define ETG_TRACE_SYS_CLS_THR(u16TraceClass_PrintfArgs)                          _ETG_TRACE_CLS__COMP u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_THR_DCL(u16DerivedClass_printfArgs)                        _ETG_TRACE_DCL__COMP u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)      _ETG_TRACE_CLS_DCL__COMP u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_COMP_THR(printfArgs)                                           _ETG_TRACE_STD__COMP printfArgs
#define ETG_TRACE_COMP_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__COMP u16TraceClass_PrintfArgs
#define ETG_TRACE_COMP_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__COMP u16DerivedClass_printfArgs
#define ETG_TRACE_COMP_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__COMP u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR1_THR(printfArgs)                                           _ETG_TRACE_STD__USR1 printfArgs
#define ETG_TRACE_USR1_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__USR1 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR1_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__USR1 u16DerivedClass_printfArgs
#define ETG_TRACE_USR1_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__USR1 u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR2_THR(printfArgs)                                           _ETG_TRACE_STD__USR1 printfArgs
#define ETG_TRACE_USR2_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__USR1 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR2_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__USR1 u16DerivedClass_printfArgs
#define ETG_TRACE_USR2_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__USR1 u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR3_THR(printfArgs)                                           _ETG_TRACE_STD__USR1 printfArgs
#define ETG_TRACE_USR3_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__USR1 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR3_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__USR1 u16DerivedClass_printfArgs
#define ETG_TRACE_USR3_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__USR1 u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR4_THR(printfArgs)                                           _ETG_TRACE_STD__USR4 printfArgs
#define ETG_TRACE_USR4_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__USR4 u16TraceClass_PrintfArgs
#define ETG_TRACE_USR4_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__USR4 u16DerivedClass_printfArgs
#define ETG_TRACE_USR4_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__USR4 u16DerivedClass_u16TraceClass_PrintfArgs

/// not supported features in DLT
//#define ETG_TRACE_ERRMEM(printfArgs)
//#define ETG_TRACE_ERRMEM_THR(printfArgs)

#endif
