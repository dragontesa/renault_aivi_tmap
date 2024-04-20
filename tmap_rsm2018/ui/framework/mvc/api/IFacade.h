#ifndef IFACADE_H
#define IFACADE_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IFacade
    : public virtual INotifier
{
public:
    virtual ~IFacade() {}

    virtual void registerProxy( const IProxyPtr& proxy )= 0;

    virtual IProxyPtr retrieveProxy( const QString& proxyName )= 0;

    virtual IProxyPtr removeProxy( const QString& proxyName )= 0;

    virtual bool hasProxy( const QString& proxyName )= 0;

    virtual void registerCommand( int noteId, const QMetaObject* commandClassRef )= 0;

    virtual void removeCommand( int noteId )= 0;

    virtual bool hasCommand( int noteId )= 0;

    virtual void registerMediator( const IMediatorPtr& mediator )= 0;

    virtual IMediatorPtr retrieveMediator( const QString& mediatorName )= 0;

    virtual IMediatorPtr removeMediator( const QString& mediatorName )= 0;

    virtual bool hasMediator( const QString& mediatorName )= 0;

    virtual void notifyObservers( const INotificationPtr& note )= 0;
};

}}

#endif // IFACADE_H
