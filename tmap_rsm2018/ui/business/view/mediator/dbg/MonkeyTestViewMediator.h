#ifndef MONKEYTESTVIEWMEDIATOR_H
#define MONKEYTESTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class MonkeyTestViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        RandClickTestClicked,
        ShowKeyboard,
        HideKeyboard,
        RegistKeyboardInput
    };
    Q_INVOKABLE MonkeyTestViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

    void monkeyTestRandClick();

    int mRepeatsPerSecond = 10;
    int mDurationMins = 60 * 3;
};

}

#endif // MONKEYTESTVIEWMEDIATOR_H
