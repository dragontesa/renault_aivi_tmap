#ifndef TPROXY_H
#define TPROXY_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TProxy
    : public QObject
    , public IProxy
{
public:
    TProxy( const QVariant& data = QVariant() );

    virtual ~TProxy();

    QString getProxyName();

    virtual void setProxyName( const QString& name );

    void setData( const QVariant& data );

    QVariant getData();

    virtual void onBeforeRegister();

    virtual void onRegister();

    virtual void onRemove();

protected:
    QString mProxyName;
    QVariant mData;
};

}}

#endif // TPROXY_H

