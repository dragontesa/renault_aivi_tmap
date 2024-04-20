#ifndef SETTINGEVCHARGETYPEVIEWMEDIATOR_H
#define SETTINGEVCHARGETYPEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingEvChargeTypeViewMediator : public AbstractViewMediator
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
    Q_INVOKABLE SettingEvChargeTypeViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool mChanged;

};

}

#endif // SETTINGEVCHARGETYPEVIEWMEDIATOR_H
