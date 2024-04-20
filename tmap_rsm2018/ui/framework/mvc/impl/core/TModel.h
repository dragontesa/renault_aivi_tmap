#ifndef TMODEL_H
#define TMODEL_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TModel
    : public QObject
    , public IModel
{
public:
    TModel();

    virtual ~TModel();

public:
    void registerProxy( const IProxyPtr& proxy );

    IProxyPtr retrieveProxy( const QString& proxyName );

    bool hasProxy( const QString& proxyName );

    IProxyPtr removeProxy( const QString& proxyName );

protected:
    QMap<QString, IProxyPtr> mProxyMap;
};

}}

#endif // TMODEL_H

