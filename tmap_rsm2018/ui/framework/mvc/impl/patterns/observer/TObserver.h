#ifndef TOBSERVER_H
#define TOBSERVER_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TObserver
    : public IObserver
{
public:
    TObserver( const IObserver::NotifyMethod& notifyMethod, const QObjectPtr& notifyContext );

    virtual ~TObserver();

    QVariant getObserverId();

    void setObserverId( const QVariant& id );

    QString getObserverName();

    void setObserverName( const QString& name );
    
    void setNotifyMethod( const IObserver::NotifyMethod& notifyMethod );
    
    void setNotifyContext( const QObjectPtr& notifyContext );
    
    IObserver::NotifyMethod getNotifyMethod();
    
    QObjectPtr getNotifyContext();

    virtual void notifyObserver( const INotificationPtr& note );
    
    bool compareNotifyContext( const QObjectPtr& object );

private:
    QVariant mObserverId;
    QString mObserverName;
    IObserver::NotifyMethod mNotifyMethod;
    QObjectPtr mNotifyContext;
};

}}

#endif // TOBSERVER_H

