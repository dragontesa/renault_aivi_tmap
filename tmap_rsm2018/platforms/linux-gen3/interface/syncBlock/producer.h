#ifndef TPRODUCER_H
#define TPRODUCER_H
#include "interface/guicontrol/guicontroltype.h"
#include <QObject>
#include "SimpleProducer.h"

class producer
{
public:
    producer();
    ~producer();
    bool init(int mInstanceId);
    void setBuffer(QImage data);
    void startTermination();
public:
    static producer* sharedInstance();

private:
    static producer* s_instance;
    SimpleProducer* simpleProducerPtr;
    int m_instanceId;

    bool m_isTerminate;
};

#endif // TPRODUCER_H
