#ifndef IMODEL_H
#define IMODEL_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IModel
{
public:
    virtual ~IModel() {}

    virtual void registerProxy( const IProxyPtr& proxy ) = 0;

    virtual IProxyPtr retrieveProxy( const QString& proxyName ) = 0;

    virtual IProxyPtr removeProxy( const QString& proxyName ) = 0;

    virtual bool hasProxy( const QString& proxyName ) = 0;
};

}}

#endif // IMODEL_H

