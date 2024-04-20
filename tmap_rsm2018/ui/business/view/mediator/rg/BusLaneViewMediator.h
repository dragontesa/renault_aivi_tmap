#ifndef BUSLANEVIEWMEDIATOR_H
#define BUSLANEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class BusLaneViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0
    };

    Q_INVOKABLE BusLaneViewMediator(QObject* viewComponent = nullptr);

    QList<int> listNotificationInterests();

protected:
    void updateBusLane(int busLaneId);

private:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    MapViewMode::ModeK mCurMapViewMode;
    MapViewMode::ReasonK mCurMapViewModeReason;

    QObject* mBusLane;
    int mBusLaneOldId;
};

}
#endif // BUSLANEVIEWMEDIATOR_H
