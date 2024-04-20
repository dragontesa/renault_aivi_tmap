#include "ManageVsmNaviCommand.h"
#include "Constants.h"
#include "RouteConstants.h"

#include <QQmlProperty>

#include "Facade.h"
#include "VSM_Navi_RgData.h"
#include "VSM_Navi.h"
#include "RouteSummaryVo.h"
#include "RouteCandidateListModel.h"
#include "VsmNaviProxy.h"
#include "VsmMapProxy.h"
#include "RouteGuidanceProxy.h"
#include "RouteSearchProxy.h"
#include "SettingProxy.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include "SoundConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProxy.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "RgConstants.h"
#include "MNS_Define.h"
#include "RouteSearchProxy.h"
#include "RouteSummaryHelper.h"
#include "util/RpErrorHelper.h"
#include "business/view/mediator/app/MapViewMode.h"

#define RouteIdKey  QStringLiteral("routeId")

namespace SKT
{

ManageVsmNaviCommand::ManageVsmNaviCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

ManageVsmNaviCommand::~ManageVsmNaviCommand()
{
    T_DEBUG("I'm done...");
}

bool ManageVsmNaviCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case MapConstants::AttachMapViewOnVsmNavi:
        {
            //MMV 18.4.18 
            auto m = noti->getBody().value<QVariantMap>();
            auto mapView = m[QStringLiteral("mapView")].value<QObject*>();

            VSM_ViewType type = VSM_VIEW_TYPE_NORMAL;
            if (m.contains(QStringLiteral("type"))) {
                type = (VSM_ViewType)m[QStringLiteral("type")].toInt();
            }

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->attachMapView( mapView, type );
            auto routeProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            routeProxy->attachMapView( mapView );
        }
        break;

    case MapConstants::DetachMapViewFromVsmNavi:
        {
            //MMV 18.4.18
            auto mapView = noti->getBody().value<QObject*>();

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->detachMapView( mapView );
            auto naviControlProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            naviControlProxy->removeMapView( mapView );
            auto routeProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            routeProxy->detachMapView( mapView );
        }
        break;

    case MapConstants::ApplyMapViewMode:
        {
            auto m = noti->getBody().toMap();
            auto mapViewMode = (MapViewMode::ModeK)m[CommonConstants::Mode].toInt();
            auto mapViewModeReason = (MapViewMode::ReasonK)m[QStringLiteral("reason")].toInt();
            auto lastMapViewDriveMode = (MapViewMode::ModeK)m[QStringLiteral("lastDriveMode")].toInt();

            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            auto isRg = rgProxy->isRg();
            if (isRg) {
                auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
                auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
                switch (mapViewMode) {
                case MapViewMode::Navi:
                    switch (mapViewModeReason) {
                    case MapViewMode::None:
                    case MapViewMode::MapViewCreated:
                    case MapViewMode::MapViewActivated:
                    case MapViewMode::ReturnFromNormalMode:
                    case MapViewMode::ReturnFromMapControlMode:
                    case MapViewMode::ReturnFromMapInquiryMode:
                    case MapViewMode::ReturnFromNaviMode:
                    case MapViewMode::ReturnFromMenuNaviMode:
                    case MapViewMode::ReturnFromSimulMode:
                    case MapViewMode::CancelRoute:
                    case MapViewMode::RecentTileGridViewLostFocus:
                        mapProxy->setShowRoute(nullptr, true);
                        break;
                    }
                    break;

                case MapViewMode::MapInquiry: // 조회 모드
                    mapProxy->setShowRoute(nullptr, false);
                    break;

                case MapViewMode::MenuNavi: // 17.10.10 hskim 내비메뉴: 환경설정 화면 맵 주행 상태 표시를 위해 추가(sdi 표시: 클릭x)
                    mapProxy->setShowRoute(nullptr, true);
                    break;

                case MapViewMode::MapControl: // 맵을 터치했을 때
                case MapViewMode::Normal: //
                case MapViewMode::Simul:
                case MapViewMode::NormalMapControl:
                    // do nothing.
                    break;
                }
            }
        }
        break;

    case RouteConstants::SetTvas: {
        auto m = noti->getBody().toMap();
        if (m.isEmpty()) {
            setTvas(noti->getBody().toList());
        } else {
            auto list = m["list"].toList();
            auto colors = m["colors"].toStringList();
            auto switchToLocal = m[RPREQ_KEY_SwitchToLocal].toBool();
            setTvas(list, colors, switchToLocal);
        }
        break;
    }

    case RouteConstants::ResumeRoute:
        {
            auto naviControlProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto isOnline = naviControlProxy->getOnlineState() && naviControlProxy->isChannelServerPolicyFinished();
            auto isLocal = !isOnline;

            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            if (rgProxy->isRg()) {
                // 경로 안내중일 때만 복구
                auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
                naviProxy->resumeRoute(isLocal);
                naviProxy->blockReroute(false);

                sendNotification(RgConstants::BlockReroute, false);
            }
        }
        break;

    case RouteConstants::RequestRouteSummary:
        summaryRoutes(noti->getBody());
        break;

    case RouteConstants::RequestRouteSummaryDetail:
        {
            QVariantMap m = noti->getBody().toMap();
            int routeId = -1;

            if (m.contains(RouteIdKey))
                routeId = m[RouteIdKey].toInt();

            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();

            if (routeId < 0) {
                // 경로 안내 중 경로요약 화면 옵션 선택 시
                auto summary = proxy->summary(routeId);
                QVariantMap m;
                m["summary"] = summary;
                sendNotification(RouteConstants::UpdateRouteSummary, m);

                m.clear();
                proxy->fillCachedSummary(m);
                proxy->fillCachedWeather(m);
                sendNotification(RouteConstants::UpdateRouteSummarySimple, m);
            } else {
                // 경로 요약 -> 경로 상세 이행 시
                auto m = noti->getBody().toMap();
                proxy->fillCachedWeather(m);
                sendNotification(RouteConstants::UpdateRouteSummarySimple, m);//noti->getBody());
            }
            getDetailRoute(routeId);
        }
        break;

    case RouteConstants::SelectRoute: {
            auto body = noti->getBody();
            auto m = body.toMap();
            auto routeId = m[RouteIdKey].toInt();
            auto option = m["option"].toInt();
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            proxy->selectRoute(routeId);
            proxy->cacheSummary(body);

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->setRouteOption(option);

            auto distance = m["distance"].toInt();
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->setTotalDistance(distance);
            rgProxy->setSimulatorTotalDistance(distance);
        }
        break;

    case RouteConstants::CancelRoute: {
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            proxy->cancelRoute();
            proxy->blockReroute(true);

            auto m = noti->getBody().toMap();
            const QString clearKey = "clear";
            auto isClear = m[clearKey].toBool();

            if (isClear) {
                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                searchProxy->clearVia();
            }
            sendNotification(RouteConstants::UpdateRequestRouteDebugText, QString{});
        }
        break;

    case SoundConstants::PlaySound: {
            QVariantMap m = noti->getBody().toMap();
            int soundId = m["soundPlayScenario"].toInt();
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            proxy->soundPlayScenario(soundId);
        }
        break;

    case NavigationControllerConstants::UpdateCavatarIcon: {
        auto m = noti->getBody().toMap();
        auto mapView = m["mapView"].value<QObject*>();
    	auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
        proxy->updateCavatarIcon(mapView);
    }
    break;

    case NavigationControllerConstants::SetCavatarIcon: {
        QObject* mapView = nullptr;
        int cavatarType = 0;
        if ( noti->getBody().canConvert(QVariant::Map) ) {
            auto m = noti->getBody().toMap();
            mapView = m["mapView"].value<QObject*>();
            cavatarType = m["cavatarType"].toInt();
        }
        else {
            cavatarType = noti->getBody().toInt();
        }

   	    auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
        proxy->setCavatarIcon(mapView, cavatarType);
    }
    break;

    case RouteConstants::ChangeReRouteIntervalTime: {
        int intervalTime = noti->getBody().toInt(); // server if the value is 0, otherwise miliseconds
        qDebug() << "ChangeReRouteIntervalTime : " << intervalTime;
        if (0 != intervalTime) {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->setPeriodicRerouteTime(intervalTime);
        }
        break;
    }
    break;

    default:
        return false;
    }

    return true;
}

void ManageVsmNaviCommand::summaryRoutes(const QVariant &data)
{
    auto m = data.toMap();
    auto type = m["type"].toString();
    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    auto routeId = m["routeId"].toInt();
    auto routeOption = m["routeOption"].toInt();
    auto byVr = m["byVr"].toBool();

    QVariantMap m2;

    m2["byVr"] = byVr;
    if (byVr) {
        m2["routeOption"] = routeOption;
        auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();

        m2["apply"] = searchProxy->applyTimer();

        searchProxy->setApplyTimer(true);
    }

    bool isDistanceOver = false;
    if (type == "local") {
        auto summary = naviProxy->summary(routeId);
        if (summary.isValid()) {
            auto vo = summary.value<RouteSummaryVoPtr>();
            auto type = RouteSummaryHelper::routeToString(static_cast<TeRouteOption>(routeOption));
            vo->setType(type);

            if (vo->isDistanceOver(false)) {
                isDistanceOver = true;
            } else {
                m2["summary"] = summary;
                sendNotification(RouteConstants::UpdateRouteSummary, m2);
            }
        } else {
            sendNotification(RouteConstants::UpdateRouteSummary, QVariantMap{
                                 {"summary", summary}
                             });
        }
    } else {
        QVariantList summaries;
        for (int i=0; i<3; i++) {
            auto summary = naviProxy->summary(i);
            if (!summary.isNull()) {
                auto vo = summary.value<RouteSummaryVoPtr>();
                if (vo->isDistanceOver(true)) {
                    isDistanceOver = true;
                    break;
                }
                summaries.append(summary);
            }
        }
        if (!isDistanceOver) {
            m2["summaries"] = summaries;
            sendNotification(RouteConstants::UpdateRouteSummary, m2);
        }
    }

    if (isDistanceOver) {
        // Cancel request
        QVariantMap m;
        m.insert(QStringLiteral("RequestType"), RPRequest::RequestCancel);
        sendNotification(SearchConstants::RequestCommand, m);

        // Show Error view dialog
        int errorCode = E_ERROR_ROUTE_DISTOVER;

        bool cancelRoute = true;
        RpErrorHelper::exec(this, errorCode, cancelRoute);
    }
}

void ManageVsmNaviCommand::getDetailRoute(int routeId)
{
    auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    auto model = proxy->getDetailRoute(routeId);

    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();

    QVariantMap m;
    m["model"] = model;
    m["remainDistance"] = rgProxy->remainDistance();
    sendNotification(RouteConstants::UpdateRouteSummaryDetail, m);
}

bool ManageVsmNaviCommand::setTvas(const QVariantList &list, const QStringList &colors, bool switchToLocal)
{
    QList<quint32> options;
    QStringList filenames;

    auto naviControlProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    auto isOnline = naviControlProxy->getOnlineState() && naviControlProxy->isChannelServerPolicyFinished();
    auto isLocal = !isOnline || switchToLocal;

    for (int i=0; i<list.count()/2; i++) {
        options.append(list.at(i*2).toInt());
        filenames.append(list.at(i*2+1).toString());
    }
    auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
    bool result;
    if (colors.isEmpty()) {
        result = proxy->setTvasData(filenames, options, isLocal);
    } else {
        result = proxy->setTvasDataWithColorTable(filenames, options, colors, isLocal);
    }
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->setTvasValid(result);
    return true;
}

bool ManageVsmNaviCommand::setTvas(const QVariantList &list)
{
    auto switchToLocal = false;
    return setTvas(list, QStringList{}, switchToLocal);
}

}//SKT
