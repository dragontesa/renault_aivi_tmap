#ifndef MAPMODEOPTIONVIEWMEDIATOR_H
#define MAPMODEOPTIONVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class MapModeOptionViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        Activated,
        Deactivated,
        RequestSetting,
        SaveSetting,
        CancelRouteGuidance,
        RequestRouteSummary,
        OtherRouteClicked,
        ModalChanged,
        ResetCsw,
        RegistCsw,
        RemoveCsw,
        CaptureScreen,
        RequestLang,
        ShowToast,
        QueryOnlineStatus,
    };
    Q_INVOKABLE MapModeOptionViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    int mRouteOption = 1;
};

}

#endif // MAPMODEOPTIONVIEWMEDIATOR_H
