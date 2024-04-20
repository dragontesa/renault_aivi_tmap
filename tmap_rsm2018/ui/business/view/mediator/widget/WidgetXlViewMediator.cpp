#include "WidgetXlViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "ZoneConstants.h"
#include "WidgetConstants.h"
#include "ViewRegistry.h"
#include "WidgetUtils.h"
#include "NavigationControllerConstants.h"
#include "mediator/rg/RgUtils.h"
#include "ApplicationConstants.h"
#include "satellitedataptr.h"
#include "MapConstants.h"
#include "VsmMapSettingAndTheme.h"
#include "CommonConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "VSM_Enum.h"

#include "TestConstants.h"
#include "DebugUtils.h"
#include "SettingProxy.h"
#include "VSM_Map.h"
#include "QmlAppHelper.h"
#include "RouteConstants.h"
#include "RgDataVo.h"
#include "VSM_Navi.h"

using namespace SKT;

WidgetXlViewMediator::WidgetXlViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

WidgetXlViewMediator::~WidgetXlViewMediator()
{
}

QList<int> WidgetXlViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        ZoneConstants::UpdateSoftkeyRouteGuide,
        ZoneConstants::UpdateSoftkeyAddress,
        RgConstants::UpdateTbtState,
        RgConstants::UpdateSdiState,
        RgConstants::UpdateLaneState,
        RgConstants::UpdateTrafficGuidance,
        RouteConstants::UpdateRouteSummaryByReroute,
        RouteConstants::SelectRoute,
        NavigationControllerConstants::ChangeTimeFormat,
        RgConstants::UpdateGpsTime,
        MapConstants::RespondToGlobalMapViewStatus,
        MapConstants::GlobalMapViewIsNaviOrAndoChanged,
        MapConstants::GlobalMapViewIsOnlineChanged,
        MapConstants::GlobalMapViewShowTrafficInfoChanged,
        MapConstants::GlobalMapViewEnableTrafficInfoChanged,
        MapConstants::GlobalMapViewShowTrafficInfoOnMapChanged,
        MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineChanged,
        MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineForceChanged,
        MapConstants::GlobalMapViewViewLevelChanged,
        MapConstants::GlobalMapViewViewModeChanged,
        MapConstants::GlobalMapViewNightModeChanged,
        MapConstants::GlobalMapViewSkyStateChanged,
        MapConstants::GlobalMapViewMoveModeChanged,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        TestConstants::UpdateMapTheme,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ShowGadget,
        ApplicationConstants::ReplyToGadgetState,
        ApplicationConstants::ChangedGadgetState,
        ApplicationConstants::FocusToWidget,
        NavigationControllerConstants::ResponseSkyState,
        NavigationControllerConstants::UpdateSkyState
    };

    return notes;
}

bool WidgetXlViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            mRgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
//            mRenderer = findChild( "renderer" );
//            QVariantMap m;
//            m["mapView"] = QVariant::fromValue( mRenderer );
//            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);

//            m["type"] = QVariant::fromValue( (int)VsmMapType::Global );
//            sendNotification(MapConstants::ApplyMapSetting, m);
//            sendNotification(MapConstants::ApplyMapTheme, m);

            sendNotification(MapConstants::RequestGlobalMapViewStatus);
//            sendNotification(NavigationControllerConstants::UpdateCavatarIcon,
//                 QVariantMap{
//                     {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )}
//                 });

#ifdef BUILD_TARGET
            if (QQmlProperty::read(mViewComponent, "asView").toBool() == false) {
                //QQmlProperty::write(mViewComponent, "verticalFlip", true);
            }
#endif
            QJsonObject root;
            QJsonObject carvatar;
            carvatar["width"] = 70.0f;
            root["carvatar"] = carvatar;
            auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

//            auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));
//            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_DAY, json.data());
//            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_NIGHT, json.data());

//            DebugUtils::updateMapTheme(TestConstants::GXL, mRenderer, TestConstants::GXL_CS_C);
//            DebugUtils::updateMapTheme(TestConstants::GXL, mRenderer, TestConstants::GXL_RLT);
//            DebugUtils::updateMapTheme(TestConstants::GXL, mRenderer, TestConstants::GXL_RLTT);

            updateLanguage();

            // GPS speed display
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                { CommonConstants::Section, SettingConstants::MapMain },
                { CommonConstants::Key, SettingConstants::GpsSpeed }
            });

            // Sky state for trafficbar, lck 2019-10-07
            sendNotification( NavigationControllerConstants::RequestSkyUpdate );
        }
        break;
    case Activated: {
            mIsActive = true;

            sendNotification(ApplicationConstants::RequestGadgetState);
            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            sendNotification( RgConstants::RequestTbtStateUpdate );
            sendNotification( RgConstants::RequestSdiStateUpdate );
            sendNotification( RgConstants::RequestLaneStateUpdate );
            sendNotification(RgConstants::RequestTrafficGuidance);            

            updateLanguage();
        }
        break;
    case Deactivated: {
            mIsActive = false;
        }
        break;


    case SearchClicked:
        sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, ViewRegistry::url(ViewRegistry::SearchTabView)}});
        QMetaObject::invokeMethod(mViewComponent, "close");

        sendNotification(ZoneConstants::FocusActivatedZone1TextInput, data);
        break;

    case RendererOnDestruction:
        {
            sendNotification(MapConstants::DetachMapViewFromVsmNavi,data);
        }
        break;

    default:
        return false;
    }

    return true;
}

bool WidgetXlViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto m = note->getBody().toMap();
                auto isRg = m["isRg"].toBool();
                QMetaObject::invokeMethod(mViewComponent, "changeRgComponent", Q_ARG(QVariant, isRg));

                m.clear();

                if ( mIsGadgetVisible && mIsActive ) {
                    const int gadgetWidth = 598;
                    const int trafficBarWidth = 42;
                    const int gpsIconWidth = 69;
                    const int gpsIconRightMargin = 10;
                    if ( isRg ) {
                        m["x"] = gadgetWidth - trafficBarWidth - gpsIconWidth;
                        sendNotification(MapConstants::ChangeGpsStatePosition, m);
                    }
                    else {
                        m["x"] = gadgetWidth - gpsIconWidth - gpsIconRightMargin;
                        sendNotification(MapConstants::ChangeGpsStatePosition, m);
                    }
                }
            }
        }
        break;

    case RgConstants::UpdateTbtState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleTbtUpdate( WidgetConstants::SizeXL, mViewComponent, note );
            }
        }
        break;

    case RgConstants::UpdateSdiState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleSdiUpdate( WidgetConstants::SizeXL, mViewComponent, note );
            }
        }
        break;

    case RgConstants::UpdateLaneState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleLaneUpdate( WidgetConstants::SizeXL, mViewComponent, note );
            }
        }
        break;

    case RgConstants::UpdateTrafficGuidance:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto m = note->getBody().toMap();
                auto distance = m["distance"].toInt();
                auto remain   = m["remainDistance"].toInt();// 2019-12-19 fixed TMCRSM-4406, TMCRSM-4282
                auto traffic  = m["traffic"].toList();

                QQmlProperty::write(mViewComponent, "totalDistance", distance);
                if (!mIsTrafficInit && mRgProxy->isRg()) {
                    mIsTrafficInit = true;
                    if (traffic.count()) {
                        QQmlProperty::write(mViewComponent, "trafficGuideList", traffic);
                    }
                }
                // 교통정보를 업데이트 할 때는 남은 거리를 전체 경로로 리셋하므로, 마지막에 업데이트 한다.
                QQmlProperty::write(mViewComponent, "leftDistance", remain);
            }
        }
        break;
    // 20191002. lckxx fix TMCRSM-3897, 가젯에서 교통정보바를 컴포넌트로 사용하므로, 직접 RG,경로재탐색 업데이트를 받아준다
    case RgConstants::UpdateRouteGuidance: {
        if ( !mRgProxy->isSimulatorRunning() ) {
            auto rg = note->getBody().value<RgDataVo>();
            double distance = rg.nTotalRemainDist();
            QQmlProperty::write(mViewComponent, "leftDistance", distance);
        }
    }
        break;
    case RouteConstants::SelectRoute:
    case RouteConstants::UpdateRouteSummaryByReroute: {
        if ( !mRgProxy->isSimulatorRunning() ) {
            //  2019-11-14 fix, TMCRSM-3986
            auto m = note->getBody().toMap();
            auto distance = m["distance"].toInt();
            int routeId   = m["routeId"].toInt();
            QQmlProperty::write(mViewComponent, "totalDistance", distance);
            updateTrafficGuidance(routeId);
    //        sendNotification(RgConstants::RequestTrafficGuidance); // fix TMCRSM-3986, donot match trafficbar on main view that why is proxy tell only first route
        }
    }
        break;

    case ZoneConstants::UpdateSoftkeyRouteGuide:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto m = note->getBody().toMap();
                auto yah = m[QStringLiteral("yah")].toString();
                auto road = m[QStringLiteral("road")].toString();
                auto remainDistance = m[QStringLiteral("remainDistance")].toInt();
                auto remainTime = m[QStringLiteral("remainTime")].toInt();
                auto remainDistanceText = RgUtils::formatDistance(remainDistance);

                QQmlProperty::write(mViewComponent, "address", yah);
                QQmlProperty::write(mViewComponent, "remainDistance", remainDistance);
                QQmlProperty::write(mViewComponent, "remainDistanceText", remainDistanceText);
                QQmlProperty::write(mViewComponent, "remainTime", remainTime);
            }
        }
        break;

    case ZoneConstants::UpdateSoftkeyAddress:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto address = note->getBody().toString();

                QQmlProperty::write(mViewComponent, "address", address);
            }
        }
        break;

    case NavigationControllerConstants::ChangeTimeFormat:
        {
            auto m = note->getBody().toMap();
            auto format = m["format"].toString();
            QQmlProperty::write(mViewComponent, "timeFormat", format);
        }
        break;

    case RgConstants::UpdateGpsTime:
        parseGpsTime(note->getBody());
        break;

    case MapConstants::RespondToGlobalMapViewStatus:
        {
            auto m = note->getBody().toMap();
            QQmlProperty::write( mViewComponent, "isNaviOrAndo", m["isNaviOrAndo"].toBool() );
            QQmlProperty::write( mViewComponent, "isOnline", m["isOnline"].toBool() );
            QQmlProperty::write( mViewComponent, "showTrafficInfo", m["showTrafficInfo"].toBool() );
            QQmlProperty::write( mViewComponent, "enableTrafficInfo", m["enableTrafficInfo"].toBool() );
            QQmlProperty::write( mViewComponent, "showTrafficInfoOnMap", m["showTrafficInfoOnMap"].toBool() );
            QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLine", m["showTrafficInfoOnRouteLine"].toBool() );
            QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLineForce", m["showTrafficInfoOnRouteLineForce"].toBool() );
//            QQmlProperty::write( mRenderer, "viewLevel", m["viewLevel"].toInt() );
//            QQmlProperty::write( mRenderer, "viewMode", m["viewMode"].toInt() );
//            QQmlProperty::write( mRenderer, "nightMode", m["nightMode"].toBool() );
//            QQmlProperty::write( mRenderer, "skyState", m["skyState"].toInt() );
//#ifdef GADGET_BIND_TO_SIMULATOR
//            QQmlProperty::write( mRenderer, "moveMode", m["moveMode"].toInt() );
//#endif
        }
        break;
    case MapConstants::GlobalMapViewIsNaviOrAndoChanged:
        QQmlProperty::write( mViewComponent, "isNaviOrAndo", note->getBody() );
        break;
    case MapConstants::GlobalMapViewIsOnlineChanged:
        QQmlProperty::write( mViewComponent, "isOnline", note->getBody() );
        break;
    case MapConstants::GlobalMapViewShowTrafficInfoChanged:
        QQmlProperty::write( mViewComponent, "showTrafficInfo", note->getBody() );
        break;
    case MapConstants::GlobalMapViewEnableTrafficInfoChanged:
        QQmlProperty::write( mViewComponent, "enableTrafficInfo", note->getBody() );
        break;
    case MapConstants::GlobalMapViewShowTrafficInfoOnMapChanged:
        QQmlProperty::write( mViewComponent, "showTrafficInfoOnMap", note->getBody() );
        break;
    case MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineChanged:
        QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLine", note->getBody() );
        break;
    case MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineForceChanged:
        QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLineForce", note->getBody() );
        break;
//    case MapConstants::GlobalMapViewViewLevelChanged:
//        QQmlProperty::write( mRenderer, "viewLevel", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewViewModeChanged:
//        QQmlProperty::write( mRenderer, "viewMode", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewNightModeChanged:
//        QQmlProperty::write( mRenderer, "nightMode", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewSkyStateChanged:
//        QQmlProperty::write( mRenderer, "skyState", note->getBody() );
//        break;
#ifdef GADGET_BIND_TO_SIMULATOR
    case MapConstants::GlobalMapViewMoveModeChanged:
        QQmlProperty::write( mRenderer, "moveMode", note->getBody() );
        break;
#endif
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::MapSubCarIcon ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::Icon ) {
//                    auto type = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
//                    qDebug() << "Icon Changed icon:" << type;
//                    sendNotification( NavigationControllerConstants::SetCavatarIcon,
//                      QVariantMap{
//                            {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )},
//                            {QStringLiteral("cavatarType"), type}
//                      }
//                    );
                }
            }
            else if ( section == SettingConstants::DirectionGuid ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::MapMode ) {
//                    auto settingMapMode = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
//                    int vsmMapMode = VSM_VIEW_MODE_BIRDVIEW;
//                    switch ( settingMapMode ) {
//                    case SettingConstants::MapModeBirdView:
//                        vsmMapMode = VSM_VIEW_MODE_BIRDVIEW;
//                        break;
//                    case SettingConstants::MapModeDriveDirection:
//                        vsmMapMode = VSM_VIEW_MODE_HEADUP;
//                        break;
//                    case SettingConstants::MapModeNorthDirection:
//                        vsmMapMode = VSM_VIEW_MODE_NORTHUP;
//                        break;
//                    }

//                    QQmlProperty::write( mRenderer, "viewMode", vsmMapMode );
                }
            }
            // GPS speed display
            else if ( section == SettingConstants::MapMain ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::GpsSpeed) {
                    auto gpsSpeedDisplay = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    QQmlProperty::write(mViewComponent, QStringLiteral("sdiCurrentSpeedNumberVisible"), gpsSpeedDisplay == 1);
                }
            }
        }
        break;
    case TestConstants::UpdateMapTheme:
        {
//            auto m = note->getBody().toMap();
//            DebugUtils::updateMapTheme(TestConstants::GXL, mRenderer, m["item"].toString());
        }
        break;
    case ApplicationConstants::ShowGadget:
        {
            auto m = note->getBody().toMap();
            auto viewType = (UIConstants::GadgetType)m[QStringLiteral("type")].toInt();
            if ( viewType == UIConstants::GadgetXL ) {
                sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            }
        }
        break;

    case NavigationControllerConstants::ChangeLanguage:
        updateLanguage();
        break;

    case ApplicationConstants::ChangedGadgetState:
    case ApplicationConstants::ReplyToGadgetState:
        {
            auto m = note->getBody().toMap();
            mIsGadgetVisible = m["visible"].toBool();
        }
        break;

    case ApplicationConstants::FocusToWidget:
        {
            auto m = note->getBody().toMap();
            auto instanceId = m[CommonConstants::FocusIndex].toInt();
            if (instanceId == UIConstants::GadgetXL) {
                auto hasFocus = m[CommonConstants::Focus].toBool();
                //QQmlProperty::write(mViewComponent, "drawOutline", hasFocus);
            }
        }
        break;

    //case NavigationControllerConstants::ResponseSkyState:
    case NavigationControllerConstants::UpdateSkyState: {
        auto m = note->getBody().toMap();
        auto nightMode = m["nightMode"].toBool();
        QQmlProperty::write( mViewComponent, "skyState", nightMode?1:0 );
    }
        break;
    default:
        return false;
    }

    return true;
}

void WidgetXlViewMediator::parseGpsTime(const QVariant &v)
{
    auto gpsTime = v.value<QDateTime>();
    QQmlProperty::write(mViewComponent, "gpsTime", gpsTime);
    auto time = gpsTime.time();
    auto h = time.hour();
    auto m = time.minute();
    auto s = time.second();
    QQmlProperty::write(mViewComponent, "gpsHour", h);
    QQmlProperty::write(mViewComponent, "gpsMin", m);
    QQmlProperty::write(mViewComponent, "gpsSec", s);
}

void WidgetXlViewMediator::updateLanguage()
{
    auto lang = QmlAppHelper::instance()->language();
    QQmlProperty::write(mViewComponent, "lang", lang);
}


void WidgetXlViewMediator::updateTrafficGuidance(int routeId)
{
    int alternativeRouteId = routeId;
    if (alternativeRouteId < 0 || alternativeRouteId > 2) {
        alternativeRouteId = 0;
        T_DEBUGV("xxlwidget invalid route id %d for traffic info", routeId);
    }

    // update traffic guidance
    std::vector<VSM_ROUTETRAFFIC> trafficGuide;
    VSM_Bool success = VSMNavi_GetRouteTrafficAllList(alternativeRouteId,trafficGuide);
    if (success == VSM_FALSE)
    {
        T_DEBUG("xxlwidget failed to get route traffic guide");
        return;
    }

    if (trafficGuide.size() > 0)
    {
        // takeout total distance
//		double totalDistance = trafficGuide.back().uAccDist;
//		QQmlProperty::write(mTrafficbarView,QStringLiteral("totalDistance"),totalDistance);

        QVariantList guides;
        for (int i=0;i<trafficGuide.size();i++)
        {
            const VSM_ROUTETRAFFIC& t = trafficGuide.at(i);
            QVariantList guide;
            guide << t.uAccDist << t.uCong;
            guides.append(QVariant::fromValue(guide));
        }

        QQmlProperty::write(mViewComponent, QStringLiteral("trafficGuideList"), guides);
    }
}
