/* ***************************************************************************************
* FILE:          TimerThreaded.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  TimerThreaded.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2018 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#if !defined(_hmibase_util_timer_h)
#define _hmibase_util_timer_h

#include "Macros.h"
#include <string>

#include "hmibase/util/Thread.h"
#include "hmibase/util/Semaphore.h"
#include "hmibase/util/Ticker.h"

#define TIMER_MAX_TIME_OUTS   10

namespace hmibase {
namespace util {

class TimerThreaded
{
   public:
      typedef int32_t TimerId;

      class ITimerFunction
      {
            // the timer callback function
         public:
            virtual ~ITimerFunction() {}
            virtual void timerExpired(TimerId timerId, uint32_t timIndex) = 0;
      };

   public:
      enum Status
      {
         Stopped,
         Running,
         Paused,
         Expired
      };

      static bool createTimerThread();

      TimerThreaded();
      virtual ~TimerThreaded();

      void start();
      void restart();
      void stop();
      void pause();
      void resume();

      bool setTimeoutWithRepeat(uint32_t timeout, uint32_t repeatTime, TimerThreaded::ITimerFunction* fktTimerCallback);
      bool setTimeout(uint32_t timeout, TimerThreaded::ITimerFunction* fktTimerCallback);
      bool setTimeout(uint32_t timIndex, uint32_t timeout, TimerThreaded::ITimerFunction* fktTimerCallback);
      void resetTimeouts();

      void setName(const char* compName, const char* instanceName);
      void setTimerId(TimerId id)
      {
         _timerId = id;
      }

      inline uint32_t getRepeatCounter() const
      {
         return _repeatCounter;
      }

      inline uint32_t getElapsedTimeSinceStart() const
      {
         return _startTicks.diffMsec();
      }

      inline bool stopped() const
      {
         return (getStatus() == Stopped);
      }

      inline bool running() const
      {
         return (getStatus() == Running);
      }

      inline Status getStatus() const
      {
         return _status;
      }

      inline bool multiTimeoutExpired() const
      {
         return (getStatus() == Expired || (_timIndex > 0 && getStatus() == Running));
      }

      inline uint32_t getExpiringTime() const
      {
         return _expiringTime;
      }

      std::string	getName() const
      {
         return _name;
      }

      virtual void expire();

   public:
      void startInternal(uint32_t timeout);
      void stopInternal();

   private:
      Status	   _status;
      uint32_t	   _timeout[TIMER_MAX_TIME_OUTS + 1];
      uint32_t	   _repeatTime;
      uint32_t	   _timIndex;           // index if different timers 0..10
      uint32_t    _repeatCounter;      // counter repeat events
      uint32_t    _expiringTime;
      uint32_t    _remainingTime;
      std::string	_name;               // friendly Name for trace & debug

      ::hmibase::util::Ticker        _startTicks;
      TimerThreaded::ITimerFunction* _fktTimerCallback;

      TimerId _timerId;
      HMIBASE_UNCOPYABLE(TimerThreaded);
};


} // namespace util
} // namespace hmibase


#endif  // _hmibase_util_timer_h
