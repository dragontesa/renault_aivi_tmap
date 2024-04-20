/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#include <QDebug>
#include "consumer.h"
#include <QThread>

class consumerThread : public QThread {
private:
Consumer* consumerPtr;
public:
    void setConsumer(Consumer* cptr) {
        qDebug() << "consumerThread::setConsumer";
        consumerPtr = cptr;
    }
protected:
    void run() {
        qDebug() << "consumerThread::run";

        Timeout timeout(300000); // End Consumer after 30 Seconds
        while (!timeout.isReached()) {
        consumerPtr->waitForUpdate();

        vector<int> updateIds;
        consumerPtr->isUpdated(updateIds);
        qDebug() << "consumerThread::updateIds.size: " << updateIds.size();

        for (int i = 0; i < updateIds.size(); i++) {
           int instanceId = updateIds[i];
           int32_t fd = consumerPtr->exchange(instanceId);

           if (fd == -1) {
              cout << "Consumer[" << instanceId << "]: No Buffer available" << endl;
           } else {
              BufferInfo bi = consumerPtr->getBufferInfo(instanceId);

              VideoBufferType videoBuffer(bi.getWidth(), bi.getHeight(), bi.getDepth(), bi.getBpp());
              videoBuffer.attach(fd, bi.getSize(), bi.getWidth()*3);

              uint32_t counter = 0;
              if (videoBuffer.getDataPtr() != NULL)  {
                 std::memcpy(&counter, videoBuffer.getDataPtr(), sizeof(counter));
              }
              qDebug() << "Consumer[" << instanceId << "]: Buffer Content: " << counter;
           }
        }
    }
}
};


consumer* consumer::s_instance = NULL;

consumer::consumer()
{
}
consumer* consumer::sharedInstance() {
    qDebug() << "consumer sharedInstance";
    if (s_instance == NULL) {
        qDebug() << "new consumer";
        s_instance = (consumer*) new consumer();
    }
    return s_instance;
}

void consumer::init(int consumerId) {
    t = new consumerThread;
    qDebug() << "consumer::init";
    consumerPtr = new Consumer(consumerId);
    qDebug() << "consumerPtr->create: " << consumerPtr->create(100, false);
    t->setConsumer(consumerPtr);
}

void consumer::start() {
    if (!consumerPtr) {
        return;
    }
    if (t->isRunning()) {
        return;
    }
    qDebug() << "consumer::start!" ;

    t->start();
}


void consumer::end() {
    delete consumerPtr;
    qDebug() << "consumer_Done!!!" ;
}


