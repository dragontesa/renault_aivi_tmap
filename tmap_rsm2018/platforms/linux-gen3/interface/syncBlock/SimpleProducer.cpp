/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#include "SimpleProducer.h"
#include "platforms/linux-gen3/interface/syncBlock/include/hmibase/util/DltLogger.h"
#include "platforms/linux-gen3/interface/syncBlock/include/hmibase/gadget/syncblock2/Producer.h"
#include "platforms/linux-gen3/interface/syncBlock/include/hmibase/gadget/videobuffer/VideoBufferType.h"
#include <unistd.h>
#include <iostream>
#include <QDebug>
using namespace std;
using namespace hmibase::gadget::syncblock2;
using namespace hmibase::gadget::videobuffer;

Producer* SimpleProducer::smProducerPtr = NULL;

const char* SimpleProducer::getVersion()
{
   return Producer::getVersion();
}


void SimpleProducer::initialize(int key, ProducerType producerType)
{
   qDebug() << "Producer::initialize #1";
   if (smProducerPtr == NULL)
   {
      qDebug() << "Producer::initialize #2 ";
      smProducerPtr = new Producer(key, producerType);
      if (smProducerPtr) {
          qDebug() << "Producer::initialize( " << key << ", " << producerType << " )";
      }
   }
}


void SimpleProducer::terminate()
{
   if (smProducerPtr != NULL)
   {
      delete smProducerPtr;
      smProducerPtr = NULL;
   }
}


SimpleProducer::SimpleProducer(int instanceId, uint16_t width, uint16_t height, uint16_t depth, uint16_t bpp)
   : mInstanceId(instanceId)
{
   if (smProducerPtr == NULL)
      smProducerPtr = new Producer(765, SOCKET_PRODUCER);

   if (smProducerPtr != NULL)
   {
      uint32_t size = 0;
      smProducerPtr->attach(mInstanceId);
      for (int i = 0; i < 3; i++)
      {
         VideoBuffer* videoBufferPtr = new VideoBufferType(width, height, depth, bpp);
         if (videoBufferPtr) {
             qDebug() << "videoBufferPtr init ( " << width << ", " << height << ", " << depth << ", " << bpp << " )" ;
         }
         int id = videoBufferPtr->create();

         qDebug() << "videoBufferPtr->create( " << id << " )" ;

         mBuffers[id] = videoBufferPtr;
         size = videoBufferPtr->getDataSize();
         smProducerPtr->addBufHandle(i, id);
      }

      mBufferInfo = BufferInfo(size, width, height, depth, bpp, width*3);
      smProducerPtr->setBufferInfo(mBufferInfo);
   }
}

SimpleProducer::~SimpleProducer()
{
   startTermination();
   qDebug() << "::::::  SimpleProducer::~SimpleProducer()";
   while (!isTerminated())
   {
      usleep(10000); // Sleep 10 Milliseconds
   }

   for (std::map<int, VideoBuffer*>::iterator iter = mBuffers.begin(); iter != mBuffers.end(); iter++)
   {
      VideoBuffer* bufferPtr = iter->second;
      if (bufferPtr != NULL)
      {
          qDebug() << "::::::  SimpleProducer::~SimpleProducer()" << iter->first;

         delete bufferPtr;
      }
   }

   mBuffers.clear();
}


const BufferInfo& SimpleProducer::getBufferInfo() const
{
   return mBufferInfo;
}

VideoBuffer* SimpleProducer::getBuffer()
{
    if (mNextBufferId == -1) {
        return NULL;
    }

    return mBuffers[mNextBufferId];
}


VideoBuffer* SimpleProducer::exchange()
{
   if (smProducerPtr != NULL)
   {
      auto debug = mInstanceId == 82;

      smProducerPtr->attach(mInstanceId);
      int id = smProducerPtr->exchange();

      if (id != -1)
      {
         mNextBufferId = id;
         return mBuffers[id];
      }
   }
   return NULL;
}


bool SimpleProducer::isTerminated()
{
   if (smProducerPtr != NULL)
   {
      smProducerPtr->attach(mInstanceId);
      delete smProducerPtr;
      smProducerPtr = NULL;

      return true;
      return smProducerPtr->isTerminated();
   }

   return true;
}


void SimpleProducer::startTermination()
{
   qDebug() << "::::::  startTermination()";
   if (smProducerPtr != NULL)
   {
       qDebug() << ":::::: ~SimpleProducer startTermination() #2" <<mInstanceId;

      smProducerPtr->attach(mInstanceId);
      smProducerPtr->startTermination();

      delete smProducerPtr;
      smProducerPtr = NULL;
   }
}

