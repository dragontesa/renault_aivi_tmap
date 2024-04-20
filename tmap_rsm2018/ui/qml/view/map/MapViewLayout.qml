import QtQuick 2.1
import LayoutConstants 1.0
import MapViewMode 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
	// 세로 기본
    property int laneViewBottomMargin: 10
    property int sdiViewY: 123
    property int sdiViewWidth: 140
    property int sdiViewHeight: 164
    property int speedMeterViewY: sdiViewY + 164 + 6
    property int speedMeterViewWidth: 140
    property int speedMeterViewHeight: 48
    property int highwayTbtViewX: GlobalConstants.kScreenWidth - 348
    property int highwayTbtViewY: 11
    property int hiPassLaneBottomMargin: 10
    property real scale: 1.0
    property int mapScaleX: 10
    property int mapScaleOffsetY: -287
    property int trafficBarBottomMargin: 52 // GlobalConstants.kScreenZone3Height + 10
    property int mapPaddingY: 83
    property int buttonSize: 69
    property int crossImageViewY: 0
    property int curLayout
    property int gpsStateXOnRight: GlobalConstants.kScreenWidth - 69 - 10
    property int gpsStateXOnMiddle: 350 + 10
    property int gpsStateXOnBottom: GlobalConstants.kScreenWidth - 69 - 69 - 10
    property int gpsStateY: 10
    property int gpsStateYOnBottom: GlobalConstants.kScreenContentHeight - GlobalConstants.kScreenZone3Height - 69 - 10

    Component.onCompleted: {
    }
	
    function applyMapViewMode( mode ) {
        if ( curLayout === LayoutConstants.L820x1024 ) {
            switch ( mode ) {
            case MapViewMode.Normal:
                sdiViewY = 140
                break;
            case MapViewMode.MapControl:
                break;
            case MapViewMode.MapInquiry:
                break;
            case MapViewMode.Navi:
                sdiViewY = 140
                break;
            case MapViewMode.MenuNavi:
                sdiViewY = 123
                break;
            case MapViewMode.Simul:
                sdiViewY = 140
                break;
            case MapViewMode.NormalMapControl:
                break;
            }
        }
    }

    function updateLayout( layout ) {
        curLayout = layout
        if ( layout === LayoutConstants.L820x1024 ) {
            trafficBarBottomMargin = 249 // GlobalConstants.kScreenZone3Height + 126  -> guid v3.04 ScreenZone3Height(83) + 87 + 69 + 10 = 249
        } else if (layout === LayoutConstants.L800x480) {
            sdiViewY = 120
            scale = 0.5
            mapScaleX = 0
            mapScaleOffsetY = -216
            trafficBarBottomMargin = 52
            mapPaddingY = 41
            buttonSize = 34
        }
	}
}

