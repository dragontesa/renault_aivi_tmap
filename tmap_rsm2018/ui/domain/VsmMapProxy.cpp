#include "VsmMapProxy.h"
#include "VsmNaviProxy.h"
#include "VSM_Navi.h"
#include "VSM_Map.h"
#include "VSM_Pos.h"
#include "VSM_Map_Settings.h"
#include <QTextCodec>
#include "VsmMapContants.h"
#include "VSM.h"
#include "CommonConstants.h"
#include "SettingConstants.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"

using namespace SKT;

VsmMapProxy::VsmMapProxy()
    : AbstractProxy( "VsmMapProxy" )
{
}

void VsmMapProxy::setGlobalMapView(QObject* mapView)
{
    mGlobalMapView = mapView;
    mGlobalMapViewHandle = qvariant_cast<void*>(mapView->property("nativeHandle"));
}

VsmMapSetting VsmMapProxy::setting( VsmMapType type )
{
    VsmMapSetting ret;

    if ( type == VsmMapType::Global ) {
        ret.densitydpi = VsmMapConstants::Setting::Global::Densitydpi;
        ret.tileDiskCache = VsmMapConstants::Setting::Global::TileDiskCache;
        ret.buildingFilter = VsmMapConstants::Setting::Global::BuildingFilter;
        ret.fovy = VsmMapConstants::Setting::Global::Fovy;
        ret.naviPoiScale = VsmMapConstants::Setting::Global::NaviPoiScale;
        ret.routeWidth = VsmMapConstants::Setting::Global::RouteWidth;
        ret.copycatBuilding = VsmMapConstants::Setting::Global::CopycatBuilding;
        ret.showPoi = VsmMapConstants::Setting::Global::ShowPoi;
        ret.poiScaleForce = VsmMapConstants::Setting::Global::PoiScaleForce;
        ret.poiOcclusion = VsmMapConstants::Setting::Global::PoiOcclusion;
        ret.renderSubwayline = VsmMapConstants::Setting::Global::RenderSubwayline;
        ret.routeTi = VsmMapConstants::Setting::Global::RouteTi;
        ret.landmark = VsmMapConstants::Setting::Global::Landmark;
        ret.outline3d = VsmMapConstants::Setting::Global::Outline3d;
        ret.outline2d = VsmMapConstants::Setting::Global::Outline2d;
        ret.only2d = VsmMapConstants::Setting::Global::Only2d;
    }
    else if ( type == VsmMapType::Cluster ) {
        ret.densitydpi = VsmMapConstants::Setting::Cluster::Densitydpi;
        ret.tileDiskCache = VsmMapConstants::Setting::Cluster::TileDiskCache;
        ret.buildingFilter = VsmMapConstants::Setting::Cluster::BuildingFilter;
        ret.fovy = VsmMapConstants::Setting::Cluster::Fovy;
        ret.naviPoiScale = VsmMapConstants::Setting::Cluster::NaviPoiScale;
        ret.routeWidth = VsmMapConstants::Setting::Cluster::RouteWidth;
        ret.copycatBuilding = VsmMapConstants::Setting::Cluster::CopycatBuilding;
        ret.showPoi = VsmMapConstants::Setting::Cluster::ShowPoi;
        ret.showRoadOnly = VsmMapConstants::Setting::Cluster::ShowRoadOnly;
        ret.poiScaleForce = VsmMapConstants::Setting::Cluster::PoiScaleForce;
        ret.poiOcclusion = VsmMapConstants::Setting::Cluster::PoiOcclusion;
        ret.renderSubwayline = VsmMapConstants::Setting::Cluster::RenderSubwayline;
        ret.routeTi = VsmMapConstants::Setting::Cluster::RouteTi;
        ret.landmark = VsmMapConstants::Setting::Cluster::Landmark;
        ret.outline3d = VsmMapConstants::Setting::Cluster::Outline3d;
        ret.outline2d = VsmMapConstants::Setting::Cluster::Outline2d;
        ret.only2d = VsmMapConstants::Setting::Cluster::Only2d;
    }
    else if ( type == VsmMapType::ClusterHud ) {
        ret.densitydpi = VsmMapConstants::Setting::Cluster::Densitydpi;
        ret.tileDiskCache = VsmMapConstants::Setting::Cluster::TileDiskCache;
        ret.buildingFilter = VsmMapConstants::Setting::Cluster::BuildingFilter;
        ret.fovy = VsmMapConstants::Setting::Cluster::Fovy;
        ret.naviPoiScale = VsmMapConstants::Setting::Cluster::NaviPoiScale;
        ret.routeWidth = VsmMapConstants::Setting::Cluster::RouteWidth;
        ret.copycatBuilding = VsmMapConstants::Setting::Cluster::CopycatBuilding;
        ret.showPoi = VsmMapConstants::Setting::ClusterHud::ShowPoi;
        ret.showRoadOnly = VsmMapConstants::Setting::ClusterHud::ShowRoadOnly;
        ret.poiScaleForce = VsmMapConstants::Setting::Cluster::PoiScaleForce;
        ret.poiOcclusion = VsmMapConstants::Setting::Cluster::PoiOcclusion;
        ret.renderSubwayline = VsmMapConstants::Setting::ClusterHud::RenderSubwayline;
        ret.routeTi = VsmMapConstants::Setting::Cluster::RouteTi;
        ret.landmark = VsmMapConstants::Setting::Cluster::Landmark;
        ret.outline3d = VsmMapConstants::Setting::Cluster::Outline3d;
        ret.outline2d = VsmMapConstants::Setting::Cluster::Outline2d;
        ret.only2d = VsmMapConstants::Setting::Cluster::Only2d;
    }	

    return ret;
}

void VsmMapProxy::applySetting( QObject* mapView, const VsmMapSetting& setting )
{
    void* handle = chooseMapViewHandle(mapView);

    VSMMap_Setting_SetDensityDpi(handle, (VSM_UInt16)setting.densitydpi);

    // [TMCRSM-2128] VSM_SetTileDiskCachMode is applied globally, so no need to call it whenenver each map view is created
    //VSM_SetTileDiskCachMode(VSM_TileDBMode(setting.tileDiskCache));

    VSMMap_Setting_SetBuildingFilterMode(handle, VSM_BuildingFilterMode(setting.buildingFilter));
    VSMMap_Setting_SetFovy(handle, (VSM_Float)setting.fovy);
    VSMMap_Setting_SetEnableBuildingPattern(handle, (VSM_Bool)setting.copycatBuilding);
    VSMMap_Setting_SetShowPOI(handle, (VSM_Bool)setting.showPoi);
    //VSMMap_Setting_SetShowPOI(handle, (VSM_Bool)VSM_FALSE);
    VSMMap_Setting_SetPOIScaleForNaviMode(handle, (VSM_Float)setting.naviPoiScale);
    VSMMap_Setting_SetPOIScaleForcedApply(handle, (VSM_Bool)setting.poiScaleForce);
    VSMMap_Setting_SetPOIOcclusion(handle, (VSM_Bool)setting.poiOcclusion);
    VSMMap_Setting_SetShouldRenderSubwayLines(handle, (VSM_Bool)setting.renderSubwayline);
    VSMMap_Setting_SetRouteTI(handle, (VSM_Bool)setting.routeTi);
    VSMMap_Setting_SetShowLandmark(handle, (VSM_Bool)setting.landmark);
    VSMMap_Setting_SetShowBuildingOutline3D(handle, (VSM_Bool)setting.outline3d);
    VSMMap_Setting_SetShowBuildingOutline2D(handle, (VSM_Bool)setting.outline2d);
    VSMMap_Setting_SetShowBuilding2DOnly(handle, (VSM_Bool)setting.only2d);
}

VsmMapTheme VsmMapProxy::theme( VSM_ThemeType themeType )
{
    VsmMapTheme ret;

    // route candidate

    VsmMapThemeRouteSelection rs;
    rs.width = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Width;
    rs.fillColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::FillColor;
    rs.strokeColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::StrokeColor;
    rs.showCongestion = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::ShowCongestion;
    rs.congestion.noDataFillColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::NoDataFillColor;
    rs.congestion.noDataStrokeColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::NoDataStrokeColor;
    rs.congestion.goodFillColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::GoodFillColor;
    rs.congestion.goodStrokeColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::GoodStrokeColor;
    rs.congestion.slowFillColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::SlowFillColor;
    rs.congestion.slowStrokeColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::SlowStrokeColor;
    rs.congestion.badFillColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::BadFillColor;
    rs.congestion.badStrokeColor = VsmMapConstants::Theme::RouteSelection::Candidate0::Default::Congestion::BadStrokeColor;

    ret.routeSelection.candidate[0].defoult = rs;
    ret.routeSelection.candidate[0].selected = rs;
    ret.routeSelection.candidate[0].selected.width = VsmMapConstants::Theme::RouteSelection::Candidate0::Selected::Width;
    ret.routeSelection.candidate[0].selected.showCongestion = VsmMapConstants::Theme::RouteSelection::Candidate0::Selected::ShowCongestion;

    ret.routeSelection.candidate[1].defoult = ret.routeSelection.candidate[0].defoult;
    ret.routeSelection.candidate[1].defoult.fillColor = VsmMapConstants::Theme::RouteSelection::Candidate1::Default::FillColor;
    ret.routeSelection.candidate[1].selected = ret.routeSelection.candidate[1].defoult;
    ret.routeSelection.candidate[1].selected.width = VsmMapConstants::Theme::RouteSelection::Candidate1::Selected::Width;
    ret.routeSelection.candidate[1].selected.showCongestion = VsmMapConstants::Theme::RouteSelection::Candidate1::Selected::ShowCongestion;

    ret.routeSelection.candidate[2].defoult = ret.routeSelection.candidate[0].defoult;
    ret.routeSelection.candidate[2].defoult.fillColor = VsmMapConstants::Theme::RouteSelection::Candidate2::Default::FillColor;
    ret.routeSelection.candidate[2].selected = ret.routeSelection.candidate[2].defoult;
    ret.routeSelection.candidate[2].selected.width = VsmMapConstants::Theme::RouteSelection::Candidate2::Selected::Width;
    ret.routeSelection.candidate[2].selected.showCongestion = VsmMapConstants::Theme::RouteSelection::Candidate2::Selected::ShowCongestion;

    if ( themeType == VSM_THEME_DAY ) {
        // alternate route

        ret.alternateRoute.width = VsmMapConstants::Theme::Day::AlternateRoute::Width;
        ret.alternateRoute.fillColor = VsmMapConstants::Theme::Day::AlternateRoute::FillColor;
        ret.alternateRoute.strokeColor = VsmMapConstants::Theme::Day::AlternateRoute::StrokeColor;

        // route line

        ret.routeLine.width = VsmMapConstants::Theme::Day::RouteLine::Width;
        ret.routeLine.fillColor = VsmMapConstants::Theme::Day::RouteLine::FillColor;
        ret.routeLine.strokeColor = VsmMapConstants::Theme::Day::RouteLine::StrokeColor;
        ret.routeLine.passedFillColor = VsmMapConstants::Theme::Day::RouteLine::PassedFillColor;
        ret.routeLine.passedStrokeColor = VsmMapConstants::Theme::Day::RouteLine::PassedStrokeColor;

        ret.routeLine.turnPoint.width = VsmMapConstants::Theme::Day::RouteLine::TurnPoint::Width;
        ret.routeLine.turnPoint.floatGap = VsmMapConstants::Theme::Day::RouteLine::TurnPoint::FloatGap;
        ret.routeLine.turnPoint.fillColor = VsmMapConstants::Theme::Day::RouteLine::TurnPoint::FillColor;
        ret.routeLine.turnPoint.strokeColor = VsmMapConstants::Theme::Day::RouteLine::TurnPoint::StrokeColor;

        ret.routeLine.congestion.noDataFillColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::NoDataFillColor;
        ret.routeLine.congestion.noDataStrokeColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::NoDataStrokeColor;
        ret.routeLine.congestion.goodFillColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::GoodFillColor;
        ret.routeLine.congestion.goodStrokeColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::GoodStrokeColor;
        ret.routeLine.congestion.slowFillColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::SlowFillColor;
        ret.routeLine.congestion.slowStrokeColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::SlowStrokeColor;
        ret.routeLine.congestion.badFillColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::BadFillColor;
        ret.routeLine.congestion.badStrokeColor = VsmMapConstants::Theme::Day::RouteLine::Congestion::BadStrokeColor;
    }
    else if ( themeType == VSM_THEME_NIGHT ) {
        // alternate route

        ret.alternateRoute.width = VsmMapConstants::Theme::Night::AlternateRoute::Width;
        ret.alternateRoute.fillColor = VsmMapConstants::Theme::Night::AlternateRoute::FillColor;
        ret.alternateRoute.strokeColor = VsmMapConstants::Theme::Night::AlternateRoute::StrokeColor;

        // route line

        ret.routeLine.width = VsmMapConstants::Theme::Night::RouteLine::Width;
        ret.routeLine.fillColor = VsmMapConstants::Theme::Night::RouteLine::FillColor;
        ret.routeLine.strokeColor = VsmMapConstants::Theme::Night::RouteLine::StrokeColor;
        ret.routeLine.passedFillColor = VsmMapConstants::Theme::Night::RouteLine::PassedFillColor;
        ret.routeLine.passedStrokeColor = VsmMapConstants::Theme::Night::RouteLine::PassedStrokeColor;

        ret.routeLine.turnPoint.width = VsmMapConstants::Theme::Night::RouteLine::TurnPoint::Width;
        ret.routeLine.turnPoint.floatGap = VsmMapConstants::Theme::Night::RouteLine::TurnPoint::FloatGap;
        ret.routeLine.turnPoint.fillColor = VsmMapConstants::Theme::Night::RouteLine::TurnPoint::FillColor;
        ret.routeLine.turnPoint.strokeColor = VsmMapConstants::Theme::Night::RouteLine::TurnPoint::StrokeColor;

        ret.routeLine.congestion.noDataFillColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::NoDataFillColor;
        ret.routeLine.congestion.noDataStrokeColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::NoDataStrokeColor;
        ret.routeLine.congestion.goodFillColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::GoodFillColor;
        ret.routeLine.congestion.goodStrokeColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::GoodStrokeColor;
        ret.routeLine.congestion.slowFillColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::SlowFillColor;
        ret.routeLine.congestion.slowStrokeColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::SlowStrokeColor;
        ret.routeLine.congestion.badFillColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::BadFillColor;
        ret.routeLine.congestion.badStrokeColor = VsmMapConstants::Theme::Night::RouteLine::Congestion::BadStrokeColor;
    }

    return ret;
}

void VsmMapProxy::applyTheme( QObject* mapView, VSM_ThemeType themeType, const VsmMapTheme& theme)
{
    struct Internal {
        static QString decRGBA2Hex(qint32 rgba) {
            return QString("#%1").arg((uint)rgba, 8, 16, QLatin1Char('0'));
        }
    };

    void* handle = chooseMapViewHandle(mapView);

    // alternate route

    QJsonObject alternateRoute;
    alternateRoute["width"] = theme.alternateRoute.width;
    alternateRoute["fillColor"] = Internal::decRGBA2Hex(theme.alternateRoute.fillColor);
    alternateRoute["strokeColor"] = Internal::decRGBA2Hex(theme.alternateRoute.strokeColor);

    // route line

    QJsonObject routeLine;
    routeLine["width"] = theme.routeLine.width;
    routeLine["fillColor"] = Internal::decRGBA2Hex(theme.routeLine.fillColor);
    routeLine["strokeColor"] = Internal::decRGBA2Hex(theme.routeLine.strokeColor);
    routeLine["passedFillColor"] = Internal::decRGBA2Hex(theme.routeLine.passedFillColor);
    routeLine["passedStrokeColor"] = Internal::decRGBA2Hex(theme.routeLine.passedStrokeColor);

    QJsonObject turnPoint;
    turnPoint["width"] = theme.routeLine.turnPoint.width;
    turnPoint["floatGap"] = theme.routeLine.turnPoint.floatGap;
    turnPoint["fillColor"] = Internal::decRGBA2Hex(theme.routeLine.turnPoint.fillColor);
    turnPoint["strokeColor"] = Internal::decRGBA2Hex(theme.routeLine.turnPoint.strokeColor);
    turnPoint["outlineRatio"] = 0.4;

    QJsonObject congestion;
    congestion["noDataFillColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.noDataFillColor);
    congestion["noDataStrokeColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.noDataStrokeColor);
    congestion["goodFillColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.goodFillColor);
    congestion["goodStrokeColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.goodStrokeColor);
    congestion["slowFillColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.slowFillColor);
    congestion["slowStrokeColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.slowStrokeColor);
    congestion["badFillColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.badFillColor);
    congestion["badStrokeColor"] = Internal::decRGBA2Hex(theme.routeLine.congestion.badStrokeColor);

    routeLine["turnPoint"] = turnPoint;
    routeLine["congestion"] = congestion;

    // route candidate

    QJsonArray candidates;

    for ( int i = 0; i < 3; i++ ) {
        // default
        QJsonObject rcDefault;
        rcDefault["width"] = theme.routeSelection.candidate[i].defoult.width;
        rcDefault["fillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.fillColor);
        rcDefault["strokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.strokeColor);
        rcDefault["showCongestion"] = theme.routeSelection.candidate[i].defoult.showCongestion;

        QJsonObject rcDefaultCongestion;
        rcDefaultCongestion["noDataFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.noDataFillColor);
        rcDefaultCongestion["noDataStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.noDataStrokeColor);
        rcDefaultCongestion["goodFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.goodFillColor);
        rcDefaultCongestion["goodStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.goodStrokeColor);
        rcDefaultCongestion["slowFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.slowFillColor);
        rcDefaultCongestion["slowStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.slowStrokeColor);
        rcDefaultCongestion["badFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.badFillColor);
        rcDefaultCongestion["badStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].defoult.congestion.badStrokeColor);

        rcDefault["congestion"] = rcDefaultCongestion;

        // selected
        QJsonObject rcSelected;
        rcSelected["width"] = theme.routeSelection.candidate[i].selected.width;
        rcSelected["fillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.fillColor);
        rcSelected["strokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.strokeColor);
        rcSelected["showCongestion"] = theme.routeSelection.candidate[i].selected.showCongestion;

        QJsonObject rcSelectedCongestion;
        rcSelectedCongestion["noDataFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.noDataFillColor);
        rcSelectedCongestion["noDataStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.noDataStrokeColor);
        rcSelectedCongestion["goodFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.goodFillColor);
        rcSelectedCongestion["goodStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.goodStrokeColor);
        rcSelectedCongestion["slowFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.slowFillColor);
        rcSelectedCongestion["slowStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.slowStrokeColor);
        rcSelectedCongestion["badFillColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.badFillColor);
        rcSelectedCongestion["badStrokeColor"] = Internal::decRGBA2Hex(theme.routeSelection.candidate[i].selected.congestion.badStrokeColor);

        rcSelected["congestion"] = rcSelectedCongestion;

        QJsonObject rc;
        rc["id"] = QString("route%1").arg(i);
        rc["default"] = rcDefault;
        rc["selected"] = rcSelected;

        candidates.push_back(rc);
    }

    QJsonObject routeSelection;
    routeSelection["candidates"] = candidates;

    QJsonObject oilPopup;
    oilPopup["scale"] = 1.6f;

    QJsonObject carvatar;
    carvatar["width"] = 100.0f;

    QJsonObject root;
    root["routeLine"] = routeLine;
    root["alternateRoute"] = alternateRoute;
    root["routeSelection"] = routeSelection;
    root["oilPopup"] = oilPopup;
    root["carvatar"] = carvatar;

    VSMMap_CreateOrUpdateThemeStyle(handle, themeType, QJsonDocument(root).toJson(QJsonDocument::Compact).data());
}

QObject* VsmMapProxy::chooseMapView(QObject* mapView) const
{
    return mapView != nullptr ? mapView : mGlobalMapView;
}

void* VsmMapProxy::chooseMapViewHandle(QObject* mapView) const
{
    return mapView != nullptr ? qvariant_cast<void*>(mapView->property("nativeHandle")) : mGlobalMapViewHandle;
}

void VsmMapProxy::viewLevel(QObject* mapView, int& level, int& subLevel)
{
    VSMMap_GetViewLevel(chooseMapViewHandle(mapView), level, subLevel);
}

void VsmMapProxy::setViewLevel(QObject* mapView, int level, int subLevel, bool isAnim)
{
    // To hold level screen, turn off Auto Speed Level for around 3 seconds
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
    body.insert( SettingConstants::SpeedResponseMapUse, false);
    sendNotification( NavigationControllerConstants::ApplyController, body );
    QTimer::singleShot(3000, [this](){
        QVariantMap body;
        body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
        this->sendNotification( NavigationControllerConstants::ApplyController, body );
    });

    void* handle = chooseMapViewHandle(mapView);
    VSMMap_SetViewLevel(handle, level, subLevel, isAnim); // 17.12.8 hskim appy multi map view. not used.
    // fixed TMCRSM-1206 -  tilt angle must get collaboration with level 
    float angle;
    VSMMap_Setting_GetSmooth3DAngle(handle, level, subLevel, angle);
    VSMMap_SetTiltAngle(handle, angle, VSM_TRUE);
}

void VsmMapProxy::update(QObject* mapView)
{
    VSMMap_SetNeedUpdate(chooseMapViewHandle(mapView)); // 17.12.8 hskim appy multi map view. not used.
}

void VsmMapProxy::drawRouteAll(QObject* mapView, int routeIndex, int w, int h, int flag)
{
    QMetaObject::invokeMethod(mapView, "drawRouteAll",
                                  Q_ARG(int, routeIndex),
                                  Q_ARG(int, w),
                                  Q_ARG(int, h),
                                  Q_ARG(int, flag));
}

void VsmMapProxy::showCarvata(QObject* mapView,bool show)
{
    VSM_PositionIconType iconType = show ? VSM_PositionIconType_Cavatar : VSM_PositionIconType_None;

    VSMMap_SetPositionIconType(chooseMapViewHandle(mapView), iconType); // 17.12.8 hskim appy multi map view. not used.
}

void VsmMapProxy::removeCalloutPopup(QObject* mapView, bool animate)
{
    VSMMap_RemoveCalloutPopup(chooseMapViewHandle(mapView), animate ? VSM_TRUE : VSM_FALSE);
}

QPoint VsmMapProxy::centerPosition(QObject* mapView) const
{
    auto mv = chooseMapView( mapView );
    auto mapSize = QSize(mv->property("width").toInt(),mv->property("height").toInt());

    VSM_Int32 sx = mapSize.width() / 2;
    VSM_Int32 sy = mapSize.height()/ 2;
    VSM_Int32 wx , wy;

    VSMMap_ScreenToWorld(chooseMapViewHandle(mapView), sx, sy, wx, wy);

    return QPoint(wx, wy);
}

QSize VsmMapProxy::mapSize(QObject *mapView) const
{
    auto mv = chooseMapView( mapView );
    if (mv) {
        return QSize(mv->property("width").toInt(),mv->property("height").toInt());
    } else {
        return QSize {};
    }
}

void VsmMapProxy::moveTo(QObject* mapView, const QPoint& pos)
{
    moveTo(mapView, pos, true);
}

void VsmMapProxy::moveTo(QObject *mapView, const QPoint &pos, bool anim)
{
    VSMMap_MapMoveTo(chooseMapViewHandle(mapView), pos.x(), pos.y(), anim);
}

QPoint VsmMapProxy::screenToWorld(QObject* mapView, int x, int y)
{
    VSM_Int32 x0;
    VSM_Int32 y0;
    VSM_Bool ret = VSMMap_ScreenToWorld(chooseMapViewHandle(mapView), x, y, x0, y0);
    if (ret == VSM_TRUE) {
        return QPoint(x0, y0);
    } else {
        return QPoint();
    }
}

QRect VsmMapProxy::requestMapRect(QObject* mapView)
{
    auto mv = chooseMapView( mapView );
    auto mapSize = QSize(mv->property("width").toInt(),mv->property("height").toInt());

    auto pos0 = screenToWorld(mv, 0, 0);
    auto pos1 = screenToWorld(mv, mapSize.width(), mapSize.height());

    return QRect{ pos0, pos1 };
}

void VsmMapProxy::drawRouteAll(QObject* mapView, bool isArrivalBelowDeparture, bool isSummaryView)
{
    auto mv = chooseMapView( mapView );
    auto mapSize = QSize(mv->property("width").toInt(), mv->property("height").toInt());

    auto bb = routeBoundingRect();
    if (!bb.isNull()) {
        auto mapWidth = mapSize.width();
        auto mapHeight = mapSize.height();
        auto minX = bb.left();
        auto maxX = bb.right();
        auto minY = bb.top();
        auto maxY = bb.bottom();
        float r = 8.0;
        float marginX = (maxX - minX) / r;
        float marginY = (maxY - minY) / r;
        minX -= marginX;
        minY -= marginY;
        maxX += marginX;
        maxY += marginY;
        if (isSummaryView) {
            maxY += marginY * 3.5;
        } else {
            if (!isArrivalBelowDeparture)
                maxY += marginY * 1;
        }

        QMetaObject::invokeMethod(mv, "drawMBRAll",
                                          Q_ARG(int, mapWidth),
                                          Q_ARG(int, mapHeight),
                                          Q_ARG(int, minX),
                                          Q_ARG(int, minY),
                                          Q_ARG(int, maxX),
                                          Q_ARG(int, maxY));
    }
}

void VsmMapProxy::setShowRoute(QObject *mapView, bool show)
{
    VSMMap_SetShowRoute(chooseMapViewHandle(mapView), show);
}

QString VsmMapProxy::areaName(int wx, int wy) const
{
    QString ret;
    char name[VSM_MAX_AREANAME_LEN+1];
    memset(name, 0, sizeof(name));
    auto result = VSMMap_GetAreaName(wx, wy, name, VSM_MAX_AREANAME_LEN);
    if (result) {
        QTextCodec *codec = QTextCodec::codecForName("EUC-KR");
        ret = codec->toUnicode(name);
    }
    return ret;
}

QRect VsmMapProxy::routeBoundingRect() const
{
    int minX = INT_MAX;
    int minY = INT_MAX;
    int maxX = INT_MIN;
    int maxY = INT_MIN;
    bool ok = false;

    for (auto i = 0; i<3; i++) {
        VSM_Int32 l;
        VSM_Int32 r;
        VSM_Int32 t;
        VSM_Int32 b;
        if (VSM_TRUE == VSMNavi_GetRouteBoundingBox(i, l, t, r, b)) {
            if (l < minX) {
                minX = l;
            }
            if (r > maxX) {
                maxX = r;
            }
            if (t < minY) {
                minY = t;
            }
            if (b > maxY) {
                maxY = b;
            }
            ok = true;
        }
    }

    return ok ? QRect {QPoint{minX, minY}, QPoint{maxX, maxY}} : QRect {};
}
