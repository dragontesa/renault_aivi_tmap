
#ifdef VARIANT_S_FTR_ENABLE_ETG_TO_DLT

#ifndef DLT_CONTEXT_NAME
#define DLT_CONTEXT_NAME "NONE"
#endif

#ifndef DLT_CONTEXT_DESCRIPTION
#define DLT_CONTEXT_DESCRIPTION "No Description"
#endif

#include "hmibase/util/DltApplication.h"
#include "hmibase/util/DltLogger.h"

#define ETG_TRACE_STD__USR1(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define ETG_TRACE_STD__USR2(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define ETG_TRACE_STD__USR3(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).info    (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define ETG_TRACE_STD__USR4(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).debug   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define ETG_TRACE_STD__COMP(args, ...)  hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).warning (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define ETG_TRACE_STD__ERR(args, ...)   hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).error   (hmibase::util::SS::format(args, ## __VA_ARGS__));
#define ETG_TRACE_STD__FATAL(args, ...) hmibase::util::DltLogger::getInstance(DLT_CONTEXT_NAME, DLT_CONTEXT_DESCRIPTION).fatal   (hmibase::util::SS::format(args, ## __VA_ARGS__));


#define ETG_TRACE_USR4(printfArgs)    ETG_TRACE_STD__USR4  printfArgs
#define ETG_TRACE_USR1(printfArgs)    ETG_TRACE_STD__USR1  printfArgs
#define ETG_TRACE_USR2(printfArgs)    ETG_TRACE_STD__USR2  printfArgs
#define ETG_TRACE_USR3(printfArgs)    ETG_TRACE_STD__USR3  printfArgs
#define ETG_TRACE_COMP(printfArgs)    ETG_TRACE_STD__COMP  printfArgs
#define ETG_TRACE_ERR(printfArgs)     ETG_TRACE_STD__ERR   printfArgs
#define ETG_TRACE_FATAL(printfArgs)   ETG_TRACE_STD__FATAL printfArgs

#define ETG_TRACE_USR4_THR(printfArgs)    ETG_TRACE_STD__USR4  printfArgs
#define ETG_TRACE_USR1_THR(printfArgs)    ETG_TRACE_STD__USR1  printfArgs
#define ETG_TRACE_USR2_THR(printfArgs)    ETG_TRACE_STD__USR2  printfArgs
#define ETG_TRACE_USR3_THR(printfArgs)    ETG_TRACE_STD__USR3  printfArgs
#define ETG_TRACE_COMP_THR(printfArgs)    ETG_TRACE_STD__COMP  printfArgs
#define ETG_TRACE_ERR_THR(printfArgs)     ETG_TRACE_STD__ERR   printfArgs
#define ETG_TRACE_FATAL_THR(printfArgs)   ETG_TRACE_STD__FATAL printfArgs

// not supported features in DLT
#define ETG_TRACE_ERRMEM(printfArgs)

#endif
