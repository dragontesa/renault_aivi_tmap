#ifndef TESTTABTVIEWMEDIATOR_H
#define TESTTABVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TestSearchTabViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,        
        CreateView,
        CloseView,
        ResetCsw,
        RegistCsw,
        TabPageActivated
    };
    Q_INVOKABLE TestSearchTabViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // TESTTABVIEWMEDIATOR_H
