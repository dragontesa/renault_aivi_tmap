
#if !defined(_HMIBASE_UTIL_CONSOLETRACE_H)
#define _HMIBASE_UTIL_CONSOLETRACE_H

#include <map>
#ifdef CGI_SUPPORT
#include <FeatStd/Base.h>
#ifndef FEATSTD_LOG_ENABLED
#error Feature FEATSTD_ENABLE_LOG is not enabled. Do not include any FeatStd logging releated header files!
#endif
#endif

//#include "Trace/TraceUtils.h"

#ifdef CGI_SUPPORT
#include "FeatStd/Diagnostics/Appender.h"
#endif

namespace hmibase {
namespace util {

class ConsoleTrace
{
   public:

      enum Level
      {
         Fatal = 0,      ///< Fatal
         Error = 1,      ///< Error
         Warn = 2,    ///< Warning
         Info = 3,       ///< Info
         Debug = 4       ///< Debug
      };

      class Hook
      {
         public:
            virtual  void setClassLevel(unsigned int traceClass, ConsoleTrace::Level traceLevel) = 0;
      };

      void trace(ConsoleTrace::Level level, unsigned int uTraceClass1, unsigned int uTraceClass2, int line, const char* module, const char* fmt, ...);
      void setClassLevel(unsigned int traceClass, ConsoleTrace::Level traceLevel);
      unsigned printColor(unsigned short wTraceColor, const char* fmt, ...);
      unsigned print(const char* text);
      ConsoleTrace::Level getLevel(unsigned int traceClass);

#ifdef CGI_SUPPORT
      void traceSetup(hmibase::trace::sCGITraceGroup traceIds);
      void cgiLogEvent(const FeatStd::Diagnostics::LogEvent& logEvent);
#else
      void Setup();
#endif

      bool isTraceActive(unsigned int traceClass, unsigned short etgTraceLevel); // etg_bIsTraceActive()
      bool isTraceActiveDouble(unsigned int traceClass1AndEtgTraceLevel, unsigned short traceClass2); // etg_bIsTraceActiveDouble()

      void setHook(Hook* hook)
      {
         _hook = hook;
      }

      static ConsoleTrace& s_getInstance();

   private:
      ConsoleTrace();

      Level mapLevel(unsigned short etgTraceLevel);

      //void setCGIDiagnosticLogLevel(unsigned int traceClass, ConsoleTrace::Level traceLevel);
      typedef std::map<unsigned int, enum Level> traceClassMap;
      traceClassMap _levelMap;

      //struct TraceClassTabEntry
      //{
      //   unsigned int traceClass;
      //   ConsoleTrace::Level tracelevel;
      //};
      //static TraceClassTabEntry _tab[200];

      Hook* _hook;

#ifdef CGI_SUPPORT
      hmibase::trace::sCGITraceGroup _traceIds;
#else
      //hmibase::util::trace::sGUITraceGroup _traceIds;
#endif
};


}
}


#endif
