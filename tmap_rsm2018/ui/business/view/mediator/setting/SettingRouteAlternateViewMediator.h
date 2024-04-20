#ifndef SETTINGROUTEALTERNATEVIEWMEDIATOR_H
#define SETTINGROUTEALTERNATEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SettingRouteAlternateViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        CreateView,
        ApplySetting,
        BackupSetting,
        SelectZone1Title,
        RequestSetting,
        SaveSetting,
        NotEnoughSettings,
        ModalChanged,
        CloseView,
        ShowView,
        ResetCsw,
        RegistCsw,
        RequestLang,
    };
    Q_INVOKABLE SettingRouteAlternateViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    QString caller() const;
    void close();
    void gotoBack();

    QStringList mPreviousRouteTypes;
    QStringList mRouteTypes;
};

}

#endif // SETTINGROUTEALTERNATEVIEWMEDIATOR_H
