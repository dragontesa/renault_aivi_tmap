#include "WidgetLargeViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "ZoneConstants.h"
#include "ViewRegistry.h"
#include "WidgetUtils.h"
#include "NavigationControllerConstants.h"
#include "ApplicationConstants.h"
#include "../rg/RgUtils.h"
#include "satellitedataptr.h"
#include "MapConstants.h"
#include "satellitedataptr.h"
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

using namespace SKT;

WidgetLargeViewMediator::WidgetLargeViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

WidgetLargeViewMediator::~WidgetLargeViewMediator()
{
}

QList<int> WidgetLargeViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        ZoneConstants::UpdateSoftkeyRouteGuide,
        ZoneConstants::UpdateSoftkeyAddress,
        RgConstants::UpdateTbtState,
        RgConstants::UpdateSdiState,
//        RgConstants::UpdateTrafficGuidance,
        NavigationControllerConstants::ChangeTimeFormat,
        RgConstants::UpdateGpsTime,
//        MapConstants::RespondToGlobalMapViewStatus,
//        MapConstants::GlobalMapViewIsNaviOrAndoChanged,
//        MapConstants::GlobalMapViewIsOnlineChanged,
//        MapConstants::GlobalMapViewShowTrafficInfoChanged,
//        MapConstants::GlobalMapViewEnableTrafficInfoChanged,
//        MapConstants::GlobalMapViewShowTrafficInfoOnMapChanged,
//        MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineChanged,
//        MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineForceChanged,
//        MapConstants::GlobalMapViewViewLevelChanged,
//        MapConstants::GlobalMapViewViewModeChanged,
//        MapConstants::GlobalMapViewNightModeChanged,
//        MapConstants::GlobalMapViewSkyStateChanged,
//        MapConstants::GlobalMapViewMoveModeChanged,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
//        TestConstants::UpdateMapTheme,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ShowGadget,
        ApplicationConstants::FocusToWidget
    };

    return notes;
}

bool WidgetLargeViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            mRgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();

            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            sendNotification( RgConstants::RequestTbtStateUpdate );
            sendNotification( RgConstants::RequestSdiStateUpdate );
            sendNotification(RgConstants::RequestTrafficGuidance);

//            mRenderer = findChild( "renderer" );
//            QVariantMap m;
//            m["mapView"] = QVariant::fromValue( mRenderer );
//            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);

//            m["type"] = QVariant::fromValue( (int)VsmMapType::Global );
//            sendNotification(MapConstants::ApplyMapSetting, m);
//            sendNotification(MapConstants::ApplyMapTheme, m);

//            sendNotification(MapConstants::RequestGlobalMapViewStatus);
//            sendNotification(NavigationControllerConstants::UpdateCavatarIcon,
//                 QVariantMap{
//                     {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )}
//                 });

#ifdef BUILD_TARGET
//            if (QQmlProperty::read(mViewComponent, "asView").toBool() == false) {
//                QQmlProperty::write(mViewComponent, "verticalFlip", true);
//            }
#endif
//            QJsonObject root;
//            QJsonObject carvatar;
//            carvatar["width"] = 70.0f;
//            root["carvatar"] = carvatar;
//            auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

//            auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));
//            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_DAY, json.data());
//            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_NIGHT, json.data());

//            DebugUtils::updateMapTheme(TestConstants::GL, mRenderer, TestConstants::GL_CS_C);
//            DebugUtils::updateMapTheme(TestConstants::GL, mRenderer, TestConstants::GL_RLT);
//            DebugUtils::updateMapTheme(TestConstants::GL, mRenderer, TestConstants::GL_RLTT);

            // GPS speed display
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                { CommonConstants::Section, SettingConstants::MapMain },
                { CommonConstants::Key, SettingConstants::GpsSpeed }
            });

            updateLanguage();
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

bool WidgetLargeViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                auto m = note->getBody().toMap();
                auto isRg = m["isRg"].toBool();
                QMetaObject::invokeMethod(mViewComponent, "changeRgComponent", Q_ARG(QVariant, isRg));
            }
        }
        break;

    case RgConstants::UpdateTbtState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleTbtUpdate( WidgetConstants::SizeLarge, mViewComponent, note );
            }
        }
        break;

    case RgConstants::UpdateSdiState:
        {
            if ( !mRgProxy->isSimulatorRunning() ) {
                WidgetUtils::handleSdiUpdate( WidgetConstants::SizeLarge, mViewComponent, note );
            }
        }
        break;
//    case RgConstants::UpdateTrafficGuidance:
//        {
//            auto m = note->getBody().toMap();
//            auto distance = m["distance"].toInt();
//            auto traffic = m["traffic"].toList();

//            if (traffic.count()) {
//                QQmlProperty::write(mViewComponent, "totalDistance", distance);
//                QQmlProperty::write(mViewComponent, "leftDistance", distance);
//                QQmlProperty::write(mViewComponent, "trafficGuideList", traffic);
//            }
//        }
//        break;
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

//    case MapConstants::RespondToGlobalMapViewStatus:
//        {
//            auto m = note->getBody().toMap();
//            QQmlProperty::write( mViewComponent, "isNaviOrAndo", m["isNaviOrAndo"].toBool() );
//            QQmlProperty::write( mViewComponent, "isOnline", m["isOnline"].toBool() );
//            QQmlProperty::write( mViewComponent, "showTrafficInfo", m["showTrafficInfo"].toBool() );
//            QQmlProperty::write( mViewComponent, "enableTrafficInfo", m["enableTrafficInfo"].toBool() );
//            QQmlProperty::write( mViewComponent, "showTrafficInfoOnMap", m["showTrafficInfoOnMap"].toBool() );
//            QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLine", m["showTrafficInfoOnRouteLine"].toBool() );
//            QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLineForce", m["showTrafficInfoOnRouteLineForce"].toBool() );
//            QQmlProperty::write( mRenderer, "viewLevel", m["viewLevel"].toInt() );
//            QQmlProperty::write( mRenderer, "viewMode", m["viewMode"].toInt() );
//            QQmlProperty::write( mRenderer, "nightMode", m["nightMode"].toBool() );
//            QQmlProperty::write( mRenderer, "skyState", m["skyState"].toInt() );
//#ifdef GADGET_BIND_TO_SIMULATOR
//            QQmlProperty::write( mRenderer, "moveMode", m["moveMode"].toInt() );
//#endif
//        }
//        break;
//    case MapConstants::GlobalMapViewIsNaviOrAndoChanged:
//        QQmlProperty::write( mViewComponent, "isNaviOrAndo", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewIsOnlineChanged:
//        QQmlProperty::write( mViewComponent, "isOnline", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewShowTrafficInfoChanged:
//        QQmlProperty::write( mViewComponent, "showTrafficInfo", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewEnableTrafficInfoChanged:
//        QQmlProperty::write( mViewComponent, "enableTrafficInfo", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewShowTrafficInfoOnMapChanged:
//        QQmlProperty::write( mViewComponent, "showTrafficInfoOnMap", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineChanged:
//        QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLine", note->getBody() );
//        break;
//    case MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineForceChanged:
//        QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLineForce", note->getBody() );
//        break;
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
//#ifdef GADGET_BIND_TO_SIMULATOR
//    case MapConstants::GlobalMapViewMoveModeChanged:
//        QQmlProperty::write( mRenderer, "moveMode", note->getBody() );
//        break;
//#endif
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();
//            if ( section == SettingConstants::MapSubCarIcon ) {
//                auto key = data[CommonConstants::Key].toString();
//                if ( key == SettingConstants::Icon ) {
//                    auto type = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
//                    qDebug() << "Icon Changed icon:" << type;
//                    sendNotification( NavigationControllerConstants::SetCavatarIcon,
//                      QVariantMap{
//                            {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )},
//                            {QStringLiteral("cavatarType"), type}
//                      }
//                    );
//                }
//            }
//            else if ( section == SettingConstants::DirectionGuid ) {
//                auto key = data[CommonConstants::Key].toString();
//                if ( key == SettingConstants::MapMode ) {
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
//                }
//            }
            // GPS speed display
            if ( section == SettingConstants::MapMain ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::GpsSpeed) {
                    auto gpsSpeedDisplay = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    QQmlProperty::write(mViewComponent, QStringLiteral("sdiCurrentSpeedNumberVisible"), gpsSpeedDisplay == 1);
                }
            }
        }
        break;

//    case TestConstants::UpdateMapTheme:
//        {
//            auto m = note->getBody().toMap();
//            DebugUtils::updateMapTheme(TestConstants::GL, mRenderer, m["item"].toString());
//        }
//        break;
//    case ApplicationConstants::ShowGadget:
//        {
//            auto args = note->getBody().toMap();
//            auto show = args[QStringLiteral("show")].toBool();
//            int fps = show ? WidgetConstants::FpsOnForeground : WidgetConstants::FpsOnBackground;
//            qDebug() << "[Perf][setExpectedFps]" << fps;
//            QMetaObject::invokeMethod( mRenderer, "setExpectedFps", Q_ARG(int, fps) );
//        }
//        break;

    case NavigationControllerConstants::ChangeLanguage:
        updateLanguage();
        break;

    case ApplicationConstants::FocusToWidget:
        {
            auto m = note->getBody().toMap();
            auto instanceId = m[CommonConstants::FocusIndex].toInt();
            if (instanceId == UIConstants::GadgetLarge) {
                auto hasFocus = m[CommonConstants::Focus].toBool();
                //QQmlProperty::write(mViewComponent, "drawOutline", hasFocus);
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

void WidgetLargeViewMediator::parseGpsTime(const QVariant &v)
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

void WidgetLargeViewMediator::updateLanguage()
{
    auto lang = QmlAppHelper::instance()->language();
    QQmlProperty::write(mViewComponent, "lang", lang);
}
