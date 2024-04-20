#ifndef IVIEW_H
#define IVIEW_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IView
{
public:
    virtual ~IView() {}

    virtual void registerObserver( int noteId, const IObserverPtr& observer ) = 0;

    virtual void removeObserver( int noteId, const QObjectPtr& notifyContext ) = 0;

    virtual void notifyObservers( const INotificationPtr& note ) = 0;

    virtual void registerMediator( const IMediatorPtr& mediator ) = 0;

    virtual IMediatorPtr retrieveMediator( const QString& mediatorName ) = 0;

    virtual IMediatorPtr removeMediator( const QString& mediatorName ) = 0;

    virtual bool hasMediator( const QString& mediatorName ) = 0;
};

}}

#endif // IVIEW_H

