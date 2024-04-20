/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#include "hmibase/util/DltApplication.h"
#include "hmibase/gadget/videobuffer/VideoBufferType.h"
#include "hmibase/gadget/syncblock2/Consumer.h"
#include "hmibase/util/Timeout.h"
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;
using namespace hmibase::util;
using namespace hmibase::gadget::syncblock2;
using namespace hmibase::gadget::videobuffer;


int consum() {
   // Give Dlt a chance to fire up
   sleep(1);
   hmibase::util::DltApplication dltApplication("CONS", "SyncBlock2 Consumer Example");
   sleep(1);

   cout << "Creating Consumer" << endl;

   Consumer* consumerPtr = new Consumer(13);
   consumerPtr->create(100, false);

   Timeout timeout(300000); // End Consumer after 30 Seconds
   while (!timeout.isReached())
   {
      cout << "Waiting for Update" << endl;
      consumerPtr->waitForUpdate();

      vector<int> updateIds;
      consumerPtr->isUpdated(updateIds);

      for (int i = 0; i < updateIds.size(); i++)
      {
         int instanceId = updateIds[i];
         int32_t fd = consumerPtr->exchange(instanceId);

         if (fd == -1)
         {
            cout << "Consumer[" << instanceId << "]: No Buffer available" << endl;
         }
         else
         {
            BufferInfo bi = consumerPtr->getBufferInfo(instanceId);

            VideoBufferType videoBuffer(bi.getWidth(), bi.getHeight(), bi.getDepth(), bi.getBpp());
            videoBuffer.attach(fd, bi.getSize());

            uint32_t counter = 0;
            if (videoBuffer.getDataPtr() != NULL)
            {
               memcpy(&counter, videoBuffer.getDataPtr(), sizeof(counter));
            }

            cout << "Consumer[" << instanceId << "]: Buffer Content: " << counter << endl;
         }
      }
   }

   cout << "Timeout is reached, destroying Consumer..." << endl;
   delete consumerPtr;

   cout << "Done!!!" << endl;
   return 0;
}


