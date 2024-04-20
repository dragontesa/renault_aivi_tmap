#include "MapViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "QmlAppHelper.h"
#include <QWindow>
#include <QMouseEvent>
#include <QQuickItem>
#include "RouteManager.h"
#include "StringHelper.h"
#include "SimulationViewMediator.h"
#include "ViewRegistry.h"
#include "SettingDto.h"
#include "PreferenceVo.h"
#include "PreferenceConstants.h"
#include "SoundConstants.h"
#include "NavigationControllerConstants.h"
#include "rprequest.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "mediator/rg/RgUtils.h"
#include "DebugUtils.h"
#include "ApplicationConstants.h"
#include "MapConstants.h"
#include "PlatformHelper.h"
#include "../app/SystemPopupMediator.h"
#include "../environment.h"
#include "VSM_Map.h"
#include "VSM_Map_Settings.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"

using namespace SKT;

#define CONTINUE_RG_MESSAGE tr("경로 안내를 이어서 안내 받으시겠습니까?")

void MapViewMediator::requestSettingAndPreference()
{
    sendNotification( SettingConstants::RequestSetting,
                      QVariantMap{
                          { CommonConstants::Section, CommonConstants::RouteSearch }
                      } );

    QVariantMap m;
    m[CommonConstants::Section] = SettingConstants::MapMain;
    m[CommonConstants::Key] = SettingConstants::SkyeBox;
    sendNotification( SettingConstants::RequestSetting, m);
    m[CommonConstants::Key] = SettingConstants::DisplayArroundTrafficInfo;
    sendNotification( SettingConstants::RequestSetting, m);
    m[CommonConstants::Key] = SettingConstants::AlwaysDisplayDriveLineTrafficInfo;
    sendNotification( SettingConstants::RequestSetting, m);

    m[CommonConstants::Section] = SettingConstants::MapSubSpeedResponse;
    m[CommonConstants::Key] = SettingConstants::SpeedResponseMapUse;
    sendNotification(SettingConstants::RequestSetting, m);
    m[CommonConstants::Key] = SettingConstants::Distance;
    sendNotification(SettingConstants::RequestSetting, m);

    // MapView 생성 이후 카바타 설정이 되어야 초기 화면에서 설정된 카바타가 나옴
    // 17.12.12 ymhong
    sendNotification(NavigationControllerConstants::UpdateCavatarIcon,QVariant());

    // 맵뷰에 글자의 크기를 적용
    // 19.11.06 cjlee
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyFontScale );
    sendNotification( NavigationControllerConstants::ApplyController, body );
}

void MapViewMediator::handleSettingAndPreference( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        {
            T_DEBUG("[MapViewMediator]ApplySetting RouteSearch");

            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();

            if ( section == CommonConstants::RouteSearch ) {
                auto settings = data[QStringLiteral("settings")].value<QList<SettingDtoPtr>>();

                for ( auto setting : settings ) {
                    auto key = setting->key().toString();
                    if ( key == QLatin1String("AutoRedetection") ) {    // periodic
                        mSupportPeriodicReroute = setting->value().toBool();
                    } else if ( key == QLatin1String("RedetectRouteVeerOff") ) {    // breakaway
                        mSupportBreakawayReroute = setting->value().toBool();
                    }
                }
            }

            // GPS speed display
            if ( section == SettingConstants::MapMain ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::GpsSpeed) {
                    T_DEBUG("[MapViewMediator]ApplySetting GpsSpeed");
                    auto gpsSpeedDisplay = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    QQmlProperty::write(mViewComponent, "gpsSpeedDisplay", gpsSpeedDisplay);
                } else if (key == SettingConstants::SkyeBox) {
                    T_DEBUG("[MapViewMediator]ApplySetting SkyeBox");
                    auto enable = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                    QMetaObject::invokeMethod(mViewComponent, "setSkyboxEnable", Q_ARG(QVariant, enable));
                    mSkyboxEnabled = enable;
                } else if (key == SettingConstants::DisplayArroundTrafficInfo) {
                    T_DEBUG("[MapViewMediator]ApplySetting DisplayArroundTrafficInfo");
                    auto enable = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                    sendNotification(MapConstants::ShowTrafficInfoOnMap, enable);
                } else if (key == SettingConstants::AlwaysDisplayDriveLineTrafficInfo) {
                    T_DEBUG("[MapViewMediator]ApplySetting AlwaysDisplayDriveLineTrafficInfo");
                    auto enable = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                    sendNotification(MapConstants::ShowTrafficInfoOnRouteLine, enable);
                }
            }

            auto key = data[CommonConstants::Key].toString();
            if ( section == SettingConstants::DirectionGuid && key == SettingConstants::MapMode ) {
                auto mapMode = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                T_DEBUG("[MapViewMediator]ApplySetting MapMode");
                mMapModeSetting = mapMode;
                if ( MapViewMode::isInDrive( mMapViewMode ) ) {
                    restoreDriveMapModeAndLevel();
                }
            }
            if (section == SettingConstants::MapSubSpeedResponse) {
                T_DEBUG("[MapViewMediator]ApplySetting MapSubSpeedResponse");
                auto setting = data[CommonConstants::Setting].value<SettingDtoPtr>();
                if (key == SettingConstants::SpeedResponseMapUse) {
                    auto speedResponseMapUse = setting->value().toBool();

                    mUseSpeedResponseMap = speedResponseMapUse;
                }
             }
        }
        break;

    case SettingConstants::ChangedSetting:
        {
            T_DEBUG("[MapViewMediator]ChangedSetting");

            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();

            if ( section == CommonConstants::RouteSearch ) {
                auto item = data[CommonConstants::Setting].value<SettingDtoPtr>();

                if ( key == QLatin1String("AutoRedetection") ) {    // periodic
                    mSupportPeriodicReroute = item->value().toBool();
                } else if ( key == QLatin1String("RedetectRouteVeerOff") ) {    // breakaway
                    mSupportBreakawayReroute = item->value().toBool();
                }
            }
            if ( section == SettingConstants::DirectionGuid && key == SettingConstants::MapMode ) {
                auto mapMode = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                T_DEBUG( QString( "<B>ChangedSetting mapMode %1</B>" ).arg( mapMode ) );
                mMapModeSetting = mapMode;
                if ( MapViewMode::isInDrive( mMapViewMode ) ) {
                    T_DEBUG( QString( "<B>ChangedSetting.1 mapMode %1</B>" ).arg( mapMode ) );
                    restoreDriveMapModeAndLevel();
                }
            }
            if (section == SettingConstants::MapMain) {
                if (key == SettingConstants::SkyeBox) {
                    auto enable = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                    QMetaObject::invokeMethod(mViewComponent, "setSkyboxEnable", Q_ARG(QVariant, enable));
                    if (enable) {
                        QTimer::singleShot( 500, this, [&](){
                           auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));
                           VSMCameraConfig config;
                           auto success = VSMMap_Setting_GetCameraConfig(handle, config);
                           success = VSMMap_SetTiltAngle(handle, config.max3dAngles[10], true);
                        } );

                    }
                } else if (key == SettingConstants::DisplayArroundTrafficInfo) {
                    auto enable = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                    sendNotification(MapConstants::ShowTrafficInfoOnMap, enable);
                } else if (key == SettingConstants::AlwaysDisplayDriveLineTrafficInfo) {
                    auto enable = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toBool();
                    sendNotification(MapConstants::ShowTrafficInfoOnRouteLine, enable);
                }
            }
            if (section == SettingConstants::MapSubSpeedResponse) {
                auto setting = data[CommonConstants::Setting].value<SettingDtoPtr>();
                if (key == SettingConstants::SpeedResponseMapUse) {
                    auto speedResponseMapUse = setting->value().toBool();

                    mUseSpeedResponseMap = speedResponseMapUse;
                }
             }
        }
        break;

    case PreferenceConstants::ApplyPreference:
        {
            // 경로 이어 가기
            auto result = note->getBody().value<QVariantMap>();
            auto req = result[QStringLiteral("request")].value<QVariantMap>();
            auto caller = req[CommonConstants::Caller].toString();
            auto from   = req[CommonConstants::From].toInt();
            if ( caller == QStringLiteral("MapViewMediator") ) {
                mRouteContPrefs = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                auto isNormallyTerminated = mRouteContPrefs[QStringLiteral("isNormallyTerminated")].value().toBool();
                bool ok = false;
                auto selectedRouteIndex = mRouteContPrefs[QStringLiteral("selectedRouteIndex")].value().toInt( &ok );
                auto address = mRouteContPrefs[RgConstants::Address].value().toString();

                if ( ok ) {
                    if (!isNormallyTerminated && PlatformHelper::instance()->isRgContinueEnable()) {
                        if ( selectedRouteIndex > -1 ) {
                            QVariantMap m;
                            m["title"] = tr("알림");
                            m["message"] = CONTINUE_RG_MESSAGE;
                            m["message2"] = address;
                            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
                            m[CommonConstants::ParentObjectName] = CommonConstants::ContentRootForCommonDialog;
                            m[CommonConstants::Menus] = QVariantList { CommonConstants::Cancel, CommonConstants::Ok };
                            m["showTimer"] = false;
                            m["popupType"] = SystemPopupMediator::ContinueRg;
                            QObject *popupRoot = nullptr;
#if defined(BUILD_TARGET)
                            popupRoot = PlatformHelper::instance()->popupRoot();
#endif
                            if (popupRoot) {
                                m[CommonConstants::Parent] = QVariant::fromValue(popupRoot);
                            } else {
                                m[CommonConstants::ParentObjectName] = "popupWrap";
                            }
                            sendNotification(Constants::CreateView, m);//CreateSystemPopup, m);

                            // Map update 뷰가 있다면 hide 시킴
                            // 19.10.11 ymhong
                            sendNotification(Constants::HideView, QVariantMap{
                                                 {CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView)}
                                             });
                        }
                    }
                    else if (from == NavigationControllerConstants::ChangedUserProfile ) { // RG needs restarting on profile changing  2018.3.19 by lckxx
                        T_DEBUG("[MapViewMediator]ChangedUserProfile");

                        auto selectedRouteOption = mRouteContPrefs["selectedRouteOption"].value().toInt();
                        auto via = mRouteContPrefs["via"].value().toStringList();
                        QVariantMap m;
                        m[QStringLiteral("selectedRouteIndex")] = selectedRouteIndex;
                        m[QStringLiteral("selectedRouteOption")] = selectedRouteOption;
                        m[QStringLiteral("via")]  = via;
                        sendNotification(RgConstants::StartRouteGuidanceContinuously, m);
                    } else {
                        sendNotification(ZoneConstants::ShowRecentTileGridView);
                    }
                }
            }

            auto preferenceName = req[SettingConstants::PreferencesName].toString();
            auto section = req[SettingConstants::Section].toString();
            auto key = req[SettingConstants::Key].toString();
            if (preferenceName == PreferenceConstants::Rg && section == SettingConstants::MapMain) {
                if (key == "viewLevel") {
                    auto response = result["response"].value<PreferenceVo>();
                    auto viewLevel = response.value().toInt();

                    QMetaObject::invokeMethod(mViewComponent, "setMapViewLevelWithNoAnim", Q_ARG(QVariant,viewLevel));
                }
            }
        }
        break;

    case SettingConstants::DialogAccepted:
        {
            auto m = note->getBody().toMap();
            auto message = m["message"];
            if (message == CONTINUE_RG_MESSAGE) {
                auto selectedRouteIndex = mRouteContPrefs["selectedRouteIndex"].value().toInt();
                auto selectedRouteOption = mRouteContPrefs["selectedRouteOption"].value().toInt();
                auto via = mRouteContPrefs["via"].value().toStringList();
                QVariantMap data;
                data["selectedRouteIndex"] = selectedRouteIndex;
                data["selectedRouteOption"] = selectedRouteOption;
                data["via"] = via;

                // App이 foreground가 되면 실행 되도록 예약
                // Map의 Size가 변경 되면서 RouteCandidateView가 실행 중 순서가 뒤바뀜
                // RouteCandidateView에 visible에 영향을 받는 코드가 있음
                // 19.07.06 ymhong
                sendNotification(NavigationControllerConstants::ReserveAppSwitchJob, QVariantMap{
                                     {CommonConstants::Command, RgConstants::StartRouteGuidanceContinuously},
                                     {CommonConstants::Arg, data}
                                 });
            } else if (message == SettingConstants::InCompleteMapDataKo) {
                 QString ignorePopup = Environment::getMapUpdatePath() + QDir::separator() + SettingConstants::IgnorePopupFileName;
                 QFile out(ignorePopup);
                 if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                     out.close();
                 }
            }
        }
        break;

    case SettingConstants::DialogCanceled:
        {
            auto m = note->getBody().toMap();
            auto message = m["message"];
            if (message == CONTINUE_RG_MESSAGE) {
                // 경로 이어하기 다이얼로그에서 취소를 누르면 경로 안내 정상 종료로 처리.
                sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                     {"preferencesName",PreferenceConstants::Rg},
                     {CommonConstants::Section,QStringLiteral("Termination")},
                     {CommonConstants::Key,QStringLiteral("isNormallyTerminated")},
                     {CommonConstants::Value,true}
                 });

                sendNotification(RouteConstants::ClearRouteDirectory);
            }
        }
        break;
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        {
            T_DEBUG("[MapViewMediator]ChangedUserProfile");
            requestSettingAndPreference();
        }
        break;
    }            
}
