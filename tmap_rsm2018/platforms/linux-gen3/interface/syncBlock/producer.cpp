/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#include "producer.h"
#include "platforms/linux-gen3/interface/syncBlock/include/hmibase/util/DltApplication.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <QDebug>
#include <QImage>
#include <QThread>

using namespace std;
using namespace hmibase::gadget::syncblock2;
using namespace hmibase::gadget::videobuffer;

producer* producer::s_instance = NULL;

producer::producer():m_instanceId(0), simpleProducerPtr(0) {
    qDebug() << "producer producer()";
}

producer::~producer() {
}

producer* producer::sharedInstance() {
    qDebug() << "producer sharedInstance "<< s_instance;
//    if (s_instance == NULL) {
        qDebug() << "new producer";
        s_instance = (producer*) new producer();
//    }
    return s_instance;
}


bool producer::init(int instanceId) {
   qDebug()<< "Producer-Version: " << SimpleProducer::getVersion();
//   if (m_instanceId != instanceId) {
       qDebug() << "producer start(mInstanceId: " << instanceId << ")";
       m_instanceId = instanceId;

//        SimpleProducer::initialize(765, SOCKET_PRODUCER);
//    }
   m_isTerminate = false;
}

//#include "QDateTime"
//qint64 tmpTime = 0;
//int AccTime = 0;
//int gadget_fps = 0;
void producer::setBuffer(QImage data) {
    if(m_isTerminate){
        qDebug() << "producer m_isTerminate";
        return;
    }

//    qint64 sec = QDateTime::currentMSecsSinceEpoch();
//    if(tmpTime != 0)
//    {
//        if(AccTime > 1000)
//        {
//             qDebug() << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Gadget fps [ " << gadget_fps << " ]";
//             AccTime =0;
//             gadget_fps = 0;
//        }
//    }

//    AccTime += sec - tmpTime;
//    tmpTime = sec;
//    gadget_fps++;

    qDebug() << "producer setBuffer: " << data.byteCount() << ")";
    uint16_t width  = (uint16_t)data.width();
    uint16_t height = (uint16_t)data.height();
    uint16_t depth  = (uint16_t)data.depth();
    uint16_t bpp    = (uint16_t)data.bitPlaneCount();

    if (!m_instanceId)
        return;

    if((!simpleProducerPtr) || (simpleProducerPtr->getBufferInfo().getWidth() != width || simpleProducerPtr->getBufferInfo().getHeight() != height)){
        // This will create VideoBuffers and set up communication with the consumer
        simpleProducerPtr = new SimpleProducer(m_instanceId, width, height, depth, bpp);
        if (simpleProducerPtr == NULL)
            return;

        uint32_t size = simpleProducerPtr->getBufferInfo().getSize();
    }

    auto buffer = simpleProducerPtr->getBuffer();
    if (buffer) {
        std::memcpy(buffer->getDataPtr(), (uint8_t*)data.constBits(), data.byteCount());
        simpleProducerPtr->exchange();
    }
    else {
        VideoBuffer* videoBufferPtr = simpleProducerPtr->exchange();

        if (videoBufferPtr == NULL) {
    //       qDebug()<< "Producer[" << m_instanceId << "]: No buffer available";
        } else {
           qDebug()<< "Producer[" << m_instanceId << "] Buffer Content: " << data;
    //       QImage mirroedImage = data.mirrored(0, 1);

           std::memcpy(videoBufferPtr->getDataPtr(), (uint8_t*)data.constBits(), data.byteCount());

           //qDebug() << "[Gadget][VideoBuffer]memcpy after m_instanceId: " << m_instanceId << " thread: " << QThread::currentThread();
        }
        //sleep(1);
    }
}

void producer::startTermination() {
    simpleProducerPtr->startTermination();
    while (!simpleProducerPtr->isTerminated())
    {
       qDebug()<< "Waiting for Consumer to release Buffers";

       sleep(1);
    }

    simpleProducerPtr->terminate();

    if(simpleProducerPtr != NULL)
    {
        qDebug()<< " delete simpleProducerPtr ";
        m_isTerminate = true;
        delete simpleProducerPtr;
        simpleProducerPtr = NULL;
    }
    qDebug()<< "Producer_Done!!!";
}

