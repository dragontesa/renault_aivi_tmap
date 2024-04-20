#include "ManageRouteSearchCommand.h"
#include <QQmlProperty>
#include "ApplicationConstants.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "RequestCommand.h"
#include "PreferenceConstants.h"
#include "RouteGuidanceProxy.h"
#include "RouteSearchProxy.h"
#include "RouteSearchResultHelper.h"
#include "RouteSummaryHelper.h"
#include "SoundConstants.h"
#include "ViewRegistry.h"
#include "VsmMapProxy.h"
#include "VsmNaviProxy.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProxy.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "../platforms/linux-gen3/interface/server/navigation/navigationservicetype.h"

#include "MapConstants.h"
#include "UtilConstants.h"
#include "util/RpErrorHelper.h"
#include "PlatformHelper.h"

using namespace SKT;

ManageRouteSearchCommand::ManageRouteSearchCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

ManageRouteSearchCommand::~ManageRouteSearchCommand()
{
    T_DEBUG( "I'm done..." );
}

bool ManageRouteSearchCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case NavigationControllerConstants::ChangeOnlineStatus:
        {
            auto isOnline = noti->getBody().toBool();
            auto proxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            proxy->setOnline(isOnline);

            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->setOnline(isOnline);
        }
        break;

    case RouteConstants::RequestRouteAsDestination:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            auto result = searchProxy->checkViaIsNear();
            auto success = result == CheckViaResultOk;

            // 새로운 목적지를 설정하면 기존 경로는 취소.
            // artf261781
            // 17.10.27 ymhong
            // 실패한 경우라도 경로 요청을 하였으므로 기존 경로가 취소 되도록 한다
            // 19.09.03 ymhong
            sendNotification( RgConstants::StopRouteGuidance );
            sendNotification(RgConstants::BlockReroute, true);

            sendNotification(RouteConstants::ResponseRouteAsDestination, success);

            if (success) {
                sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)}});
            } else {
                QVariantMap m;

                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
                m[QStringLiteral("title")] = tr("알림");
                m[QStringLiteral("message")] = tr("현위치와 동일한 지점입니다.\n다시 확인해 주세요.");
                m["showTimer"] = true;
                PlatformHelper::instance()->setParentForSystemPopup(m);

                // TMCRSM-3694
                // 목적지 변경 팝업과 동일한 시스템의 내비팝업을 공유하고 있기 때문에 딜레이를 주어 다이얼로그를 표시한다.
                // 19.08.27 ymhong
                sendNotification(RouteConstants::ShowErrorDialog, m);
            }
        }
        break;

    case SearchConstants::RequestCommand:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            auto data = noti->getBody().value<QVariantMap>();

            if (!data.contains(RPREQ_KEY_Online)) {
                data[RPREQ_KEY_Online] = searchProxy->isRpServerOnline();
            }
            auto requestType = data[RPREQ_KEY_RequestType].toInt();
            auto isOnline = data[RPREQ_KEY_Online].toBool();
            bool isReroute = false;
            bool isFillVia = true;
            bool isRp = true;

            switch (requestType) {
            case RPRequest::RequestRoute:
                searchProxy->relocationViaInfo();

                // NDDS용 지난 위치 정보 설정
                if (isOnline) {
                    searchProxy->fillLastGoalData(data);
                    searchProxy->fillRerouteData(data);

                    if (searchProxy->isRpContinue()) {
                        searchProxy->setRpContinue(false);
                    }
                } else {
                    searchProxy->fillLocalRerouteData(data);
                }
                searchProxy->resetLastGoalData();
                if (data.contains(RPREQ_KEY_Apply)) {
                    auto isApply = data[RPREQ_KEY_Apply].toBool();
                    if (isApply) {
                        // 내비 진입 전 VR에서 경로 탐색 시 경로 이어하기 팝업이 나오지 않도록 처리.
                        PlatformHelper::instance()->disableRgContinue();
                    }
                }
                break;

            case RPRequest::RequestNone:
            case RPRequest::RequestCancel:
                isFillVia = false;
                isRp = false;
                break;

            case RPRequest::RequestReRouteForce:
            case RPRequest::RequestReRouteByAuto:
            case RPRequest::RequestReRouteByBreakaway:
                isReroute = true;
                if (isOnline) {
                    searchProxy->fillRerouteData(data);
                } else {
                    searchProxy->fillLocalRerouteData(data);
                }
                break;

            case RPRequest::RequestRouteSummary:
            case RPRequest::RequestVMSInfo:
                isRp = false;
                break;
            }

            // 서버 연결 중 reroute를 하면 "via1"이 null이어야
            // 서버 경로 요청 시 현재 위치로 via1을 채우게 된다.
            // 17.07.26 ymhong.
            if (isFillVia) {
                bool skipDeparture = searchProxy->isRpServerOnline() && isReroute;
                searchProxy->fillViaInfo(data, skipDeparture);
                searchProxy->fillDepartureInfo(data);
            }

            searchProxy->fillAngleAndSpeed(data);
            if (isRp) {
                // local tvas가 설정되고 나서 CancelRoute전에 VSMNavi_GetLinkTraceData()
                // 를 호출하면 Crash 발생
                // 19.11.21 ymhong.
                searchProxy->fillGpsTraceData(false, data);
            }

            // IntegratedSearchProxy와 RouteSearchProxy의 onResponse에서 자기 것이 아닌 응답을 무시할 수 있도록 구분 타입 추가
            data[SearchConstants::SearchType] = CommonConstants::RouteSearch;

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto oilType = naviProxy->fuelType();
            data[SettingConstants::OilKind] = oilType;
            auto chargeType = naviProxy->chargeType();
            data[SettingConstants::ChargeType] = chargeType;

            auto notifyMethod = std::bind(&ManageRouteSearchCommand::onResponseCommand, this, std::placeholders::_1);
            auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;
            searchProxy->requestCommand( req );

            ///////////////////////////////////////////////////////////////////////
            //[MonkeyTest]를 위하여
            QVariantMap m;
            m.insert(CommonConstants::Name,    CommonConstants::RouteSearch);
            m.insert(CommonConstants::Value,   true);
            m.insert(CommonConstants::Type,    0);
            m.insert(CommonConstants::Keyword, CommonConstants::Empty);
            sendNotification(SearchConstants::SearchWait, m);
            //[MonkeyTest]를 위하여
            ///////////////////////////////////////////////////////////////////////


            if (requestType == RPRequest::RequestRouteSummary)
                searchProxy->clearVia();

            if (requestType == RPRequest::RequestRoute
                || requestType == RPRequest::RequestRouteSummary
                || requestType == RPRequest::RequestVMSInfo) {
                // Donot change navi state while rg, 2019.5.30 required by HyunSoo
                if (!Facade::instance()->retrieveProxy<RouteGuidanceProxy>()->isRg()) {
                    auto isRp = requestType == RPRequest::RequestRoute;
                    m.clear();
                    m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                    m["propertyValue"] = isRp ? TMAPNAVISTATUS_CALCULATING_ROUTE : TMAPNAVISTATUS_BUSY;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
                }
            }
            auto command = data["Command"].toString();
            if (command == "ChangeOption") {
                m.clear();
                m["type"] = NavigationPropertyProtocol::TmapRouteRecalculationReason;
                m["propertyValue"] = ROUTE_CRITERION_CHANGED;
                sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
            }
            if (requestType == RPRequest::RequestRoute) {
                searchProxy->sendWaypointProperties();
            }
        }
        break;

    case RouteConstants::RequestReroute:
    case RouteConstants::RequestSkipViaReroute:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            QVariantMap data = noti->getBody().value<QVariantMap>();
            auto isOnline = searchProxy->isRpServerOnline();
            bool skipDeparture = isOnline;
            auto skipWaypoint = noti->getId() == RouteConstants::RequestSkipViaReroute;

            if (skipWaypoint) {
                auto mapView = data["mapView"].value<QObject*>();
                searchProxy->skipNextVia(mapView);

                searchProxy->sendWaypointProperties();
            }

            searchProxy->fillViaInfo(data, skipDeparture);
            bool isReroute = true;
            searchProxy->fillDepartureInfo(data, isReroute);
            searchProxy->fillAngleAndSpeed(data);
            QList<TeRouteOption> *routeOption = new QList<TeRouteOption>();
            routeOption->append(static_cast<TeRouteOption>(searchProxy->routeOption()));

            data[SearchConstants::SearchType] = CommonConstants::RouteSearch;
            data["RPOptions"] = QVariant::fromValue(QRouteOptionListPtr(routeOption));

            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto oilType = naviProxy->fuelType();
            data[SettingConstants::OilKind] = oilType;
            auto chargeType = naviProxy->chargeType();
            data[SettingConstants::ChargeType] = chargeType;

            auto isPeriodic = data["periodic"].toBool();
            searchProxy->fillGpsTraceData(isPeriodic, data);

            if (isOnline) {
                searchProxy->fillRerouteData(data);
            } else {
                searchProxy->fillLocalRerouteData(data);
            }

            auto notifyMethod = std::bind(&ManageRouteSearchCommand::onRerouteResponseCommand, this, std::placeholders::_1);
            auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;
            searchProxy->requestCommand( req );

            // avoid reroute during requesting
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->setRerouting(true);
            rgProxy->restartPeriodicRerouteTimer();

            sendNotification(UtilConstants::ShowProgressIndicator, QVariantMap{{"reroute", true}});

            int reason = 0;

            auto command = data["Command"].toString();
            auto requestType = data[RPREQ_KEY_RequestType].toInt();
            switch (requestType) {
            case RPRequest::RequestReRouteByAuto:
                reason = OPTIMIZE_ROUTE;
                break;
            case RPRequest::RequestReRouteByBreakaway:
                reason = NO_ROUTE_AVAILABLE;
                break;
            case RPRequest::RequestReRouteForce:
                reason = OPTIMIZE_ROUTE;
                break;
            }

            if (isPeriodic) {
                reason = OPTIMIZE_ROUTE;
            } else if (command == tr("경로 수동 재탐색") || command == tr("경유지 통과") || skipWaypoint) {
                reason = REGARDING_USER_TRAFFIC_MESSAGE;
            }
            QVariantMap m;
            m["type"] = NavigationPropertyProtocol::TmapRouteRecalculationReason;
            m["propertyValue"] = reason;
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
        }
        break;

    case RouteConstants::RequestRouteByProfileChange:
        {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();

            if (rgProxy->isRg()) {
                if (rgProxy->isSimulatorRunning()) {
                    // 모의 주행 일때에도 isRg가 true이기 때문에 별도 조건으로 처리한다.
                    rgProxy->stopSimulator();

                    sendNotification(RouteConstants::CancelRoute);

                    sendNotification(MapConstants::ChangeMapViewMode,
                        QVariantMap{{CommonConstants::Mode,MapViewMode::Normal},
                        {"reason",MapViewMode::CancelRoute}});

                    rgProxy->stopRouteGuidance();
                } else {
                    // 외부에서 이 noti를 날리면 rgProxy->isRg()가 false가 되어
                    // 이 조건문을 타지 않기 때문에 여기서 StopRouteGuidance를 날린다.
                    sendNotification(RgConstants::StopRouteGuidance);

                    // 이전에 주행하던 경로를 재탐색이 아닌 경로시작으로 요청한다.
                    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                    auto option = static_cast<TeRouteOption>(searchProxy->routeOption());
                    QList<TeRouteOption> *options = new QList<TeRouteOption>{ option };

                    QStringList *filenames = new QStringList();
                    filenames->append("tvas_route.db");

                    auto isOnline = searchProxy->isRpServerOnline();

                    QVariantMap m;
                    m[RPREQ_KEY_RequestType] = RPRequest::RequestRoute;
                    m[RPREQ_KEY_TvasFileName] = QVariant::fromValue(QStringListPtr(filenames));
                    m[RPREQ_KEY_RPOptions] = QVariant::fromValue(QRouteOptionListPtr(options));
                    m[RPREQ_KEY_Online] = isOnline;
                    m[RPREQ_KEY_Command] = QObject::tr("경로 요청");
                    m[REREQ_KEY_ByProfileChange] = true;
                    m[SearchConstants::SearchType] = CommonConstants::RouteSearch;

                    auto skipDeparture = true;
                    searchProxy->fillViaInfo(m, skipDeparture);
                    searchProxy->fillDepartureInfo(m);
                    searchProxy->fillAngleAndSpeed(m);
                    searchProxy->fillGpsTraceData(false, m);
                    if (isOnline) {
                        searchProxy->fillRerouteData(m);
                    } else {
                        searchProxy->fillLocalRerouteData(m);
                    }

                    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
                    auto cur = naviProxy->currentPosition();
                    TS_ROUTE_POS *pos = new TS_ROUTE_POS();
                    memset(pos, 0, sizeof(TS_ROUTE_POS));
                    pos->tp.x = cur.x();
                    pos->tp.y = cur.y();
                    QString via = QString("Via%1").arg(1);
                    m.insert(via, QVariant::fromValue(RoutePosPtr(pos)));

                    auto oilType = naviProxy->fuelType();
                    m[SettingConstants::OilKind] = oilType;

                    auto notifyMethod = std::bind(&ManageRouteSearchCommand::onResponseCommand, this, std::placeholders::_1);
                    auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , m) ;
                    searchProxy->requestCommand( req );

                    m.clear();
                    m["type"] = NavigationPropertyProtocol::TmapRouteRecalculationReason;
                    m["propertyValue"] = FIRST_CALCULATION;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
                }
            }
        }
        break;

    case RouteConstants::RequestViaInfo:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->sendViaInfo();
        }
        break;

    case RouteConstants::MoveRouteManagerItem:
        {
            auto m = noti->getBody().toMap();
            auto index = m[CommonConstants::Index].toInt();
            auto offset = m[CommonConstants::Offset].toInt();
            auto mapView = m["mapView"].value<QObject*>();

            Q_ASSERT(offset == 1 || offset == -1);

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            if (0 < offset) {
                searchProxy->moveDown(mapView, index);
            } else {
                searchProxy->moveUp(mapView, index);
            }
            searchProxy->sendWaypointProperties();
            searchProxy->sendWaypointOperationStatus(WaypointOperationStatus::MOVED);
        }
        break;

    case RouteConstants::RemoveRouteManagerItem:
        {
            int index = 0;
            QObject* mapView = nullptr;
            if ( noti->getBody().canConvert(QVariant::Map) ) {
                auto m = noti->getBody().toMap();
                mapView = m["mapView"].value<QObject*>();
                index = m["index"].toInt();
            }
            else {
                index = noti->getBody().toInt();
            }

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->removeVia(mapView, index);
            searchProxy->sendWaypointProperties();
            searchProxy->sendWaypointOperationStatus(WaypointOperationStatus::DELETED);
        }
        break;

    case RouteConstants::ShowViaMarker:
        {
            QObject* mapView = nullptr;
            if ( noti->getBody().canConvert(QVariant::Map) ) {
                auto m = noti->getBody().toMap();
                mapView = m["mapView"].value<QObject*>();
            }

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->showVia(mapView);
        }
        break;

    case RouteConstants::ClearViaMarker:
        {
            QObject* mapView = nullptr;
            if ( noti->getBody().canConvert(QVariant::Map) ) {
                auto m = noti->getBody().toMap();
                mapView = m["mapView"].value<QObject*>();
            }
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->hideVia(mapView);
        }
        break;

    case RouteConstants::BackupViaData:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->backupVia();
        }
        break;

    case RouteConstants::RestoreViaData:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->restoreVia();
            searchProxy->sendWaypointProperties();
        }
        break;

    case RouteConstants::PrepareRouteCandidate:
        prepareRouteCandidate();
        break;

    case RouteConstants::SaveVia:
        saveVia();
        break;

    case RgConstants::ChangeRouteOption:
        changeRouteOption(noti->getBody());
        break;

    case SearchConstants::RequestRouteOption:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            sendNotification(SearchConstants::ResponseRouteOption, searchProxy->routeOption());
        }
        break;

    case RouteConstants::ClearVia:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->clearVia();
        }
        break;

    case RouteConstants::CheckViaValidate:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            auto result = searchProxy->checkViaIsNear();

            if (CheckViaResultOk == result) {
                QVariantMap m;
                m["arrivalPos"] = searchProxy->viaWorldPos(RouteItem_End);
                sendNotification(RouteConstants::CheckViaValidateIsOk, m);
            } else {
                QString errorMessage;
                switch (result) {
                case CheckViaResultFail_DepartuerAndVia: //errorMessage = tr("출발지와 경유지가 동일합니다."); break;
                case CheckViaResultFail_DepartureAndArrival: //errorMessage = tr("출발지와 목적지가 동일합니다."); break;
                case CheckViaResultFail_All: //errorMessage = tr("출발지, 경유지, 목적지가 모두 동일합니다.");break;
                case CheckViaResultFail_ViaAndArrival: //errorMessage = tr("경유지와 목적지가 동일합니다.");break;
                case CheckViaResultFail_Via: //errorMessage = tr("서로 동일한 경유지가 있습니다.");break;
                    errorMessage = tr("선택한 위치와 동일한 지점이\n등록되어 있습니다.\n다시 확인해 주세요.");
                    break;
                }
                QVariantMap m;
                m["errorMessage"] = errorMessage;
                sendNotification(RouteConstants::CheckViaValidateIsFail, m);
            }
        }
        break;

    case RouteConstants::SetHipass:
        {
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            auto isHipass = noti->getBody().toBool();
            searchProxy->setHipass(isHipass);
        }
        break;

    case MapConstants::MoveMapByUser:
        {
            // 잘못 된 시나리오. last user select position은 interface에서 전달 됨. 19.08.09 ymhong
#if (0)
            auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
            auto pos = mapProxy->centerPosition(nullptr);

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            QString address;
            searchProxy->setLastUserSelectPosition(pos.x(), pos.y(), address);
#endif
        }
        break;

    case RouteConstants::HitSearchMarker:
        {
            // 잘못 된 시나리오. last user select position은 interface에서 전달 됨. 19.08.09 ymhong
#if (0)
            auto m = noti->getBody().toMap();
            auto x = m["wX"].toInt();
            auto y = m["wY"].toInt();
            auto address = m["name"].toString();
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->setLastUserSelectPosition(x, y, address);
#endif
        }
        break;

    case RouteConstants::PrimaryNddsServerUpdated:
        {
            auto primaryNddsDomain = noti->getBody().toString();
            auto routeSearchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            routeSearchProxy->setNddsPrimaryDomain(primaryNddsDomain);
        }
        break;

    default:
        return false;
    }

    return true;
}

bool ManageRouteSearchCommand::onResponseCommand( const mvc::INotificationPtr& note )
{
    auto req = note->getBody().value<RequestCommandPtr>();
    QVariantMap reqCommand = req->getReqData();
    QVariantMap resCommand = req->getResData();

    QString command = reqCommand["Command"].toString();
    QString answer = resCommand["Answer"].toString();

    if(answer == "Done"){
        auto reqType = reqCommand[RPREQ_KEY_RequestType];
        auto reqByPos = reqCommand.contains("continuously");
        auto validCommand =
                command.compare(tr("경로 요청")) == 0 ||
                command.compare("ChangeOption") == 0;
        auto sessionId = resCommand[RPREQ_KEY_SessionId].toString();
        qDebug() << "ManageRouteSearchCommand::onResponseCommand(sessionId: " << sessionId << ")";
        sendNotification(ApplicationConstants::UpdateLastSesstionId, sessionId);

        //RP 결과 디버그 로그 테스트.
        //RouteSearchResultHelper::dumpResponseCommand(reqCommand, resCommand);

        if(!reqByPos && validCommand){
            auto errorCode = resCommand[RPREQ_KEY_ErrorCode].toInt();

            if (errorCode != E_ERROR_SUCCESS) {
                sendNotification(RouteConstants::RequestRouteFailed);

                RpErrorHelper::exec(this, errorCode);
            } else {
                auto byProfileChange = reqCommand[REREQ_KEY_ByProfileChange].toBool();
                auto byApply = reqCommand[RPREQ_KEY_Apply].toBool();
                auto sessionId = resCommand[RPREQ_KEY_SessionId].toString();
                auto rpProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                rpProxy->setSessionId(sessionId);

                if (resCommand.contains(RPREQ_KEY_SwitchToLocal)) {
                    auto switchToLocal = resCommand[RPREQ_KEY_SwitchToLocal].toBool();
                    if (switchToLocal) {
                        sendNotification(RgConstants::SwitchToLocal);
                    }
                }

#ifdef QT_DEBUG
#if (0) // 디버깅 모드에서 경로 안내 중 프로파일 전환이 제대로 되는지 확인할 때 사용.
                if (byProfileChange) {
                    QVariantMap m;
                    m[CommonConstants::Position] = ToastsConstants::ToastUp;
                    m[CommonConstants::Message] = tr("사용자 전환 경로 탐색 완료.");
                    sendNotification(ToastsConstants::ShowToast, m);
                }
#endif
#endif
                if (command == "ChangeOption" || byProfileChange || byApply) {
                    QVariantList list;
                    auto options = reqCommand["RPOptions"].value<QRouteOptionListPtr>();
                    auto tvas = reqCommand["TvasFileName"].value<QStringListPtr>();
                    auto option = options->at(0);
                    auto filename = tvas->at(0);
                    list << option << filename;
                    sendNotification(RouteConstants::SetTvas, list);

                    QVariantMap m;
                    m["routeId"] = 0;
                    m["routeOption"] = option;
                    m[CommonConstants::Command] = command;
                    sendNotification(RgConstants::StartRouteGuidance, m);

                    // TMCRSM-4007
                    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
                    auto summary = naviProxy->summary(0).value<RouteSummaryVoPtr>();

                    int distance = 0;
                    int fee = 0;
                    int remainTime = 0;
                    if (summary != nullptr) {
                        distance = summary->distance();
                        fee = summary->fee();
                        remainTime = summary->remainTime();
                    }

                    m.clear();
                    m["distance"] = distance;
                    m["fee"] = fee;
                    m["remainTime"] = remainTime;
                    sendNotification(RouteConstants::UpdateRouteSummaryByReroute, m);

                    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
                    rgProxy->setTotalDistance(distance);

                    naviProxy->cacheSummary(m);
                } else {
                    QVariantMap m;
                    m["resCommand"] = resCommand;
                    m["reqCommand"] = reqCommand;
                    sendNotification(RouteConstants::UpdateRouteCandidate, m);//resCommand);
                }

                auto duration = resCommand["elapsed"].toInt();
                QVariantMap m;
                m["type"] = NavigationPropertyProtocol::TmapRouteCalculateDuration;
                m["propertyValue"] = duration;
                sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
            }
        }
        if (!reqType.isNull()) {
            auto type = reqType.toInt();

            switch (type) {
            case RPRequest::RequestRouteSummary:
                {
                    auto errorCode = resCommand[RPREQ_KEY_ErrorCode].toInt();
                    if (errorCode == E_ERROR_SUCCESS) {
                        auto summary = resCommand["RouteSummaryInfo"].value<QRouteSummaryInfo>();
                        QVariantMap m;
                        auto via5 = reqCommand["Via5"].value<RoutePosPtr>();
                        if (via5->tp.x > 0 && via5->tp.y > 0) {
                            reqCommand["x"] = via5->tp.x;
                            reqCommand["y"] = via5->tp.y;
                        }
                        m["reqData"] = QVariant::fromValue(reqCommand);
                        m["dist"] = summary->nTotalDist;
                        m["time"] = summary->nTotalTime;
                        sendNotification(RouteConstants::UpdateSummary, m);
                    }
                }
                break;
            }
        }
    }

    if (command == "ChangeOption") {
        sendNotification(UtilConstants::HideProgressIndicator);
    }

    ///////////////////////////////////////////////////////////////////////
    //[MonkeyTest]를 위하여
    QVariantMap m;
    m.insert(CommonConstants::Name,    CommonConstants::RouteSearch);
    m.insert(CommonConstants::Value,   false);
    m.insert(CommonConstants::Type,    0);
    m.insert(CommonConstants::Keyword, CommonConstants::Empty);
    sendNotification(SearchConstants::SearchWait, m);
    //[MonkeyTest]를 위하여
    ///////////////////////////////////////////////////////////////////////

    sendRequestCommandResponsed(reqCommand);

    // below ==> cause the issue TMCRSM-2385 which stay in BUSY status under IDLE, 2019.9.9 by lckxx
    // Don't update status 4(ANDO), but take status 6(calculating)
//    auto requestType = reqCommand[RPREQ_KEY_RequestType].toInt();
//    if (requestType == RPRequest::RequestRoute || requestType == RPRequest::RequestRouteSummary) {
//        m.clear();
//        m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
//        m["propertyValue"] = TMAPNAVISTATUS_ANDO;
//        qDebug() << "ui, routesearch res  navstat= 4";
//        sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
//    }

    // lckxx fixd cause the issue TMCRSM-2385 which stay in BUSY status under IDLE, 2019.9.9
    int requestType = reqCommand[RPREQ_KEY_RequestType].toInt();
    if (requestType == RPRequest::RequestRoute
        || requestType == RPRequest::RequestRouteSummary
        || requestType == RPRequest::RequestVMSInfo) {
        if (!Facade::instance()->retrieveProxy<RouteGuidanceProxy>()->isRg()) {
            QVariantMap m;
            m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
            m["propertyValue"] = TMAPNAVISTATUS_ANDO;
            qDebug() << "navi, routeSearchcommand navstat -> ando";
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
        }
    }

    return true;
}

bool ManageRouteSearchCommand::onRerouteResponseCommand( const mvc::INotificationPtr& note )
{
    auto req = note->getBody().value<RequestCommandPtr>();
    QVariantMap reqCommand = req->getReqData();
    QVariantMap resCommand = req->getResData();


    QString command = reqCommand["Command"].toString();

    QString answer = resCommand["Answer"].toString();
    if(answer == "Done"){
        auto naviControlProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
        auto isOnline = naviControlProxy->getOnlineState() && naviControlProxy->isChannelServerPolicyFinished();
        auto isLocal = !isOnline;
        auto routeOption = searchProxy->routeOption();
        auto byForce = reqCommand["RequestType"] == RPRequest::RequestReRouteForce;
        auto isPeriodic = reqCommand["periodic"].toBool();
        auto destSearchCode = reqCommand[RPREQ_KEY_DestSearchCode].toInt();
        auto explorerReroutePeriodInsideDes = destSearchCode == RouteConstants::EXPLORER_REROUTE_PERIOD_INSIDE_DES;
        auto sessionId = resCommand[RPREQ_KEY_SessionId].toString();
        searchProxy->setSessionId(sessionId);
        qDebug() << "ManageRouteSearchCommand::onResponseCommand(sessionId: " << sessionId << ")";
        sendNotification(ApplicationConstants::UpdateLastSesstionId, sessionId);

        if (explorerReroutePeriodInsideDes && isPeriodic) {
            // 정주기 재탐색 가능 구간이 아닐 때 호출인 경우
            // 호출만 하고 실제 설정은 하지 않는다.
            qDebug() << "onRerouteResponseCommand : EXPLORER_REROUTE_PERIOD_INSIDE_DES";
        } else {
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            if (naviProxy->setTvasDataRerouted(isLocal, routeOption)) {
                auto mapView = reqCommand["mapView"].value<QObject*>();
                naviProxy->applySelectRoute(mapView, 0);
                naviProxy->setDrawGasStationInfo(mapView);

                auto summary = naviProxy->summary(0).value<RouteSummaryVoPtr>();

                int distance = 0;
                int fee = 0;
                int remainTime = 0;
                if (summary != nullptr) {
                    distance = summary->distance();
                    fee = summary->fee();
                    remainTime = summary->remainTime();
                }

                QVariantMap m;
                m["distance"] = distance;
                m["fee"] = fee;
                m["remainTime"] = remainTime;
                sendNotification(RouteConstants::UpdateRouteSummaryByReroute, m);

                auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
                rgProxy->setTotalDistance(distance);

                naviProxy->cacheSummary(m);
            }

            if (isPeriodic) {
                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                auto option = searchProxy->routeOption();
                searchProxy->showRerouteToast(option);

                QVariantMap m;
                m.insert( QStringLiteral("soundPlayScenario"), VSM_VoiceScenarioIndex::REROUTETPEG);
                sendNotification( SoundConstants::PlaySound, m );
            } else {
                auto command = reqCommand[SearchConstants::Command].toString();
                if (command == tr("경로 수동 재탐색")) {
                    QVariantMap m;
                    m.insert( QStringLiteral("soundPlayScenario"), VSM_VoiceScenarioIndex::REROUTEOUTROUTE);
                    sendNotification( SoundConstants::PlaySound, m );
                }
            }
        }
    }

    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
    rgProxy->setRerouting(false);

    sendNotification(UtilConstants::HideProgressIndicator);

    sendRequestCommandResponsed(reqCommand);

#ifdef QT_DEBUG
    if (resCommand.contains("bufferSize")) {
        auto bufferSize = resCommand["bufferSize"].toInt();
        sendNotification(ZoneConstants::PrintBuffersize, bufferSize);
    }
#endif

    QVariantMap m;
    m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
    m["propertyValue"] = TMAPNAVISTATUS_PLAYROUTE;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);

    return true;
}

void ManageRouteSearchCommand::prepareRouteCandidate()
{
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    auto byVr = searchProxy->byVr();

    if (!byVr) {
        // 일반적인 경로 안내.
        QVariantMap m {{CommonConstants::Section, SettingConstants::RouteAlternateSelection}};

        sendNotification(SettingConstants::RequestSetting, m);
    } else {
        auto option = searchProxy->routeOption();

        QVariantMap m;
        m["routeOption"] = option;
        m["byVr"] = true;
        m["apply"] = searchProxy->applyTimer();
        sendNotification(RouteConstants::RequestRoute, m);

        searchProxy->setByVr(false);
    }
}

void ManageRouteSearchCommand::saveVia()
{
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
    searchProxy->saveViaToPreference();
}

void ManageRouteSearchCommand::changeRouteOption(const QVariant &data)
{
    auto option = data.toInt();
    auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();

    //TMCRSM-2798, UID v1.97
    if (searchProxy->isRpServerOnline() == false) {
        if (!searchProxy->isOfflineOption(option)) {
            QVariantMap m;
            m[CommonConstants::Position] = ToastsConstants::ToastUp;
            m[CommonConstants::Message] = "네트워크가 연결되어야 탐색 가능한 경로입니다.\n최적길, 최단거리, 무료도로 경로를 이용해 주세요.";
            sendNotification(ToastsConstants::ShowToast, m);
            return;
        }
    }

    sendNotification(RgConstants::BlockReroute, true);

    sendNotification(ZoneConstants::SelectSoftkeyNormalDriveMode);
    sendNotification(RouteConstants::CancelRoute );

    searchProxy->rerouteByOption(option);

    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
    rgProxy->restartPeriodicRerouteTimer();

    // 경로 안내중 rp option 변경 시 이를 저장.
    // 재시작 이어하기 할 때 경로 옵션이 올바로 표시 되도록.
    // 18.04.16 ymhong
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Rg},
         {CommonConstants::Section,QStringLiteral("Termination")},
         {CommonConstants::Key,QStringLiteral("selectedRouteOption")},
         {CommonConstants::Value,option}
                     });
}

void ManageRouteSearchCommand::sendRequestCommandResponsed(const QVariantMap &reqCommand)
{
    auto reqType = reqCommand["RequestType"].toInt();

    switch (reqType) {
    case RPRequest::RequestRouteSummary:
        break;
    case RPRequest::RequestCancel:
    case RPRequest::RequestRoute:
    case RPRequest::RequestReRouteForce:
    case RPRequest::RequestReRouteByAuto:
    case RPRequest::RequestReRouteByBreakaway:
        sendNotification(RouteConstants::RequestCommandResponsed, reqCommand);
        auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        rgProxy->recreateTbtList();
        break;
    }
}
