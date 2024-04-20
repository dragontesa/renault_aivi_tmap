#include "ManageRouteGuidanceCommand.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "NavibookmarkConstants.h"
#include "RouteGuidanceProxy.h"
#include "VsmNaviProxy.h"
#include "mediator/app/MapViewMode.h"
#include "PreferenceConstants.h"
#include "RouteSearchProxy.h"
#include "ViewRegistry.h"
#include "NavibookmarkProtocol.h"
#include "mediator/map/SimulationViewMediator.h"
#include "SoundConstants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "navigationdata.h"
#include "ApplicationConstants.h"
#include "NavigationControllerProxy.h"

using namespace SKT;

ManageRouteGuidanceCommand::ManageRouteGuidanceCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

bool ManageRouteGuidanceCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case RgConstants::StartRouteGuidance:
        {
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto m = noti->getBody().toMap();

            auto routeIdKey = QStringLiteral("routeId");
            auto routeOptionKey = QStringLiteral("routeOption");

            if (m.contains(routeIdKey)) { // 안내 시작.
                auto routeId = m[routeIdKey].toInt();
                auto routeOption = m[routeOptionKey].toInt();

                // 경로 안내 전 SetTvas()에 tvas_route.db 하나만 설정하기 때문에
                // 경로 안내 시 routeId는 무조건 0이다.
                // 2018.01.17 ymhong
                routeId = 0;

                auto mapView = m["mapView"].value<QObject*>();
                proxy->applySelectRoute(mapView, routeId);
                proxy->setDrawGasStationInfo(mapView);
                proxy->blockReroute(false);

                sendNotification(Constants::CloseView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::NaviMenuView)}});
                sendNotification(Constants::CloseView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::SimulationView)}});
                sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::Navi}});
                sendNotification(ZoneConstants::SelectZone1TextInput);
                sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
                //sendNotification(MapConstants::UpdateMapViewPadding);

                auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
                rgProxy->startRouteGuidance();

                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                searchProxy->updatePoi();
                searchProxy->setRouteOption(routeOption);
                searchProxy->saveViaToPreference();

                // 경로 이어하기를 위한 플래그 저장.
                sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                     {"preferencesName",PreferenceConstants::Rg},
                     {CommonConstants::Section,QStringLiteral("Termination")},
                     {CommonConstants::Key,QStringLiteral("isNormallyTerminated")},
                     {CommonConstants::Value,false}
                 });

                // UID v2.01 변경으로 더 이상 경로 안내 시 최근 목적지를 등록하지 않는다.
                // 경로 탐색이 성공한 시점에 최근 목적지를 등록.
                // 최근 목적지 등록은 RgConstants::RegistRecentDestination 에서 처리.
                // 19.04.11 ymhong

                QVariantMap m;
                m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                m["propertyValue"] = TMAPNAVISTATUS_PLAYROUTE;
                sendNotification(NavigationPropertyConstants::setNavigationProperty, m);

                playSoundTmapStartRouteGuidance();
            }
        }
        break;

    case RgConstants::RegistRecentDestination:
        {
            QVariantMap bookmark;

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            auto arrivalItem = searchProxy->targetItem();

            // fixed artf258897,  it needs to select address if it is called from map search 2017.7.7
            auto pos = arrivalItem.toPoint();
            bookmark[CommonConstants::Name] = arrivalItem.name;
            bookmark[CommonConstants::Address] = arrivalItem.address; // fixed TMCRSM-198, changed from Address to name of POI,  because of that searchProxy can get address or name of a POI and this override poi address or name of one selected from search result list , by lckxx 2017.12.29
            bookmark[CommonConstants::X] = pos.x();
            bookmark[CommonConstants::Y] = pos.y();
            // TODO set poiID and navSeq into that bookmark
            bookmark[SearchConstants::PoiID] = arrivalItem.poiId;
            bookmark[SearchConstants::NavSeq] = arrivalItem.navSeq;
            bookmark[SearchConstants::RpFlag] = arrivalItem.rpFlag;
            bookmark[SearchConstants::OpiPoleCate] = arrivalItem.poleCate;
            bookmark[SearchConstants::OpiPrice] = 0;

            sendNotification(NavibookmarkConstants::SelectNavibookAddress, bookmark);

            bookmark.insert(CommonConstants::BookmarkType,(int)NavibookmarkProtocol::BookmarkRecentDestination);
            sendNotification(NavibookmarkConstants::RegistNavibookBookmark,bookmark);
        }
        break;

    case RgConstants::StartRouteGuidanceContinuously:
        {
            auto m = noti->getBody().toMap();
            auto via = m["via"].toStringList();

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            auto skipDeparture = true;
            searchProxy->restoreViaFromPreference(via, skipDeparture);
            searchProxy->setRpContinue(true);

            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteCandidateView);
            sendNotification(Constants::CreateView, m);
        }
        break;

    case RgConstants::StartRouteGuidanceByVr:
        {
            // showCalculateTour(POI Push)에 의해 requestRoute()가 호출 된 경우
            // 출발지, 경유지, 목적지는 이미 셋팅된 상태
            // 아래 코드는 단 하나의 경로 옵션을 사용할 때 사용 되는 코드
            // 티맵에서는 경로 요약 화면에서 한개의 경로만 표출하는 것은 에러이므로
            // 인터페이스로 전달 되는 하나의 옵션은 무시하고
            // 통신, 비통신에 따라 해당하는 옵션들이 선택 되도록 한다.
            // 19.06.21 ymhong
#if (0)
            auto m = noti->getBody().toMap();
            auto option = m[QStringLiteral("routeOption")].toInt();
            auto apply = m[QStringLiteral("apply")].toBool();

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();

            searchProxy->setRouteOption(option);
            searchProxy->setByVr(true);
            searchProxy->setApplyTimer(apply);
#endif

            sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, ViewRegistry::url(ViewRegistry::RouteCandidateView)}});
        }
        break;

    case RgConstants::StopRouteGuidance:
        {
            // 경로 취소 직전
            // 재탐색이 요청 된 경우 취소하도록 커맨드를 보낸다.
            // 19.04.25 ymhong
            QVariantMap m;
            m.insert(RPREQ_KEY_RequestType, RPRequest::RequestCancel);
            sendNotification(SearchConstants::RequestCommand, m);

            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            if (rgProxy->isRg()) {
                auto m = noti->getBody().toMap();
                auto isGoal = m["goal"].toBool();
                auto isUserCancel = m["userCancel"].toBool();
                auto isClearVia = m["clearVia"].toBool();
                auto routeSearchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();

                if (isGoal || isUserCancel) {
                    if (isGoal) {
                        // 목적지 도착에 의한 종료
                        playSound(VSM_VoiceScenarioIndex::RGEND);// 경로안내를 종료 안내 (IDG_RG_END)

                        // 지난 위치 정보 저장(for NDDS)
                        auto sessionId = routeSearchProxy->sessionId();
                        if (sessionId.length() > 0) {
                            auto currentTime = rgProxy->currentTime();
                            routeSearchProxy->saveLastGoalData(sessionId, currentTime);
                        }
                    } else if (isUserCancel) {
                        // 사용자에 의해 종료
                        playSound(VSM_VoiceScenarioIndex::ROUTECANCEL); // 경로취소 안내 (IDG_CANCEL_ROUTE)

                        routeSearchProxy->resetLastGoalData();
                    }
                    sendNotification( RouteConstants::CancelRoute, QVariantMap{{"clear",true}} );
                } else {
                    // Case1. Profile 변경.
                    sendNotification( RouteConstants::CancelRoute);

                    routeSearchProxy->resetLastGoalData();
                }

                sendNotification(MapConstants::ChangeMapViewMode,
                    QVariantMap{{CommonConstants::Mode,MapViewMode::Normal},
                    {"reason",MapViewMode::CancelRoute}});

                if (isGoal || isUserCancel || isClearVia) {
                    // via 데이터 삭제
                    sendNotification(RouteConstants::ClearVia);
                    sendNotification(RouteConstants::SaveVia);

                    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                         {"preferencesName",PreferenceConstants::Rg},
                         {CommonConstants::Section,QStringLiteral("Termination")},
                         {CommonConstants::Key,QStringLiteral("isNormallyTerminated")},
                         {CommonConstants::Value,true}
                     });
                }

                rgProxy->stopRouteGuidance();

                m.clear();
                m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                m["propertyValue"] = TMAPNAVISTATUS_ANDO;
                sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
            }
        }
        break;

    case RgConstants::TerminatedRouteGuidance:
        {
            // 17.5.30 hskim 시뮬레이션 도착 처리 스킵 임시 처리
            auto mediator = Facade::instance()->retrieveMediator<SimulationViewMediator>();
            if ( mediator.isNull() ) {
                QVariantMap m;
                m["goal"] = true;
                sendNotification(RgConstants::StopRouteGuidance, m);
            }
        }
        break;        
    case RgConstants::RequestRouteGuidanceState:
        {
            auto observerId = noti->getBody();
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            QVariantMap m;
            m.insert( QStringLiteral("isRg"), rgProxy->isRg() );
            sendNotification( RgConstants::ReplyToRouteGuidanceState, m, observerId );
            T_DEBUG( "RgConstants::RequestRouteGuidanceState #1" );
        }
        break;

    case RgConstants::UpdateRouteGuidanceState:
        {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            auto isRg = rgProxy->isRg();
            auto naviControllerProxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            naviControllerProxy->setIsRg(isRg);
        }
        break;

    case ZoneConstants::UpdateSoftkeyRouteGuide:
        {
            auto m = noti->getBody().toMap();
            auto remainDistance = m["remainDistance"].toInt();
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            proxy->cacheRemainDistance(remainDistance);
        }
        break;

    case RgConstants::RequestTrafficGuidance:
        {
            // 주행 중 widget에 traffic정보를 채우기 위해 호출
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            QVariantMap m;
            proxy->fillCachedSummary(m);

            m["traffic"] = proxy->trafficGuidance();

            sendNotification(RgConstants::UpdateTrafficGuidance, m);
        }
        break;

    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = noti->getBody().value<RgDataVo>();
            auto pos = vo.stCurrent_position();
            auto address = vo.stCurrent_szPosAddrName();

            auto routeSearchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            routeSearchProxy->setCurrentPosition(pos.x(), pos.y());
            routeSearchProxy->setCurrentAddress(address);
        }
        break;

    case RgConstants::RequestGetGasStationInfo:
        {
            // 주행 중에 주유소/충전소 정보를 채우기 위해 호출
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            QVariantMap m;
            m["info"] = naviProxy->getGasStationInfo();
            sendNotification(RgConstants::ResponseGetGasStationInfo, m);
        }
        break;
    case RgConstants::RequestGetEStationInfo:
        {
            // 주행 중에 전기차 정보를 채우기 위해 호출
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            QVariantMap m = noti->getBody().toMap();
            m["info"] = naviProxy->getEStationInfo();
            sendNotification(RgConstants::ResponseGetEStationInfo, m);
        }
        break;


    case RgConstants::RequestIsSoundPlaying:
        {
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto isSoundPlaying = naviProxy->isSoundPlaying();
            sendNotification(RgConstants::ReplyToIsSoundPlaying, isSoundPlaying);
        }
        break;

    case RgConstants::RequestCurrentSpeed:
        {
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            auto speed = naviProxy->currentSpeed();
            sendNotification(RgConstants::ReplyToCurrentSpeed, speed);
        }
        break;

    case RgConstants::EnableSimulatorStopBySpeed:
        {
            auto enable = noti->getBody().toBool();

            // 경로 요약 화면에서 체크
            auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
            naviProxy->enableSimulatorStopBySpeed(enable);

            // 모의 주행 화면에서 체크
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->enableSimulatorStopBySpeed(enable);
        }
        break;

    case RgConstants::ForcePeriodicReroute:
        {
#ifdef QT_DEBUG
            // 디버깅 모드에서 Qt::Key_P를 눌러 정주기 재탐색 요청.
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            rgProxy->forcePeriodicRerouteForDebugging();
#endif
        }
        break;

    case RgConstants::ChangeRouteGuideBindState: {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            int bindState = noti->getBody().toInt();
            switch (bindState) {
            case 0:
                rgProxy->setBindState(RouteGuidanceProxy::BIND_STATE_NONE);
                break;
            case 1:
                rgProxy->setBindState(RouteGuidanceProxy::BIND_STATE_GPS);
                break;
            }
        }
        break;

    case RgConstants::BlockReroute: {
        auto enable = noti->getBody().toBool();

        auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
        naviProxy->blockReroute(enable);

        auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
        rgProxy->blockReroute(enable);
        break;
    }//BlockReroute

    case RgConstants::RequestMovingDistance:
        {
            auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            auto distance = rgProxy->movingDistance();
            sendNotification(RgConstants::ReplyToMovingDistance, distance);
        }
        break;

    default:
        return false;
    }

    return true;
}

void ManageRouteGuidanceCommand::playSound(int voiceId)
{
    sendNotification(SoundConstants::PlaySound, QVariantMap{{QStringLiteral("soundPlayScenario"), voiceId}});
}

void ManageRouteGuidanceCommand::playSoundTmapStartRouteGuidance()
{
    playSound(VSM_VoiceScenarioIndex::RGSTARTTMAPNAVI);
}
