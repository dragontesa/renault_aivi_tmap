#ifndef SETTINGAQIDETAILVIEWMEDIATOR_H
#define SETTINGAQIDETAILVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingAQIdetailViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( KindK )

public:
    enum EventK {        
        Activated,
        Deactivated,
        SelectZone1Title,
        SelectSoftkeyIndexMenuMode,
        CloseView,
        CreateView,
        ResetCsw,
        RegistCsw,
        RequestSearch,
        QueryOnlineStatus,
        ShowKeyboard,
        HideKeyboard,
        RequestRouteGuidanceState,
        ShowToast,
        BackPressed,
        RequestLang,
        ShowProgressIndicator,
        HideProgressIndicator,
        RequestGpsTime,
    };


    enum KindK {
        CurrentPos,
        DestinationPos,
    };


    Q_INVOKABLE SettingAQIdetailViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void back();
};

}

#endif // SETTINGAQIDETAILVIEWMEDIATOR_H
