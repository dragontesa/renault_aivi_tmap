#ifndef IMEDIATOR_H
#define IMEDIATOR_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT IMediator
    : public virtual INotifier
{
public:
    virtual ~IMediator() {}

    virtual QVariant getMediatorId() = 0;

    virtual void setMediatorId( const QVariant& id ) = 0;

    virtual QString getMediatorName() = 0;

    virtual void setMediatorName( const QString& name ) = 0;

    virtual QObject* getViewComponent() = 0;

    virtual void setViewComponent( QObject* viewComponent ) = 0;

    virtual QList<int> listNotificationInterests() = 0;

    virtual bool handleNotification( const INotificationPtr& notification ) = 0;

    virtual void onBeforeRegister() = 0;

    virtual void onRegister() = 0;

    virtual void onRemove() = 0;
};

}}

#endif // IMEDIATOR_H

