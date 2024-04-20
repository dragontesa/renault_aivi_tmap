#ifndef FACADE_H
#define FACADE_H

namespace SKT
{

class Notification : public mvc::TNotification
{
public:
    Notification( QObject* source, int id, const QVariant& body, const QVariant& observerId = QVariant() );
};

class TMAP_RSM_UI_DLLSHARED_EXPORT Facade
    : public mvc::TFacade
{
    Q_OBJECT
public:
    static Facade* instance();

    void initialize();

    void terminate();

    template<typename Proxy>
    QSharedPointer<Proxy> retrieveProxy( const QString& name = QString() ) {
        QString proxyName = name;
        if ( proxyName.isEmpty() ) {
            proxyName = Proxy::staticMetaObject.className();
        }

        return qSharedPointerDynamicCast<Proxy>(TFacade::retrieveProxy( proxyName ));
    }

    template<typename Mediator>
    QSharedPointer<Mediator> retrieveMediator( const QString& name = QString() ) {
        QString mediatorName = name;
        if ( mediatorName.isEmpty() ) {
            mediatorName = Mediator::staticMetaObject.className();
        }

        return qSharedPointerDynamicCast<Mediator>(TFacade::retrieveMediator( mediatorName ));
    }

    void registerCommand( int noteId, const QMetaObject* commandClassRef );

    void registerObserver( int noteId, const mvc::IObserverPtr& observer );

    void removeObserver( int noteId, const QObjectPtr& notifyContext );

    void sendNotification( QObject* source, int noteId, const QVariant& body = QVariant(), const QVariant& observerId = QVariant(), const QString& type = QString() );

private:
    Facade();

    void sendNotification( int noteId, const QVariant& body = QVariant(), const QVariant& observerId = QVariant(), const QString& type = QString() );

    mvc::TModel mModel;
    mvc::TView mView;
    mvc::TController mController;
};

}

#endif // FACADE_H
