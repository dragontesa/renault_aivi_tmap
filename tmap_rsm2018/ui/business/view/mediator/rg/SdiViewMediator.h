#ifndef SDIVIEWMEDIATOR_H
#define SDIVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "RgUtils.h"
#include "RgProtocol.h"
#include "RgConstants.h"

namespace SKT {

class SdiViewMediator : public AbstractViewMediator
{
    Q_OBJECT   
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        VisibleChanged
    };	

    Q_INVOKABLE SdiViewMediator( QObject* viewComponent = nullptr );

    ~SdiViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

    void applySdiData(RgDataVo& rgDataVo);

    void setSignNumber(RgConstants::SdiSignTypeK signType, int limit);
    bool setSectionSpeedAverageNumber(RgConstants::SdiSectionSpeedNumberType numberType, int average);
    bool setSectionSpeedTime(RgConstants::SdiSectionSpeedNumberType numberType, int time);
    bool setSectionSpeed(RgConstants::SdiSignTypeK& signType, QString& signImg, bool isChangeableSpeedType, int blockSpeed, int blockTime, int averageSpeed, int speedLimit);
    void setRemainDistanceNumber(bool& remainDistVisible, int sdiDist, int sdiBlockDist, int sdiType);

    void reset();

    void synchronizeWithGadget( const QVariant& data );

    int mLastSdiType = RgSdiCode::UNKNOWN;
    int mLastSdiPointX = 0;
    int mLastSdiPointY = 0;
    int mLastPosX = 0;
    int mLastPosY = 0;

    // layout
    //     sign wrap(circle image)   block wrap
    //        sign                      text
    //        number                    number
    //
    //     rest far

    // 제한속도구간 영역
    QObject* mSectionSpeedWrap = nullptr;

    // 제한속도표식 영역
    QObject* mSignWrap = nullptr;
    // 표식
    QObject* mSignImage = nullptr;

    // 남은거리
    QObject* mRemainDistWrap = nullptr;
    QObject* mRemainDistText = nullptr;

    int mCurrentRecommendedSpeed = -1;
    int mRest = -1;
    bool mAlertState = false;

    struct NotifiableUpdate {
        QString speedLimit;
        int signType = -1;
        QString signImage;
        QString sectionSpeedAverage;
        QString sectionSpeedTime;
        bool sectionSpeedToggleAverageAndTime = false;
        RgConstants::SdiSectionSpeedNumberType sectionSpeedAverageNumberType = RgConstants::SdiNormal;
        bool sectionSpeedVisible = false;
        bool sectionSpeedAverageVisible = false;
        bool sectionSpeedTimeVisible = false;
        bool signVisible = false;
        bool remainDistVisible = false;
        QString remainDist;
    };

    NotifiableUpdate mLastState;
    NotifiableUpdate mUpdate;
    int mTest = 0;
    bool mSdiLog = false;
};

}

#endif // SDIVIEWMEDIATOR_H
