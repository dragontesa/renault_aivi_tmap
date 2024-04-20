#ifndef TFACADE_H
#define TFACADE_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TFacade
    : public QObject
    , public IFacade
{
public:
    TFacade();

    virtual ~TFacade();

public:
    void setCore( IModel* model, IView* view, IController* controller );

    void registerCommand( int noteId, const QMetaObject* commandClassRef );

    void removeCommand( int noteId );

    bool hasCommand( int noteId );

    void registerProxy ( const IProxyPtr& proxy );

    IProxyPtr retrieveProxy ( const QString& proxyName );

    IProxyPtr removeProxy ( const QString& proxyName );

    bool hasProxy( const QString& proxyName );

    void registerMediator( const IMediatorPtr& mediator );

    IMediatorPtr retrieveMediator( const QString& mediatorName );

    IMediatorPtr removeMediator( const QString& mediatorName );

    bool hasMediator( const QString& mediatorName );

    void notifyObservers ( const INotificationPtr& note );

protected:
    IController* mController;
    IModel* mModel;
    IView* mView;
};

}}

#endif // TFACADE_H

