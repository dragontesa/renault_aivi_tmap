#ifndef MAPVIEWMODE_H
#define MAPVIEWMODE_H

namespace SKT {

// 맵 뷰 모드의 변화만을 다룬다
// 주행관련 처리는 RouteGuidanceProxy등에서 구현

class MapViewMode : public QObject
{
    Q_OBJECT    
public:
    enum ModeK {
        Normal = 0, // 초기 모드(경로없는 Navi 모드)
        MapControl, // 맵을 터치했을 때
        MapInquiry, // 조회 모드
        Navi, //
        MenuNavi, // 17.10.10 hskim 내비메뉴, 환경설정 화면 맵 주행 상태 표시를 위해 추가(sdi 표시, 클릭x)
        Simul,
        NormalMapControl,
    };
    Q_ENUM( ModeK )

    enum ReasonK {
        None = 0,
        MapViewCreated,
        MapViewActivated,
        ReturnFromNormalMode,
        ReturnFromMapControlMode,
        ReturnFromMapInquiryMode,
        ReturnFromNaviMode,
		ReturnFromMenuNaviMode,
        ReturnFromSimulMode,
        CancelRoute,
        RecentTileGridViewLostFocus // MapView_OffGuidance screen 시나리오에 대응
    };
    Q_ENUM( ReasonK )

    MapViewMode();

    ModeK currentMode();
    void setCurrentMode( QObject* caller, ModeK mode, ReasonK reason = ReasonK::None );
    void notifyCurrentMode( ReasonK reason = ReasonK::None );
    ReasonK currentReason();
    ModeK lastDriveMode();

    static QString modeAsString( ModeK mode );
    static QString reasonAsString( ReasonK reason );

    static bool isInDrive( int mapViewMode );
    static bool isInRouteGuidance( int mapViewMode );

private:
    ModeK mCurMode;
    ReasonK mCurReason;
    ModeK mLastDriveMode;
};

}

#endif // MAPVIEWMODE_H
