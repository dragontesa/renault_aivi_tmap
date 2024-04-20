#ifndef IPROXY_H
#define IPROXY_H

#include "INotifier.h"

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IProxy
    : public virtual INotifier
{
public:
    virtual ~IProxy() {}

    virtual QString getProxyName() = 0;

    virtual void setProxyName( const QString& name ) = 0;

    virtual void setData( const QVariant& data ) = 0;

    virtual QVariant getData() = 0;

    virtual void onBeforeRegister() = 0;

    virtual void onRegister() = 0;

    virtual void onRemove() = 0;
};

}}

#endif // IPROXY_H

