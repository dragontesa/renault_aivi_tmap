#ifndef SETTINGCARICONVIEWMEDIATOR_H
#define SETTINGCARICONVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingCarIconViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        SelectZone1Title,
        RequestSetting,
        SaveSetting,
        CloseView,
        ResetCsw,
        RegistCsw,
        RequestLang,
    };
    Q_INVOKABLE SettingCarIconViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // SETTINGCARICONVIEWMEDIATOR_H
