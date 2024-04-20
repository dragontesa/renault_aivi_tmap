#ifndef SUMMARYTBTVIEWMEDIATOR_H
#define SUMMARYTBTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class SummaryTbtViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        ArrowClicked,
        VisibleChanged,
        ClosingTouchAreaClicked,
        RegistCsw,
        RemoveCsw,
        SelectCsw
    };

    Q_INVOKABLE SummaryTbtViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void updateView( const RgDataVo& rgDataVo );

    QVariantMap makeModel(
        const QString& img, const QString& name,
        const QVariantList& distance, const QString& progress,
        bool noName
        );

    QObject* mList;
    MapViewMode::ModeK mCurMapViewMode;
    MapViewMode::ReasonK mCurMapViewModeReason;
    QStringList mLastDistances;
    bool mCrossImageVisible = false;
    bool mHighwayVisible = false;
    int mTest = -1;
    bool mIsInHighway = false;
    bool mIsOnline = false;
};

}

#endif // SUMMARYTBTVIEWMEDIATOR_H
