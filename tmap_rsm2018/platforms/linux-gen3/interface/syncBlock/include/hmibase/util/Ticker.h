/* ***************************************************************************************
* FILE:          Ticker.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Ticker.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __Ticker_H_
#define __Ticker_H_

//lint -sem(hmibase::util::Ticker::start,initializer)

#include "Macros.h"
#include <string>
#include <stdint.h>

namespace hmibase {
namespace util {

class Ticker
{
   public:
      Ticker();
   public:
      void     start();
      void     stop();
      uint64_t diffUsec() const;
      inline uint32_t diffMsec() const
      {
         return static_cast<uint32_t>(diffUsec() / 1000);
      }
      inline void reset()
      {
         start();
      }

      static void sleepUSec(unsigned long USec);
      static void sleepMSec(unsigned long MSec);
      static uint64_t getTickCountUsec();
      static uint32_t getTickCountMsec();

   private:
      uint64_t _tickstart;
      uint64_t _tickstop;
};


class File
{
   public:
      static std::string getBinFolder(void);
      static std::string getSourceRootFolder(void);
      static std::string getSourceRootFolderAddPath(const std::string& addPath);
};


}
namespace utils {
HMIBASE_DEPRECATED("Deprecated class hmibase::utils::Ticker, please use hmibase::util::Ticker instead", typedef ::hmibase::util::Ticker Ticker);
}


}

#endif
