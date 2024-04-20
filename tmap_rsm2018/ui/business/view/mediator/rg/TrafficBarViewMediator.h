#ifndef TRAFFICBARVIEWMEDIATOR_H
#define TRAFFICBARVIEWMEDIATOR_H
#include "mediator/AbstractViewMediator.h"
#include "mediator/app/MapViewMode.h"
#include "RgDataVo.h"

namespace SKT {

class TrafficBarViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( Type )

public:
    enum EventK {
        Mediated = 0
    };
    enum Type {
        App_L820x1024 = 0,
        App_L800x480,
        Widget_XXXL,
        Widget_XXL,
        Widget_XL,
        Widget_L,
        Widget_M,
        Widget_S
    };

    Q_INVOKABLE  TrafficBarViewMediator(QObject* viewComponent = nullptr);
    QList<int> listNotificationInterests();

private:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );
    void setTotalRouteDistance(uint distance);
    void updateTrafficGuidance(int routId=0);

    QObject* mTrafficbarView;

    MapViewMode::ModeK mCurMapViewMode;
    MapViewMode::ReasonK mCurMapViewModeReason;
    bool mIsOnline;
    bool mSummaryTbtVisible;
    bool mHighwayVisible;
    bool mHighwayTbtListVisible;
    bool mCrossImageVisible;

};

}
#endif // TRAFFICBARVIEWMEDIATOR_H
