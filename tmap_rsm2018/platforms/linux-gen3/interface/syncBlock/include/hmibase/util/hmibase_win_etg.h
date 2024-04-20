/* ***************************************************************************************
* FILE:          hmibase_win_etg.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  hmibase_win_etg.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef HMIBASE_WIN_ETG_H
#define HMIBASE_WIN_ETG_H

#if defined(VARIANT_S_FTR_ENABLE_ETG_CONSOLETRACE)

#if defined(WIN32)
typedef unsigned short etg_tU16;

#define ETG_C_TRACE_MAX 239

enum ETG_teLevel
{
   ETG_LEVEL_FATAL        = 0,
   ETG_LEVEL_ERRORS       = 1,
   ETG_LEVEL_SYSTEM_MIN   = 2,
   ETG_LEVEL_SYSTEM       = 3,
   ETG_LEVEL_COMPONENT    = 4,
   ETG_LEVEL_USER_1       = 5,
   ETG_LEVEL_USER_2       = 6,
   ETG_LEVEL_USER_3       = 7,
   ETG_LEVEL_USER_4       = 8
};


#endif

#include <stdio.h>
#include "hmibase/util/ConsoleTrace.h"

#define _ETG_TRACE_STD__DEBUG(args, ...)                 ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Debug,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_STD__INFO(args, ...)                  ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Info,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_STD__WARN(args, ...)                  ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Warn,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_STD__ERROR(args, ...)                 ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Error,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_STD__FATAL(args, ...)                 ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Fatal,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)

#define _ETG_TRACE_DCL__DEBUG(TraceClass,args, ...)      ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Debug,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__INFO(TraceClass,args, ...)       ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Info,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__WARN(TraceClass,args, ...)       ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Warn,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__ERROR(TraceClass,args, ...)      ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Error,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__FATAL(TraceClass,args, ...)      ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Fatal,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args##, ## __VA_ARGS__)

#define _ETG_TRACE_CLS__DEBUG(TraceClass,args, ...)      ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Debug,TraceClass,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__INFO(TraceClass,args, ...)       ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Info,TraceClass,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__WARN(TraceClass,args, ...)       ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Warn,TraceClass,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__ERROR(TraceClass,args, ...)      ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Error,TraceClass,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__FATAL(TraceClass,args, ...)      ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Fatal,TraceClass,0,__LINE__,__FILE__,args##, ## __VA_ARGS__)

#define _ETG_TRACE_CLS_DCL__DEBUG(t1cls,t2dcl,args, ...) ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Debug,t1cls,t2dcl,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__INFO(t1cls,t2dcl,args, ...)  ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Info,t1cls,t2dcl,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__WARN(t1cls,t2dcl,args, ...)  ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Warn,t1cls,t2dcl,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__ERROR(t1cls,t2dcl,args, ...) ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Error,t1cls,t2dcl,__LINE__,__FILE__,args##, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__FATAL(t1cls,t2dcl,args, ...) ::hmibase::util::ConsoleTrace::s_getInstance().trace(::hmibase::util::ConsoleTrace::Fatal,t1cls,t2dcl,__LINE__,__FILE__,args##, ## __VA_ARGS__)

#define  etg_bIsTraceActive(u16CompID, u16LevelId)                ::hmibase::util::ConsoleTrace::s_getInstance().isTraceActive(u16CompID,u16LevelId)
#define  etg_bIsTraceActiveDouble(u32CompAndLevelID, u16CompId2)  ::hmibase::util::ConsoleTrace::s_getInstance().isTraceActiveDouble(u32CompAndLevelID, u16CompId2)

typedef unsigned int etg_tU32;
typedef unsigned short etg_tU16;

#elif defined(VARIANT_S_FTR_ENABLE_ETG_QTTRACE)

#include <stdio.h>
#include "hmibase/trace/QtCreatorTrace.h"

#define _ETG_TRACE_STD__DEBUG(args, ...)                 ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Debug,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_STD__INFO(args, ...)                  ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Info,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_STD__WARN(args, ...)                  ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Warn,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_STD__ERROR(args, ...)                 ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Error,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_STD__FATAL(args, ...)                 ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Fatal,ETG_DEFAULT_TRACE_CLASS,0,__LINE__,__FILE__,args, ## __VA_ARGS__)

#define _ETG_TRACE_DCL__DEBUG(TraceClass,args, ...)      ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Debug,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__INFO(TraceClass,args, ...)       ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Info,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__WARN(TraceClass,args, ...)       ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Warn,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__ERROR(TraceClass,args, ...)      ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Error,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_DCL__FATAL(TraceClass,args, ...)      ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Fatal,ETG_DEFAULT_TRACE_CLASS,TraceClass,__LINE__,__FILE__,args, ## __VA_ARGS__)

#define _ETG_TRACE_CLS__DEBUG(TraceClass,args, ...)      ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Debug,TraceClass,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__INFO(TraceClass,args, ...)       ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Info,TraceClass,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__WARN(TraceClass,args, ...)       ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Warn,TraceClass,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__ERROR(TraceClass,args, ...)      ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Error,TraceClass,0,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS__FATAL(TraceClass,args, ...)      ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Fatal,TraceClass,0,__LINE__,__FILE__,args, ## __VA_ARGS__)

#define _ETG_TRACE_CLS_DCL__DEBUG(t1cls,t2dcl,args, ...) ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Debug,t1cls,t2dcl,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__INFO(t1cls,t2dcl,args, ...)  ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Info,t1cls,t2dcl,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__WARN(t1cls,t2dcl,args, ...)  ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Warn,t1cls,t2dcl,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__ERROR(t1cls,t2dcl,args, ...) ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Error,t1cls,t2dcl,__LINE__,__FILE__,args, ## __VA_ARGS__)
#define _ETG_TRACE_CLS_DCL__FATAL(t1cls,t2dcl,args, ...) ::hmibase::util::QtCreatorTrace::trace(::hmibase::util::QtCreatorTrace::Fatal,t1cls,t2dcl,__LINE__,__FILE__,args, ## __VA_ARGS__)

#define  etg_bIsTraceActive(u16CompID, u16LevelId)                ::hmibase::util::QtCreatorTrace::isTraceActive(u16CompID,u16LevelId)
#define  etg_bIsTraceActiveDouble(u32CompAndLevelID, u16CompId2)  ::hmibase::util::QtCreatorTrace::isTraceActiveDouble(u32CompAndLevelID, u16CompId2)

typedef unsigned int etg_tU32;

#endif


#define ETG_CENUM(cEnumName,u8Val) (u8Val)

#define ETG_TRACE_ERRMEM(printfArgs)                                             _ETG_TRACE_STD__FATAL printfArgs
#define ETG_TRACE_ERRMEM_CLS(u16TraceClass_PrintfArgs)                           _ETG_TRACE_CLS__FATAL u16TraceClass_PrintfArgs
#define ETG_TRACE_ERRMEM_DCL(u16DerivedClass_printfArgs)                         _ETG_TRACE_DCL__FATAL u16DerivedClass_printfArgs
#define ETG_TRACE_ERRMEM_CLS_DCL(printfArgs)                                     _ETG_TRACE_CLS_DCL__FATAL printfArgs

#define ETG_TRACE_FATAL(printfArgs)                                              _ETG_TRACE_STD__FATAL printfArgs
#define ETG_TRACE_FATAL_CLS(u16TraceClass_PrintfArgs)                            _ETG_TRACE_CLS__FATAL u16TraceClass_PrintfArgs
#define ETG_TRACE_FATAL_DCL(u16DerivedClass_printfArgs)                          _ETG_TRACE_DCL__FATAL u16DerivedClass_printfArgs
#define ETG_TRACE_FATAL_CLS_DCL(printfArgs)                                      _ETG_TRACE_CLS_DCL__FATAL printfArgs

#define ETG_TRACE_ERR(printfArgs)                                                _ETG_TRACE_STD__ERROR printfArgs
#define ETG_TRACE_ERR_CLS(u16TraceClass_PrintfArgs)                              _ETG_TRACE_CLS__ERROR u16TraceClass_PrintfArgs
#define ETG_TRACE_ERR_DCL(u16DerivedClass_printfArgs)                            _ETG_TRACE_DCL__ERROR u16DerivedClass_printfArgs
#define ETG_TRACE_ERR_CLS_DCL(printfArgs)                                        _ETG_TRACE_CLS_DCL__ERROR printfArgs

#define ETG_TRACE_SYS_MIN(printfArgs)                                            _ETG_TRACE_STD__WARN printfArgs
#define ETG_TRACE_SYS_MIN_CLS(u16TraceClass_PrintfArgs)                          _ETG_TRACE_CLS__WARN u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_MIN_DCL(u16DerivedClass_printfArgs)                        _ETG_TRACE_DCL__WARN u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_MIN_CLS_DCL(printfArgs)                                    _ETG_TRACE_CLS_DCL__WARN printfArgs

#define ETG_TRACE_SYS(printfArgs)                                                _ETG_TRACE_STD__WARN printfArgs
#define ETG_TRACE_SYS_CLS(u16TraceClass_PrintfArgs)                              _ETG_TRACE_CLS__WARN u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_DCL(u16DerivedClass_printfArgs)                            _ETG_TRACE_DCL__WARN u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_CLS_DCL(printfArgs)                                        _ETG_TRACE_CLS_DCL__WARN printfArgs

#define ETG_TRACE_COMP(printfArgs)                                               _ETG_TRACE_STD__WARN printfArgs
#define ETG_TRACE_COMP_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__WARN u16TraceClass_PrintfArgs
#define ETG_TRACE_COMP_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__WARN u16DerivedClass_printfArgs
#define ETG_TRACE_COMP_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__WARN printfArgs

#define ETG_TRACE_USR1(printfArgs)                                               _ETG_TRACE_STD__INFO printfArgs
#define ETG_TRACE_USR1_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__INFO u16TraceClass_PrintfArgs
#define ETG_TRACE_USR1_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__INFO u16DerivedClass_printfArgs
#define ETG_TRACE_USR1_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__INFO printfArgs

#define ETG_TRACE_USR2(printfArgs)                                               _ETG_TRACE_STD__INFO printfArgs
#define ETG_TRACE_USR2_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__INFO u16TraceClass_PrintfArgs
#define ETG_TRACE_USR2_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__INFO u16DerivedClass_printfArgs
#define ETG_TRACE_USR2_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__INFO printfArgs

#define ETG_TRACE_USR3(printfArgs)                                               _ETG_TRACE_STD__INFO printfArgs
#define ETG_TRACE_USR3_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__INFO u16TraceClass_PrintfArgs
#define ETG_TRACE_USR3_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__INFO u16DerivedClass_printfArgs
#define ETG_TRACE_USR3_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__INFO printfArgs

#define ETG_TRACE_USR4(printfArgs)                                               _ETG_TRACE_STD__DEBUG printfArgs
#define ETG_TRACE_USR4_CLS(u16TraceClass_PrintfArgs)                             _ETG_TRACE_CLS__DEBUG u16TraceClass_PrintfArgs
#define ETG_TRACE_USR4_DCL(u16DerivedClass_printfArgs)                           _ETG_TRACE_DCL__DEBUG u16DerivedClass_printfArgs
#define ETG_TRACE_USR4_CLS_DCL(printfArgs)                                       _ETG_TRACE_CLS_DCL__DEBUG printfArgs


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

#define ETG_TRACE_ERR_THR(printfArgs)                                            _ETG_TRACE_STD__ERROR printfArgs
#define ETG_TRACE_ERR_CLS_THR(u16TraceClass_PrintfArgs)                          _ETG_TRACE_CLS__ERROR u16TraceClass_PrintfArgs
#define ETG_TRACE_ERR_THR_DCL(u16DerivedClass_printfArgs)                        _ETG_TRACE_DCL__ERROR u16DerivedClass_printfArgs
#define ETG_TRACE_ERR_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)      _ETG_TRACE_CLS_DCL__ERROR u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_SYS_MIN_THR(printfArgs)                                        _ETG_TRACE_STD__WARN printfArgs
#define ETG_TRACE_SYS_MIN_CLS_THR(u16TraceClass_PrintfArgs)                      _ETG_TRACE_CLS__WARN u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_MIN_THR_DCL(u16DerivedClass_printfArgs)                    _ETG_TRACE_DCL__WARN u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_MIN_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)  _ETG_TRACE_CLS_DCL__WARN u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_SYS_THR(printfArgs)                                            _ETG_TRACE_STD__WARN printfArgs
#define ETG_TRACE_SYS_CLS_THR(u16TraceClass_PrintfArgs)                          _ETG_TRACE_CLS__WARN u16TraceClass_PrintfArgs
#define ETG_TRACE_SYS_THR_DCL(u16DerivedClass_printfArgs)                        _ETG_TRACE_DCL__WARN u16DerivedClass_printfArgs
#define ETG_TRACE_SYS_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)      _ETG_TRACE_CLS_DCL__WARN u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_COMP_THR(printfArgs)                                           _ETG_TRACE_STD__WARN printfArgs
#define ETG_TRACE_COMP_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__WARN u16TraceClass_PrintfArgs
#define ETG_TRACE_COMP_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__WARN u16DerivedClass_printfArgs
#define ETG_TRACE_COMP_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__WARN u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR1_THR(printfArgs)                                           _ETG_TRACE_STD__INFO printfArgs
#define ETG_TRACE_USR1_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__INFO u16TraceClass_PrintfArgs
#define ETG_TRACE_USR1_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__INFO u16DerivedClass_printfArgs
#define ETG_TRACE_USR1_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__INFO u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR2_THR(printfArgs)                                           _ETG_TRACE_STD__INFO printfArgs
#define ETG_TRACE_USR2_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__INFO u16TraceClass_PrintfArgs
#define ETG_TRACE_USR2_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__INFO u16DerivedClass_printfArgs
#define ETG_TRACE_USR2_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__INFO u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR3_THR(printfArgs)                                           _ETG_TRACE_STD__INFO printfArgs
#define ETG_TRACE_USR3_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__INFO u16TraceClass_PrintfArgs
#define ETG_TRACE_USR3_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__INFO u16DerivedClass_printfArgs
#define ETG_TRACE_USR3_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__INFO u16DerivedClass_u16TraceClass_PrintfArgs

#define ETG_TRACE_USR4_THR(printfArgs)                                           _ETG_TRACE_STD__DEBUG printfArgs
#define ETG_TRACE_USR4_CLS_THR(u16TraceClass_PrintfArgs)                         _ETG_TRACE_CLS__DEBUG u16TraceClass_PrintfArgs
#define ETG_TRACE_USR4_THR_DCL(u16DerivedClass_printfArgs)                       _ETG_TRACE_DCL__DEBUG u16DerivedClass_printfArgs
#define ETG_TRACE_USR4_CLS_THR_DCL(u16DerivedClass_u16TraceClass_PrintfArgs)     _ETG_TRACE_CLS_DCL__DEBUG u16DerivedClass_u16TraceClass_PrintfArgs

#endif
