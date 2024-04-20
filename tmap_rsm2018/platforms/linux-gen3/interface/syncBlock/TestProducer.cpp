/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#include "hmibase/util/DltApplication.h"
#include "SimpleProducer.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

using namespace std;
using namespace hmibase::gadget::syncblock2;
using namespace hmibase::gadget::videobuffer;

int TestProducer(int argc, char* argv[])
{
    cout << "******************************************" << endl;
    cout << "start with = " << argc << endl;

   // Give Dlt a chance to fire up
   sleep(1);
   hmibase::util::DltApplication dltApplication("PROD", "SyncBlock2 Producer Example");
   sleep(1);

   // This instanceId will be passed to the producer.
   // Every producer must have its unique instanceId.
   int instanceId = 0;

   if (argc == 2)
   {
      instanceId = strtoul(argv[1], NULL, 10);
      cout << "instanceId = " << instanceId << endl;
   }
   else
   {
      cout << "Usage: programname instanceId" << endl;
      cout << "instanceId must be an integer value." << endl;
      return 1;
   }

   cout << "Producer-Version: " << SimpleProducer::getVersion() << endl;

   //SimpleProducer::initialize(13, SHM_PRODUCER);
   SimpleProducer::initialize(13, SOCKET_PRODUCER);

   uint16_t width  = 100;
   uint16_t height = 200;
   uint16_t depth  = 8;
   uint16_t bpp    = 4;

   // This will create VideoBuffers and set up communication with the consumer
   SimpleProducer* simpleProducerPtr = new SimpleProducer(instanceId, width, height, depth, bpp);

   // Get the size of the created buffers
   uint32_t size = simpleProducerPtr->getBufferInfo().getSize();
   cout << "Buffer Size = " << size << endl;

   uint32_t counter = 569;

   // This is your rendering loop
   // Here I will render for 10 seconds and then exit
   for (int i = 0; i < 10; i++)
   {
      // Try to get the next free buffer from this producer instance.
      VideoBuffer* videoBufferPtr = simpleProducerPtr->exchange();

      if (videoBufferPtr == NULL)
      {
         // There was no buffer available.
         cout << "Producer[" << instanceId << "]: No buffer available" << endl;
      }
      else
      {
         // Of cource you will put some more meaningful content into the buffer.
         // But for the sake of this example, I will just put an incrementing counter.
         counter++;
         cout << "Producer[" << instanceId << "] Buffer Content: " << counter << endl;
         memcpy(videoBufferPtr->getDataPtr(), &counter, sizeof(counter));
      }

      sleep(1);
   }

   // The Producer cannot destroy its VideoBuffers while the Consumer is using them.
   // The destructor of the Producer will wait until the Consumer has released the Buffers.
   // It will timeout eventually.
   // You have more control over the shutdown, by calling startTermination(),
   // which will order the Consumer to release the Buffers.
   // Once isTerminated() returns true, the Buffers are released
   // and the destructor of the Producer will return immediately.
   simpleProducerPtr->startTermination();
   while (!simpleProducerPtr->isTerminated())
   {
      cout << "Waiting for Consumer to release Buffers" << endl;
      sleep(1);
   }

   delete simpleProducerPtr;

   SimpleProducer::terminate();

   cout << "Done!!!" << endl;
   return 0;
}
