#ifndef SETTINGMAPVIEWMEDIATOR_H
#define SETTINGMAPVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingMapViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        CreateView,
        SelectZone1Title,
        RequestSetting,
        SaveSetting,
        HideKeyboard,
        CloseView,
        ClosedView,
        ResetCsw,
        RegistCsw,
        Deactivated,
        RequestLang,
    };
    Q_INVOKABLE SettingMapViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool mRg;

};

}

#endif // SETTINGMAPVIEWMEDIATOR_H
