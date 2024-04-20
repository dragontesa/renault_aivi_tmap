#ifndef SETTINGSPEEDRESPONSEMAPVIEWMEDIATOR_H
#define SETTINGSPEEDRESPONSEMAPVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingSpeedResponseMapViewMediator : public AbstractViewMediator
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
        ResetCsw,
        RegistCsw,
        RequestLang,
    };
    Q_INVOKABLE SettingSpeedResponseMapViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool mMapUse;
    bool mMapUseChanged;
    int  mDistance;
    bool mDistanceChanged;
};

}

#endif // SETTINGSPEEDRESPONSEMAPVIEWMEDIATOR_H
