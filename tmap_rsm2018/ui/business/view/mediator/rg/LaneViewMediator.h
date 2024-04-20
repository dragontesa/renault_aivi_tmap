#ifndef LANEVIEWMEDIATOR_H
#define LANEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class LaneViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0
    };

    Q_INVOKABLE LaneViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

private:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

    void updateView( const RgDataVo& rgDataVo );
    void updateLane( int laneIndex, const RgDataVo& rgDataVo );

    static const int LIMIT_LANE_AREA_DISTANCE = 1000;
    static const int CENTER_TBT_SHOW_DISTANCE = 300;

    QString getLaneFilter(int laneInfo, int available);

    MapViewMode::ModeK mCurMapViewMode;
    MapViewMode::ReasonK mCurMapViewModeReason;

    struct NotifiableUpdate {
        NotifiableUpdate() {
            visibleList.append(false);
            visibleList.append(false);
            imagesList.append(QStringList());
            imagesList.append(QStringList());            
            coverImagesList.append(QStringList());
            coverImagesList.append(QStringList());
            distList.append(QString());
            distList.append(QString());
        }
        QList<bool> visibleList;
        QList<QStringList> imagesList;
        QList<QStringList> coverImagesList;
        QList<QString> distList;
    };

    NotifiableUpdate mLastState;
    NotifiableUpdate mUpdate;
    int mTest = -1;
    bool mHighwayVisible = false;
    bool mHiPassLaneVisible = false;
};

}

#endif // LANEVIEWMEDIATOR_H
