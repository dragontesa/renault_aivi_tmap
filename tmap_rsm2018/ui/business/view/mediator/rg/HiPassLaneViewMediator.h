#ifndef HIPASSLANEVIEWMEDIATOR_H
#define HIPASSLANEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class HiPassLaneViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        VisibleChanged
    };

    Q_INVOKABLE HiPassLaneViewMediator( QObject* viewComponent = nullptr );

    ~HiPassLaneViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void applyRgData( const RgDataVo& rgDataVo );

    MapViewMode::ModeK mCurrMapViewMode;
    RgDataVo mOldRgDataVo;
};

}

#endif // HIPASSLANEVIEWMEDIATOR_H
