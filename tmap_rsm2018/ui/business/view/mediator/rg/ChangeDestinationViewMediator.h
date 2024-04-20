#ifndef CHANGEDESTINATIONVIEW_H
#define CHANGEDESTINATIONVIEW_H

#include "mediator/AbstractViewMediator.h"

namespace SKT
{

class ChangeDestinationViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        Activated,
        Accepted,
        Canceled,
        UpdateModal,
        RegistCSW,
        CaptureScreen,
    };

    Q_INVOKABLE ChangeDestinationViewMediator(QObject *viewComponent = nullptr);
    virtual ~ChangeDestinationViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    void sendUpdateModal(bool modal);

private:
    void onAccepted(const QVariant& v);
    void onSpcxRouteGuidanceStarted();

private:
    QPoint mDestinationPos;
    bool mNeedUpdateModalFalse;
    bool mUpdateModalTrueCalled = false;
    bool mPopupCreated = false;
};

}

#endif // CHANGEDESTINATIONVIEW_H
