#ifndef IOBSERVER_H
#define IOBSERVER_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IObserver
{
public:
    typedef std::function<bool(const INotificationPtr&)> NotifyMethod;

    virtual ~IObserver() {}

    virtual QVariant getObserverId() = 0;

    virtual void setObserverId( const QVariant& id ) = 0;

    virtual QString getObserverName() = 0;

    virtual void setObserverName( const QString& name ) = 0;

    virtual void setNotifyMethod( const NotifyMethod& method ) = 0;

    virtual void setNotifyContext( const QObjectPtr& context ) = 0;

    virtual void notifyObserver( const INotificationPtr& note ) = 0;

    virtual bool compareNotifyContext( const QObjectPtr& object ) = 0;
};

}}

#endif // IOBSERVER_H

