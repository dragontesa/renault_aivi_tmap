#ifndef MAPCONSTANTS_H
#define MAPCONSTANTS_H

#include "Constants.h"

namespace SKT {

class MapConstants {
public:
    enum NotificationK {
        UpdateMapView = Constants::Map,
        ApplyMapPosition,
        AdjustMapLevel,
        StartMapViewTimer,
        ResetMapViewTimer,
        //RestoreMapMode,
        RequestMapPosition,
        ResponseMapPosition,
        RequestMapScale,
        ResponseMapScale,
        RequestMapAreaName,
        ResponseMapAreaName,

        //[#온라인 주소 표출] 2018.09.13 cjlee
        RendererTouched,

        ShowCarvata,
        UpdateMapViewPadding,
        ResetMapPosition,
        RegistGlobalMapViewRenderer,
        PauseMapView,
        ResumeMapView,

        MapControlViewActivated,

        ShowVolume,
        HideVolume,

        ShowViaMarker,
        HideViaMarker,
        RendererSizeChanged,
        DrawMarkerAll,

        ReturnClicked,
        SetMapViewAsGlobal,
        AttachMapViewOnVsmNavi,
        DetachMapViewFromVsmNavi,

        RequestMapRect,
        ResponseMapRect,
        DrawRouteAll,

        ShowGasStationInfo,
        HideGasStationInfo,
        ShowTrafficInfoOnRouteLineForce, // 경로 요약, 경로 상세 화면에서 사용
        ShowTrafficInfoOnRouteLine, // 환경 설정
        ShowTrafficInfoOnMap,       // 환경 설정
        EnableCallout,

        ChangeAutoZoom,

        RemoveCalloutPopup,
        ShowRoute,
        MoveMapByScreenPos,
        MoveMapByUser,
        MoveMapByWorldPos,

        //////////////////////////////////////////////////////////////////////
        // MapViewMode
        //////////////////////////////////////////////////////////////////////

        ApplyMapViewMode,
        ChangeMapViewMode,
        RestoreMapViewMode,
        NotifyMapViewMode, // map view mode 요청
        RequestDriveMode,
        ResponseDriveMode,
        ChangeViewMode, // 순수 Renderer.viewMode 변경.
        ChangeViewLevel,// 순수 Renderer.viewLevel 변경.
        ChangeViewLevelUsingApi,

        // map in clustr
        StartMapInCluster,
        StopMapInCluster,
        ChangeMapInClusterLayout,

        // Vsm Map Setting
        ApplyMapSetting,
        ApplyMapTheme,

        ChangeGpsStatePosition,

        // Map View Status
        RequestGlobalMapViewStatus,
        RespondToGlobalMapViewStatus,
        GlobalMapViewIsNaviOrAndoChanged,
        GlobalMapViewIsOnlineChanged,
        GlobalMapViewShowTrafficInfoChanged,
        GlobalMapViewEnableTrafficInfoChanged,
        GlobalMapViewShowTrafficInfoOnMapChanged,
        GlobalMapViewShowTrafficInfoOnRouteLineChanged,
        GlobalMapViewShowTrafficInfoOnRouteLineForceChanged,
        GlobalMapViewViewLevelChanged,
        GlobalMapViewViewModeChanged,
        GlobalMapViewNightModeChanged,
        GlobalMapViewSkyStateChanged,
        GlobalMapViewMoveModeChanged,
        GlobalMapViewStartedToRender,        
    };

    const static QString Mode;
};

}

#endif
