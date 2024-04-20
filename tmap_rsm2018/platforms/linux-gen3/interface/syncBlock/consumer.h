#ifndef CONSUMER_H
#define CONSUMER_H
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "consumer.h"
#include "hmibase/util/DltApplication.h"
#include "hmibase/gadget/videobuffer/VideoBufferType.h"
#include "hmibase/gadget/syncblock2/Consumer.h"
#include "hmibase/util/Timeout.h"

using namespace std;
using namespace hmibase::util;
using namespace hmibase::gadget::syncblock2;
using namespace hmibase::gadget::videobuffer;
class consumerThread;
class consumer
{
public:
    consumer();
    void init(int consumerId);
    void start();
    void end();
    static consumer* sharedInstance();

private:
    static consumer* s_instance;
    Consumer* consumerPtr;
    consumerThread* t;
};

#endif // CONSUMER_H
