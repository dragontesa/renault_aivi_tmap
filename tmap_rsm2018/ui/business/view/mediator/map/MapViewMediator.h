#ifndef MAPVIEWMEDIATOR_H
#define MAPVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"
#include "mediator/app/MapViewMode.h"
#include "PreferenceVo.h"

namespace SKT {

class MapViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        RendererTouched,
        NaviButtonClicked,
        BackButtonClicked,
        OptionButtonClicked,
        //17.9.27 hskim moved SimulationScaleControl into MapControlView
        //MapScaleReturnButtonClicked,
        //17.9.27 hskim moved SimulationScaleControl into MapControlView
        //RestoreSimulationViewButtonClicked,
        ReRouteButtonClicked,
        Activated,
        ViewModeChangedFromControl,
        ViewLevelChanged,
        //17.9.27 hskim moved SimulationScaleControl into MapControlView
        //MapScaleChanged,
        AreaNameChanged,
        CreateRgPopup,
        CloseRgPopup,
        CreateMapModeOptionPopup,
        CloseMapModeOptionPopup,
        ResetMapControlViewTimer,
        VolumeButtonClicked,
        ObjectHitting,
        SkipViaButtonClicked,
        ModalChanged,
        MoveModeChanged,
        ViewModeChanged,
        ShowView,
        HideView,
        RendererSizeChanged,
        RegistCsw,
        RemoveCsw,
        RendererOnDestruction,
        Hitting,
        CloseMapTarget,
        MoveMap,

        //[#온라인 주소 표출] 2018.09.13 cjlee
        MapTouched,
        MapDragged,
        //[#속도반응형 disable/enable] 2018.11.30
        SpeedResponseMapDisable,
        SpeedResponseMapEnable,

        IsNaviOrAndoChanged,
        IsOnlineChanged,
        ShowTrafficInfoChanged,
        EnableTrafficInfoChanged,
        ShowTrafficInfoOnMapChanged,
        ShowTrafficInfoOnRouteLineChanged,
        ShowTrafficInfoOnRouteLineForceChanged,
        NightModeChanged,
        SkyStateChanged,

        SaveMapViewLevel,
        RequestViewList,

        StartedToRender
    };

    Q_INVOKABLE MapViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool eventFilter(QObject *target, QEvent *event );
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

    void onRegister();

private slots:
    void updateMapScale();
    void onViewLevelChanged(qreal);
        // map drive auto config notifications
    void onAppliedMapAutoLevelConfig( const QVariant& config );


private:
    QObject* retrieveSpeedMeter();
    QObject* retrieveBoschInfo();

    void updateCurrentPosition( const RgDataVo& rgDataVo );
    void updateVia( const RgDataVo& rgDataVo );
    void rerouteIfNeeded( const QVariantMap& data );
    void requestRerouteByBreakaway();

    void restoreDriveMapModeAndLevel();

    void handleApplication( const mvc::INotificationPtr& note );
    bool handleMap( int event, const QVariant& data );
    void handleMap( const mvc::INotificationPtr& note );
    void handleMapViewMode( const mvc::INotificationPtr& note );
    void createRgViews();
    bool handleRg( int event, const QVariant& data );
    void handleRg( const mvc::INotificationPtr& note );
    void handleSettingAndPreference( const mvc::INotificationPtr& note );
    void handleController( const mvc::INotificationPtr& note );

    void requestSettingAndPreference();
    void updateLanguage();
    bool isMapForGadget() const;
    bool isReturnFromGadget() const;

    QObject* mBoschInfo;
    QObject* mRenderer;
    QObject* mGpsState;
    QObject* mGpsMapmatchTimestamp;

    int mMapModeSetting;
    QString mRendererAreaName;
    MapViewMode::ModeK mMapViewMode;
    MapViewMode::ReasonK mMapViewModeReason;

    bool mSupportBreakawayReroute;
    bool mSupportPeriodicReroute;
    bool mIsSoundPlaying = false;

    QMap<QString,PreferenceVo> mRouteContPrefs;

    QList<int> mViewList;
    QMap<int,bool> mViewVisibleMap;

    // via
    bool mIsEnableToPassVia;

    quint64 mLastShowRerouteDialogTime;
    bool mIsVisibleRerouteDialog;
    int mMapViewLevel = 0;
    bool mUseSpeedResponseMap = false;

    //
    bool mNaviMenuViewActivated = false;
    bool mSettingMainViewActivated = false;

    bool mSummaryTbtVisible = false;
    bool mHighwayTbtVisible = false;
    bool mHighwayTbtListVisible = false;

    // mammatching feddback
    bool mDisplayMMFB;
    bool mIsOnline = false;
    bool mSkyboxEnabled = true;
    QString mLastSessionId;
    QString mRpDebugText;
};

}

#endif // MAPVIEWMEDIATOR_H
