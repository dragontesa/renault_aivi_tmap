#include "ManageVsmMapCommand.h"

#include <QSize>
#include <QVariant>
#include <QDebug>

#include "Constants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "AddressConstants.h"
#include "Facade.h"
#include "VsmMapProxy.h"
#include "VsmNaviProxy.h"
#include "VSM.h"
#include "VSM_Map.h"
#include "VSM_Navi_RgData.h"
#include "VSM_Navi.h"
#include "VSM_Util.h"
#include "RouteManager.h"
#include "RouteSearchProxy.h"
#include "RouteGuidanceProxy.h"
#include "NavibookmarkProtocol.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "NavigationControllerProxy.h"
#include "VsmNaviProxy.h"
#include "ViewRegistry.h"
#include "view/mediator/app/MapViewMode.h"
#include "PlatformHelper.h"
#include "../platforms/linux-gen3/interface/server/navigation/navigationservicetype.h"
#include "DebugUtils.h"

namespace SKT {

ManageVsmMapCommand::ManageVsmMapCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

bool ManageVsmMapCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {    
    case MapConstants::SetMapViewAsGlobal:
        {
            //MMV 18.4.18
            auto naviControlProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto m = noti->getBody().value<QVariantMap>();
            auto mapView = m[QStringLiteral("mapView")].value<QObject*>();

            naviControlProxy->addMapView( mapView );            

            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            mapProxy->setGlobalMapView(mapView);

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->setGlobalMapView(mapView);

            auto routeProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            routeProxy->setGlobalMapView(mapView);

            naviControlProxy->mapViewInitialized();
        }
        break;
    case MapConstants::UpdateMapView: {
            auto mapView = noti->getBody().value<QObject*>();
            update(mapView);

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto routeId = noti->getBody().toInt();
            naviProxy->selectRoute(routeId);
        }
        break;
    case MapConstants::MoveMapByScreenPos:
        {
            auto m = noti->getBody().toMap();
            auto x = m["x"].toInt();
            auto y = m["y"].toInt();
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto mapSize = mapProxy->mapSize(nullptr);
            auto cx = mapSize.width() / 2;
            auto cy = mapSize.height() / 2;
            x += cx;
            y += cy;
            auto worldPos = mapProxy->screenToWorld(nullptr, x, y);
            mapProxy->moveTo(nullptr, worldPos);
        }
        break;

    case MapConstants::MoveMapByWorldPos:
        {
            auto m = noti->getBody().toMap();
            auto x = m["x"].toInt();
            auto y = m["y"].toInt();
            auto anim = m["anim"].toBool();
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();

            VSMPoint2i world{x, y};
            VSMPoint2f bessel;
            VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84, bessel);

            QString bx; bx.setNum(bessel.x, 'g', 12);
            QString by; by.setNum(bessel.y, 'g', 12);
            auto str = by + ", " + bx;
            qDebug() << "[MAP] MoveMapByWorldPos" << x << "," << y << " -> " << str;

            mapProxy->moveTo(nullptr, QPoint{x, y}, anim);
        }
        break;

    case MapConstants::RequestMapPosition: {
            //MMV 18.4.18
            auto m = noti->getBody().toMap();
            auto mapView = m["mapView"].value<QObject*>();            

            auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto pt = proxy->centerPosition(mapView);

            auto type = m[CommonConstants::Type].toString();

            if (type.contains(CommonConstants::Favorite)) {
                m[CommonConstants::X] = pt.x();
                m[CommonConstants::Y] = pt.y();
                m[CommonConstants::Type] = type;

                sendNotification(AddressConstants::RequestAddress, m);
            } else if (type.contains(QStringLiteral("center"))) {
                sendNotification(MapConstants::ResponseMapPosition, pt);
            } else {
                if (!m.contains(CommonConstants::X)) {
                    m[CommonConstants::X] = pt.x();
                    m[CommonConstants::Y] = pt.y();
                }
                sendNotification(RouteConstants::AddVia, m);
            }
        }
        break;

    case MapConstants::ShowCarvata: {
            T_DEBUG( QString("MapConstants::ShowCarvata show: %1").arg(noti->getBody().toInt()) );
            showCarvata(noti->getBody());
        }
        break;

    case MapConstants::ShowRoute:
        {
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto m = noti->getBody().toMap();
            QObject* mapView = nullptr;
            if (m.contains("mapView")) {
                mapView = m["mapView"].value<QObject*>();
            }
            auto show = m["show"].toBool();
            mapProxy->setShowRoute(mapView, show);
        }
        break;

    case MapConstants::RequestMapScale: {
            auto m = noti->getBody().toMap();
            auto mapView = m["mapView"].value<QObject*>();

            VSM_Int32 level;
            VSM_Int32 subLevel;
            auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            proxy->viewLevel( mapView, level, subLevel );

            sendNotification(MapConstants::ResponseMapScale, level);
        }
        break;

    case MapConstants::ResetMapPosition: {
            //MMV 18.4.18
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();

            QObject* mapView = nullptr;
            if ( noti->getBody().canConvert(QVariant::Map) ) {
                auto m = noti->getBody().toMap();
                mapView = m["mapView"].value<QObject*>();
            }

            auto pos = naviProxy->currentPosition();
            mapProxy->moveTo(mapView, pos);
        }
        break;

    //MMV 18.4.18
    case MapConstants::RequestMapRect:
        {
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto m = noti->getBody().toMap();
            auto mapView = m["mapView"].value<QObject*>();
            QRect rect = mapProxy->requestMapRect(mapView);
            sendNotification(MapConstants::ResponseMapRect, rect);
        }
        break;
    //MMV 18.4.18
    case MapConstants::DrawRouteAll:
        {
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            if (naviProxy->isSetTvas()) {
                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                auto isArrivalBelowDeparture = searchProxy->isArrivalBelowDeparture();
                auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
                auto m = noti->getBody().toMap();
                auto mapView = m["mapView"].value<QObject*>();
                auto caller = m[CommonConstants::Caller].toString();
                auto isSummaryView = caller == ViewRegistry::objectName(ViewRegistry::RouteSummaryView);
                mapProxy->drawRouteAll(mapView, isArrivalBelowDeparture, isSummaryView);
            }
        }
        break;

    case RouteConstants::AddVia:
        addVia(noti->getBody());
        break;

    case RouteConstants::InsertVia:
        insertVia(noti->getBody());
        break;

    case RouteConstants::DeleteVia:
        deleteVia(noti->getBody());
        break;

    case RouteConstants::SelectViaSlot:
        selectViaSlot(noti->getBody());
        break;

    case RouteConstants::SetDeparture:
        setDeparture(noti->getBody());
        break;

    case RouteConstants::SetArrival:
        setArrival(noti->getBody());
        break;

    case RouteConstants::SetVia:
        setVia(noti->getBody());
        break;

    case RouteConstants::SetStartCode:
        setStartSearchCode(noti->getBody());
        break;

    case MapConstants::RequestMapAreaName:
        {
            auto m = noti->getBody().toMap();
            auto wx = 0;
            auto wy = 0;
            if (m.contains(CommonConstants::X)) {
                wx = m[CommonConstants::X].toInt();
            }
            if (m.contains(CommonConstants::Y)) {
                wy = m[CommonConstants::Y].toInt();
            }

            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();

            if (wx == 0 || wy == 0) {
                auto pt = mapProxy->centerPosition(nullptr);
                wx = pt.x();
                wy = pt.y();
            }
            auto areaName = mapProxy->areaName(wx, wy);
            sendNotification(MapConstants::ResponseMapAreaName, areaName);
        }
        break;

    case RouteConstants::AddSearchMarker:
        addSearchMarker(noti->getBody());
        break;

    case RouteConstants::RemoveSearchMarker:
        removeSearchMarker(noti->getBody());
        break;

    case MapConstants::ChangeViewLevelUsingApi:
        {
            QString srcName = DebugUtils::name( noti->getSource() );
            // MMV 18.4.18

            auto m = noti->getBody().toMap();
            auto level = m["level"].toInt();
            qDebug() << "[GlobalMap][ManageVsmMapCommand]ChangeViewLevelUsingApi source: " << srcName << " level: " << level;
            changeViewLevel(noti->getBody());
        }
        break;

    case MapConstants::ChangeMapViewMode:
        {
            auto m = noti->getBody().toMap();
            auto viewMode = m[CommonConstants::Mode].toInt();
            switch (viewMode) {
            case MapViewMode::MapInquiry:
            //MMV 18.4.18
                removeCalloutPopup(m);
                break;
            }
        }
        break;

    case MapConstants::RemoveCalloutPopup:
        //MMV 18.4.18 removeCalloutPopup(false)
        removeCalloutPopup(noti->getBody());
        break;

    case MapConstants::ShowGasStationInfo:
        //MMV 18.4.18 showGasStationInfo(true)
        showGasStationInfo(noti->getBody());
        break;

    case MapConstants::HideGasStationInfo:
        //MMV 18.4.18 showGasStationInfo(false)
        hideGasStationInfo(noti->getBody());
        break;

    case MapConstants::ChangeAutoZoom:
        changeAutoZoom(noti->getBody());
        break;

    case MapConstants::ApplyMapSetting:
        {
            auto m = noti->getBody().toMap();
            auto mapView = m["mapView"].value<QObject*>();
            auto type = m["type"].toInt();
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto setting = mapProxy->setting((VsmMapType)type);
            mapProxy->applySetting(mapView, setting);
        }
        break;

    case MapConstants::ApplyMapTheme:
        {
            auto m = noti->getBody().toMap();
            auto mapView = m["mapView"].value<QObject*>();
            auto type = (VSM_ThemeType)m["type"].toInt();
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto theme = mapProxy->theme(VSM_THEME_DAY);
            mapProxy->applyTheme(mapView, type, theme);
            theme = mapProxy->theme(VSM_THEME_NIGHT);
            mapProxy->applyTheme(mapView, type, theme);
        }
        break;

    default:
        return false;
    }

    return true;
}

//MMV 18.4.18
void ManageVsmMapCommand::addVia(const QVariant& v)
{
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    auto updateDeparture = searchProxy->viaSlot() < 0;


    QVariantMap m = v.toMap();

    auto caller = m[CommonConstants::Caller].toString();

    RouteManager::RouteItem item;
    item.poiId = m[SearchConstants::PoiID].toInt();
    item.navSeq = m[SearchConstants::NavSeq].toInt();
    item.rpFlag = m[SearchConstants::RpFlag].toInt(); //'c' unknown
    item.address = m[CommonConstants::Address ].toString();
    item.name = m[CommonConstants::Name].toString();
    item.poleCate = m[SearchConstants::OpiPoleCate].toInt();

    int x = 0;
    int y = 0;
    if (m.contains(CommonConstants::X) && m.contains(CommonConstants::Y)) {
        x = m[CommonConstants::X].toInt();
        y = m[CommonConstants::Y].toInt();
    } else {
        auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
        auto mapView = m["mapView"].value<QObject*>();
        //MMV 18.4.18
        auto center = proxy->centerPosition(mapView);
        x = center.x();
        y = center.y();
    }

    item.wx = x;
    item.wy = y;

    auto slot = searchProxy->addVia(item);

    m.clear();

    // 경유지 추가 실패
    if (slot < 0) {
        if (searchProxy->hasEmptyVia()) {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            auto isRg = rgProxy->isRg();
            auto const errorMessage = tr("선택한 위치와 동일한 지점이\n등록되어 있습니다.\n다시 확인해 주세요.");
            QVariantMap m;
            if (isRg) {
                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
                m[CommonConstants::ParentObjectName] = CommonConstants::ToastWrap;
                m[QStringLiteral("title")] = tr("알림");
                m[QStringLiteral("message")] = errorMessage;
                m["showTimer"] = true;
                PlatformHelper::instance()->setParentForSystemPopup(m);
                sendNotification(Constants::CreateView, m);

                // 주행 중 경로 복구.
                sendNotification(RouteConstants::ResumeRoute);
            } else {
                // 추가 가능한 경유지가 있는데 실패한 것은 동일 지점이 있는 경우.
                m["errorMessage"] = errorMessage;
                sendNotification(RouteConstants::CheckViaValidateIsFail, m);
            }
        } else {
            m[CommonConstants::Position] = ToastsConstants::ToastUp;
            m[CommonConstants::Message] = tr("더 이상 경유지 추가가 불가능합니다.");
            sendNotification(ToastsConstants::ShowToast, m);
        }
    } else {
        // 주행중 경유지 추가인 경우 출발지를 현재 위치로 초기화 한다.
        // 19.09.27 ymhong
        if (updateDeparture) {
            setDeparture(QVariantMap{
                             {SearchConstants::Clear, false}
                         });
        }

        // 인터페이스로 startGuidance를 호출 하면
        // 목적지가 onWaypointChanged()를 통해 전달 된다.
        // 인터페이스로 전달 된 경우에는 경유지 설정 화면을 띄우지 않도록 함.
        // 19.06.18 ymhong
        if (caller != "Interface") {
            searchProxy->sendWaypointProperties();

            m.clear();
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteSettingView);
            m[CommonConstants::Caller] = caller;
            sendNotification(Constants::CreateView, m);

            if (caller == ViewRegistry::objectName(ViewRegistry::MapControlView)) {
                sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, caller}});
            }
        }
    }
}

void ManageVsmMapCommand::insertVia(const QVariant &v)
{
    auto m = v.toMap();
    auto slot = m[CommonConstants::Slot].toInt();

    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    RouteManager::RouteItem item;
    item.wx = x;
    item.wy = y;

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    if (searchProxy->insertVia(item)) {
        searchProxy->sendWaypointProperties();
    } else {
        sendNotification(ToastsConstants::ShowToast, QVariantMap{
                             {CommonConstants::Position, ToastsConstants::ToastUp},
                             {CommonConstants::Message, tr("더 이상 경유지 추가가 불가능합니다.")}
                         });
    }
}

void ManageVsmMapCommand::deleteVia(const QVariant &v)
{
    auto m = v.toMap();
    auto begin = m["begin"].toInt();
    auto end = m["end"].toInt();

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->deleteVia(begin, end);
    searchProxy->sendWaypointProperties();
}

void ManageVsmMapCommand::selectViaSlot(const QVariant &v)
{
    auto m = v.toMap();
    auto slot = m[CommonConstants::Slot].toInt();
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->setViaSlot(slot);
}

void ManageVsmMapCommand::setVia(const QVariant &v)
{
    auto m = v.toMap();
    auto points = m["points"].value<QRoutePosListPtr>();
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    auto size = points->size();
    for (int i=0; i<size; i++) {
        auto point = points->at(i);
        RouteManager::RouteItem item;
        item.set(point);
        searchProxy->setVia(i, item);
    }
    searchProxy->sendWaypointProperties();
}

void ManageVsmMapCommand::setStartSearchCode(const QVariant &v)
{
    auto startSearchCode = v.toInt();
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->setStartSearchCode(startSearchCode);
}

void ManageVsmMapCommand::setArrival(const QVariant &v)
{
    QVariantMap m = v.toMap();

    int x = 0;
    int y = 0;
    QString name = m[CommonConstants::Name].toString();
    QString address = m[CommonConstants::Address].toString();

    auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();

    if (m.contains(CommonConstants::X) && m.contains(CommonConstants::Y)) {
        x = m[CommonConstants::X].toInt();
        y = m[CommonConstants::Y].toInt();
    } else {
        auto mapView = m["mapView"].value<QObject*>();
        //MMV 18.4.18
        auto center = proxy->centerPosition(mapView);
        x = center.x();
        y = center.y();

        T_DEBUG( QString( "#1 x:%1 y:%2" ).arg( x ).arg( y ) );
    }

    auto areaName = proxy->areaName(x, y);
    if (name.isEmpty() && address.isEmpty() && areaName.isEmpty()) {
        return;
    }
    if (address.isEmpty()) {
        address = areaName;
    }
    if (name.isEmpty()) {
        name = areaName;
    }

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    auto skipSendWaypointOperation = m["skipSendWaypointOperation"].toBool();
    if (!skipSendWaypointOperation) {
        auto changeDestination = m["changeDestination"].toBool();
        auto hasVia = false;
        if (changeDestination) {
            for (auto i = 1; i < 4; i++) {
                if (searchProxy->hasVia(i)) {
                    hasVia = true;
                    break;
                }
            }
            if (hasVia) {
                QList<int> status = {
                    WaypointOperationStatus::DELETED,
                    WaypointOperationStatus::REPLACED
                };
                searchProxy->sendWaypointOperationStatus(status);
            } else {
                searchProxy->sendWaypointOperationStatus(WaypointOperationStatus::REPLACED);
            }

            // 목적지 변경이면 RouteConstants::SetDeparture를 보내지 않기 때문에 경유지를 여기서 clear
            setDeparture(QVariantMap{
                             {SearchConstants::Clear, true}
                         });
        } else {
            searchProxy->sendWaypointOperationStatus(WaypointOperationStatus::INSERTED);
        }
    }

    auto poiId = m[SearchConstants::PoiID].toInt();
    auto navSeq = m[SearchConstants::NavSeq].toInt();
    auto rpFlag = m[SearchConstants::RpFlag].toInt();
    auto poleCate = m[SearchConstants::OpiPoleCate].toInt();
    auto destSearchCode = m[SearchConstants::DestType].toInt();

    RouteManager::RouteItem item;
    item.wx = x;
    item.wy = y;
    item.navSeq = navSeq;
    item.rpFlag = rpFlag;
    item.poiId = poiId;
    item.name = name;
    item.address = address;
    item.poleCate = poleCate;
    item.searchFlag = destSearchCode;
    
    searchProxy->setVia(RouteItem_End, item);
}

//MMV 18.4.18
void ManageVsmMapCommand::setDeparture(const QVariant &v)
{
    QVariantMap m = v.toMap();

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    bool isClear = true;

    if (m.contains(SearchConstants::Clear)) {
        isClear = m[SearchConstants::Clear].toBool();
    }
    if (isClear) {
        searchProxy->clearVia();
    }

    int x = 0;
    int y = 0;
    QString name = m[CommonConstants::Name].toString();
    QString address = m[CommonConstants::Address].toString();

    if (m.contains(CommonConstants::X) && m.contains(CommonConstants::Y)) {
        x = m[CommonConstants::X].toInt();
        y = m[CommonConstants::Y].toInt();
    } else {
        auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
        auto center = proxy->currentPosition();
        x = center.x();
        y = center.y();
    }

    auto poiId = m[SearchConstants::PoiID].toInt();
    auto navSeq = m[SearchConstants::NavSeq].toInt();
    auto rpFlag = m[SearchConstants::RpFlag].toInt();

    RouteManager::RouteItem item;
    item.wx = x;
    item.wy = y;
    item.navSeq = navSeq;
    item.rpFlag = rpFlag;
    item.poiId = poiId;
    item.name = name;
    item.address = address;

    searchProxy->setVia(RouteItem_Start, item);
}

void ManageVsmMapCommand::update(QObject* mapView)
{
    auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
    proxy->update(mapView);
}

void ManageVsmMapCommand::showCarvata(const QVariant &v)
{
    QObject* mapView = nullptr;
    bool show = false;
    if ( v.canConvert(QVariant::Map) ) {
        auto m = v.toMap();
        mapView = m["mapView"].value<QObject*>();
        show = m["show"].toBool();
    }
    else {
        show = v.toBool();
    }

    auto proxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
    proxy->showCarvata(mapView, show);
}

//MMV 18.4.18
void ManageVsmMapCommand::addSearchMarker(const QVariant& v)
{
    auto m = v.toMap();
    auto mapView = m["mapView"].value<QObject*>();

    int index = 0;
    int focusIdx = 0;
    int x = 0;
    int y = 0;
    QString text = "";

    index = m[CommonConstants::Index].toInt();
    focusIdx = m[SearchConstants::FocusIndex].toInt();
    x = m[CommonConstants::X].toInt();
    y = m[CommonConstants::Y].toInt();
    text = m[CommonConstants::Text].toString();

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->addSearchMarker(mapView, index, focusIdx, text, x, y);
}

void ManageVsmMapCommand::removeSearchMarker(const QVariant& v)
{
    auto m = v.toMap();
    auto mapView = m["mapView"].value<QObject*>();

    int index = 0;
    int x = 0;
    int y = 0;

    index = m[CommonConstants::Index].toInt();
    x = m[CommonConstants::X].toInt();
    y = m[CommonConstants::Y].toInt();

    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->removeSearchMarker(mapView, index,  x, y);
}

void ManageVsmMapCommand::changeViewLevel(const QVariant& v)
{
    // MMV 18.4.18
    auto m = v.toMap();
    auto mapView = m["mapView"].value<QObject*>();

    auto level = m["level"].toInt();
    auto subLevel = m["subLevel"].toInt();

    auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
    mapProxy->setViewLevel(mapView, level, subLevel, true);
}

void ManageVsmMapCommand::changeAutoZoom(const QVariant& v)
{
    // MMV 18.4.18
    QObject* mapView = nullptr;
    bool autoZoom = false;
    if ( v.canConvert(QVariant::Map) ) {
        auto m = v.toMap();
        mapView = m["mapView"].value<QObject*>();
        autoZoom = m["show"].toBool();
    }
    else {
        autoZoom = v.toBool();
    }

    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    naviProxy->setAutoZoom(autoZoom);
}

void ManageVsmMapCommand::showGasStationInfo(const QVariant& v)
{
    // MMV 18.4.18
    QObject* mapView = nullptr;
    bool show = true;
    if ( v.canConvert(QVariant::Map) ) {
        auto m = v.toMap();
        mapView = m["mapView"].value<QObject*>();
        if (m.contains("show")) {
            show = m["show"].toBool();
        }
    }
    else {
        if (v.isValid()) {
            show = v.toBool();
        }
    }

    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    naviProxy->showGasStationInfo(mapView, show);
}

void ManageVsmMapCommand::hideGasStationInfo(const QVariant& v)
{
    // MMV 18.4.18
    QObject* mapView = nullptr;
    bool show = false;
    if ( v.canConvert(QVariant::Map) ) {
        auto m = v.toMap();
        mapView = m["mapView"].value<QObject*>();
        if (m.contains("show")) {
            show = m["show"].toBool();
        }
    }
    else {
        if (v.isValid()) {
            show = v.toBool();
        }
    }

    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    naviProxy->showGasStationInfo(mapView, show);
}

void ManageVsmMapCommand::removeCalloutPopup(const QVariant& v)
{
    // MMV 18.4.18
    QObject* mapView = nullptr;
    bool animate = false;
    if ( v.canConvert(QVariant::Map) ) {
        auto m = v.toMap();
        mapView = m["mapView"].value<QObject*>();
        animate = m["animate"].toBool();
    }
    else {
        animate = v.toBool();
    }

    auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
    mapProxy->removeCalloutPopup(mapView, animate);
}

}//SKT
