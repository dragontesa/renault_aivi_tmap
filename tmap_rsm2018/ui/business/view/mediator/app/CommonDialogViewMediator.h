#ifndef COMMONDIALOGVIEWMEDIATOR_H
#define COMMONDIALOGVIEWMEDIATOR_H

#include "../view/mediator/AbstractViewMediator.h"

namespace SKT
{

class CommonDialogViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        Activated,
        Deactivated,
        Accepted,
        Canceled,
        MenuClicked,
        RegistCSW,
        CaptureScreen
    };

    Q_INVOKABLE CommonDialogViewMediator(QObject* viewComponent = nullptr);
    virtual ~CommonDialogViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void sendUpdateModal(bool modal);
    void onAccepted(const QVariant&);
    void onCanceled(const QVariant&);
    void onMenuClicked(const QVariant&);
    void onSystemPopupCreated();
    void onSpcxRouteGuidanceStarted();
    bool isMapUpdateView() const;

private:
    bool mNeedUpdateModalFalse = false;
    QString mTitle;
    QString mMessage;
};

}

#endif // COMMONDIALOGVIEWMEDIATOR_H
