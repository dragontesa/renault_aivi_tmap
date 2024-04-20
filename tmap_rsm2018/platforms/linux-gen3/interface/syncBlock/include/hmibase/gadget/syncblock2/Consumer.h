/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_SYNCBLOCK2_CONSUMER_H
#define HMIBASE_GADGET_SYNCBLOCK2_CONSUMER_H

#include "hmibase/gadget/syncblock2/BufferInfo.h"
#include "hmibase/util/FixedSizeMutex.h"
#include "hmibase/util/SimpleThread.h"
#include "hmibase/util/Threadable.h"
#include <map>
#include <list>
#include <vector>
#include <pthread.h>

namespace hmibase {
namespace gadget {
namespace syncblock2 {

class ConsumerInst;
namespace socks {
class ListeningSocket;
class Socket;
}


class Consumer: public hmibase::util::Threadable
{
   public:
      Consumer(int key);
      virtual ~Consumer();

      bool create(int /*numInst*/, bool /*withCb*/ = true)
      {
         return true;   // Obsolete
      }

      BufferInfo getBufferInfo(int inst);

      void clearUpdatedFlag(int inst);
      int  exchange(int inst);
      void waitForUpdate();
      void isUpdated(std::vector<int>& updateIds);

      virtual void onStart(const bool& keepRunning);
      virtual bool onRun(const bool& keepRunning);
      virtual void onFinish(const bool& keepRunning);

   protected:
      virtual void onUpdate(int /*inst*/)
      {
         mUpdateIsImplemented = false;
      };

   private:
      int                           mKey;
      socks::ListeningSocket*       mListeningSocketPtr;
      std::map<int, ConsumerInst*>  mInstPtrs;
      hmibase::util::FixedSizeMutex mMutex;
      hmibase::util::SimpleThread   mThread;
      bool                          mUpdateIsImplemented;

      socks::Socket* acceptSocket();
};


} // namespace
} // namespace
} // namespace

#endif
