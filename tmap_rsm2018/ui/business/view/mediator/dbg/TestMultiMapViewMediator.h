#ifndef TESTMULTIMAPVIEWMEDIATOR_H
#define TESTMULTIMAPVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class TestMultiMapViewMediator : public AbstractViewMediator
{
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Navigation,
        DrawRouteAllClickedUsingApi,
        DrawRouteAllClickedUsingMapViewQuick,
        MapViewOnDestruction
    };
    Q_INVOKABLE TestMultiMapViewMediator( QObject* viewComponent = nullptr );

    ~TestMultiMapViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

private:
    bool handleNotification( const mvc::INotificationPtr& note );

    QList<QObject*> mMapViewList;
};

}

#endif // TESTMULTIMAPVIEWMEDIATOR_H
