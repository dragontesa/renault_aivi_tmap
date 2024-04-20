#ifndef TMEDIATOR_H
#define TMEDIATOR_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TMediator
    : public QObject
    , public IMediator
    , TShareable<TMediator>
{
public:
    TMediator( QObject* viewComponent = nullptr );

    virtual ~TMediator() {}

    virtual QVariant getMediatorId();

    virtual void setMediatorId( const QVariant& id );

    virtual QString getMediatorName();

    virtual void setMediatorName( const QString& name );

    virtual void setViewComponent( QObject* viewComponent );

    virtual QObject* getViewComponent();

    virtual QList<int> listNotificationInterests();

    virtual bool handleNotification( const INotificationPtr& note );

    void onBeforeRegister();

    virtual void onRegister();

    virtual void onRemove();

protected:
    QVariant mMediatorId;
    QString mMediatorName;
    QObject* mViewComponent;
};

}}

#endif // TMEDIATOR_H

