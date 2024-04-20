#ifndef RGCONSTANTS_H
#define RGCONSTANTS_H

#include "Constants.h"
#include <QString>

namespace SKT {

class RgConstants {
    Q_GADGET
public:
    enum NotificationK {
        UpdateRouteGuidance = Constants::Rg,
        UpdateHighwayRouteGuidance,
        PrepareStartRouteGuidance,
        StartRouteGuidance,
        StartRouteGuidanceContinuously,
        StartRouteGuidanceByVr,
        StopRouteGuidance,
        SwitchToLocal,
        TerminatedRouteGuidance,
        ChangeRouteGuideBindState,
        ShowRouteGuidanceContinuously,
		ShowBusLaneGuidance,
        CloseNaviMenuAll,       // will be deprecated
        RequestTbtStateUpdate,
        RequestSdiStateUpdate,
        RequestLaneStateUpdate,
        UpdateTbtState,
        UpdateLaneState,
        UpdateSdiState,
        RequestRouteGuidanceState,
        ReplyToRouteGuidanceState,
        ChangedRouteGuidanceState,
        UpdateRouteGuidanceState, //only for NavigationControllerProxy
        ChangeRouteOption,
        RequestTrafficGuidance,
        UpdateTrafficGuidance,
        RequestUiState,
        ReplyToUiState,
        ChangedUiState,
        UpdateView,
        ChangedSignalState,
		UpdateGpsMapmatchTimestamp,
        UpdateGpsTime,
        RequestGetGasStationInfo,
        RequestGetEStationInfo,
        ResponseGetGasStationInfo,
        ResponseGetEStationInfo,
        RequestIsSoundPlaying,
        ReplyToIsSoundPlaying,
        RequestCurrentSpeed,
        ReplyToCurrentSpeed,
        RequestMovingDistance,
        ReplyToMovingDistance,

        UpdatePeriodicRerouteArea,
        ForcePeriodicReroute,
        BlockReroute,
        RegistRecentDestination,

        EnableSimulatorStopBySpeed,

        DisplayOnlineCrossImage
    };
    Q_ENUM( NotificationK )

    enum UpdateUiStateItemK {
        TbtVisible = 0,
        SummaryTbtVisible,
        HighwayVisible,
        CrossImageVisible,
        HighwayTbtListVisible,
        HiPassLaneVisible
    };
    Q_ENUM( UpdateUiStateItemK )

    enum UpdateDetailK {        
        TbtFirstTurnType,
        TbtFirstDistance,
        TbtFirstPointName,
        TbtSecondTurnType,
        TbtSecondDistance,
        TbtSecondPointName,
        TbtReset,

        SdiSignType,
        SdiSignImage,
        SdiSignNumber,
        SdiSignSmallNumber,
        SdiSectionSpeedAverageNumber,
        SdiSectionSpeedVisible,
        SdiSectionSpeedAverageVisible,
        SdiSectionSpeedTimeVisible,
        SdiSectionSpeedTime,
        SdiSectionSpeedToggleAverageAndTime,
        SdiSignVisible,
        SdiAlert,
        SdiDistanceVisible,
        SdiDistance,        
        SdiCurrentSpeed,
        SdiReset,

        LaneImageList,
        LaneDistance,
        LaneVisible,
        LaneReset
    };
    Q_ENUM( UpdateDetailK )

    enum SdiSignTypeK {
        SdiCircledNumber = 0, // 원 심볼 안에 숫자(고정식)
        SdiSymbol, // 숫자 없는 이미지 심볼
        SdiNumberWithSymbol, // 위 쪽에 숫자 아래 쪽에 이미지 심볼
        SdiSectionSpeed, // 구간단속
        SdiChangeableSectionSpeed, // 가변구간 단속
        SdiChangeableSectionSpeedButSpeedLimitNA, // 가변구간 단속(제한속도 표시할 수 없)
    };
    Q_ENUM(SdiSignTypeK)

    enum SdiSectionSpeedNumberType {
        SdiNormal = 0,
        SdiExcessive,
        SdiWarning
    };

    // static const QString
    static const QString Termination;
    static const QString MapMain;
    static const QString PastPos;

    static const QString Address;
    static const QString IsNormallyTerminated;
    static const QString SelectedRouteIndex;
    static const QString SelectedRouteOption;
    static const QString Via;
    static const QString ViewLevel;
    static const QString PastPosData;
    static const QString PastPosX;
    static const QString PastPosY;
    static const QString PastSessionId;
    static const QString PastDate;
};

#define MAX_SPEED_TO_OPEN_SIMULATION_VIEW   10

}

#endif
