#ifndef FULLSCREENALERTVIEWMEDIATOR_H
#define FULLSCREENALERTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class FullscreenAlertViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
		Mediated = 0
    };

    Q_INVOKABLE FullscreenAlertViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

    bool mAlert = false;
};

}

#endif // FULLSCREENALERTVIEWMEDIATOR_H
