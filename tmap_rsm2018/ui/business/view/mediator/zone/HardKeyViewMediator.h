#ifndef HARDKEYMEDIATOR_H
#define HARDKEYMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class HardKeyViewMediator : public AbstractViewMediator
{
    Q_OBJECT       
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        RecentTileButtonClicked,
        CswLeftClicked,
        CswRightClicked,
        CswUpClicked,
        CswDownClicked,
        CswSpinCcwClicked,
        CswSpinCwClicked,
        CswOkClicked,
        CswBackClicked,
        CswOptionClicked,
    };

    Q_INVOKABLE HardKeyViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // SDIVIEWMEDIATOR_H
