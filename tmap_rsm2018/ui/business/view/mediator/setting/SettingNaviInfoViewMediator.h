#ifndef SETTINGNAVIINFOVIEWMEDIATOR_H
#define SETTINGNAVIINFOVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingNaviInfoViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( ResultK )
public:
    enum EventK {
        Activated,
        Deactivated,
        SelectZone1Title,
        RequestSetting,
        SaveSetting,
        CloseView,
        CreateView,
        ResetCsw,
        RegistCsw,
        RequestLang,
    };


    enum ResultK {
        ResultAppVersion,
        ResultMapVersion,
        ResultLocalMapVersion,
        ResultLocalNetworkVersion,
        ResultLocalPoiVersion,
        ResultSDIVersion,        
        ResultCompany,
        ResultDebuging,
        ResultEnd,
    };

    Q_INVOKABLE SettingNaviInfoViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // SETTINGNAVIINFOVIEWMEDIATOR_H
