#ifndef SETTINGMAINVIEWMEDIATOR_H
#define SETTINGMAINVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingMainViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        CreateView,
        UpdateView,
        RequestSetting,
        SaveSetting,
        InitSetting,
        SelectZone1Title,
        ShowToast,
        ModalChanged,
        CloseView,
        ResetCsw,
        RegistCsw,
        ViewSizeChanged,
        RequestLang,
        RequestViewList,
    };
    Q_INVOKABLE SettingMainViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool isVisible() const;
    bool isActive() const;

};

}

#endif // SETTINGMAINVIEWMEDIATOR_H
