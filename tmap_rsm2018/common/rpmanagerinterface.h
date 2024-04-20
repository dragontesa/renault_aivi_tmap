#ifndef RPMANAGERINTERFACE_H
#define RPMANAGERINTERFACE_H

#include <QObject>
#include "rprequest.h"

namespace SKT {

class RPRequestHandle
{
public:
    virtual ~RPRequestHandle() {}
    virtual RPRequestPtr request() = 0;
    virtual QObject* qobject() = 0;

signals:
    // just hint
    void finished();
};

class RPManagerInterface
{
public:
    virtual ~RPManagerInterface() {}
    virtual bool isSync() const = 0;
    virtual RPRequestHandle* createRequest(RPRequestPtr req) = 0;
    virtual void requestCommand(RPRequestHandle* req) = 0;
};

}

Q_DECLARE_INTERFACE(SKT::RPRequestHandle, "com.skt.tmap.route.RPRequestHandle/1.0")
Q_DECLARE_INTERFACE(SKT::RPManagerInterface, "com.skt.tmap.route.RPManagerInterface/1.0")

#endif // RPMANAGERINTERFACE_H
