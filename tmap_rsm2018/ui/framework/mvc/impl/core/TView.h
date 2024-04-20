#ifndef TVIEW_H
#define TVIEW_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TView
    : public QObject
    , public IView
{
public:
    TView( );

    virtual ~TView();

public:
    void registerObserver ( int noteId, const IObserverPtr& observer );

    void notifyObservers( const INotificationPtr& note );

    void removeObserver( int noteId, const QObjectPtr& notifyContext );

    void registerMediator( const IMediatorPtr& mediator );

    IMediatorPtr retrieveMediator( const QString& mediatorName );

    IMediatorPtr removeMediator( const QString& mediatorName );

    bool hasMediator( const QString& mediatorName );

protected:
    QMap<QString,IMediatorPtr> mMediatorMap;
    QMap<int,QList<IObserverPtr>> mObserverMap;
};

}}

#endif // TVIEW_H

