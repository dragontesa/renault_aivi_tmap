#ifndef SETTINGMAPCHARSIZEVIEWMEDIATOR_H
#define SETTINGMAPCHARSIZEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingMapCharSizeViewMediator : public AbstractViewMediator
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
        ViewSizeChanged,
        UpdateView,
        RequestLang,
    };
    Q_INVOKABLE SettingMapCharSizeViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool mChanged;

};

}

#endif // SETTINGMAPCHARSIZEVIEWMEDIATOR_H
