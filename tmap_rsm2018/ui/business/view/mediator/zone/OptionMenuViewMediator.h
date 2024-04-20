#ifndef OPTIONMENUVIEWMEDIATOR_H
#define OPTIONMENUVIEWMEDIATOR_H

#include "../AbstractViewMediator.h"

namespace SKT
{

class OptionMenuViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        SelectMenu,
        SelectMenuIndex,
        ReleaseMenu,
        ReleaseMenuIndex,
        RegistCSW,
        CaptureScreen,
    };
    Q_INVOKABLE OptionMenuViewMediator(QObject* viewComponent = nullptr);

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // OPTIONMENUVIEWMEDIATOR_H
