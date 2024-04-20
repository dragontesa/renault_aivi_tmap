#ifndef TBTVIEWMEDIATOR_H
#define TBTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"

namespace SKT {

class TbtViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        ArrowClicked,
        VisibleChanged,
        RegistCsw,
        RemoveCsw,
        SelectCsw
    };

    Q_INVOKABLE TbtViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void updateView( const RgDataVo& rgDataVo );

    void updateSecondTbtAndArrowVisiblilty();

    QVariantMap makeModel(
        const QString& img, const QString& name,
        const QVariantList& distance, const QString& progress
        );

    QObject* mFirst;
    QObject* mSecond;
    QObject* mLeftArrow;
    QString mFirstImg;
    QString mSecondImg;

    MapViewMode::ModeK mCurMapViewMode;
    MapViewMode::ReasonK mCurMapViewModeReason;

    QStringList mLastDistances;
    bool mSummaryTbtListVisible;
    bool mCrossImageVisible;
    bool mIsInHighway;
    int mTest;

    struct NotifiableUpdate {
        QString firstDistance;
        int firstTurnType = -1;
        QString firstPointName;
        QString secondDistance;
        int secondTurnType = -1;
    };

    NotifiableUpdate mLastState;
    NotifiableUpdate mUpdate;
    bool mIsGadgetShown = false;
    int mGlobalMapViewMoveMode = 0;
    int mTurnType1 = 0;
    int mTurnType2 = 0;

    static const int DIRNAME_TOGGLE_DELAY = 5000;
    QStringList mDirNames;
    int mCurrentDirNameIndex = 0;
    QElapsedTimer mDirNameToggleTimer;
};

}

#endif // TBTVIEWMEDIATOR_H
