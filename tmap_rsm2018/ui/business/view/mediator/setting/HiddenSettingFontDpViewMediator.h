#ifndef HIDDENSETTINGFONTDPVIEWMEDIATOR_H
#define HIDDENSETTINGFONTDPVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class HiddenSettingFontDpViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        SelectZone1Title,
        CloseView,
        RegistKeyboardInput,
        ShowKeyboard,
        HideKeyboard,
        ChangeFontDp,
        ResetCsw,
        RegistCsw,
    };
    Q_INVOKABLE HiddenSettingFontDpViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // HIDDENSETTINGFONTDPVIEWMEDIATOR_H
