#ifndef SELFTESTVIEWMEDIATOR_H
#define SELFTESTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class SelfTestViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        CreateView,
        CloseView,
        MouseClick,
        MousePress,
        MouseRelease,
        MouseMove,
        RegistKeyboardInput,
        ShowKeyboard,
        HideKeyboard,
        CloseAllView,
        SaveScenario,
        GetScenario,
    };
    Q_INVOKABLE SelfTestViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // SELFTESTVIEWMEDIATOR_H
