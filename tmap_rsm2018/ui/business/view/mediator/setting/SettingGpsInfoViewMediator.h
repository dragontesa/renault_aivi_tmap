#ifndef SETTINGGPSINFOVIEWMEDIATOR_H
#define SETTINGGPSINFOVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingGpsInfoViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        SelectZone1Title,
        RequestSetting,
        SaveSetting,
        CloseView,
        CreateView,
        RequestLang,
        ResetCsw,
        RegistCsw,
    };
    Q_INVOKABLE SettingGpsInfoViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // SETTINGGPSINFOVIEWMEDIATOR_H
