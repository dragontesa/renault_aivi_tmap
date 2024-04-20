#ifndef PROGRESSINDICATORVIEWMEDIATOR_H
#define PROGRESSINDICATORVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class ProgressIndicatorViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
		Mediated = 0
    };

    Q_INVOKABLE ProgressIndicatorViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // PROGRESSINDICATORVIEWMEDIATOR_H
