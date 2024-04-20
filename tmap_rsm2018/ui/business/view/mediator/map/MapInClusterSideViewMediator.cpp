#include "MapInClusterSideViewMediator.h"
#include "Constants.h"
#include "MapConstants.h"
#include "QmlAppHelper.h"
#include "NavigationControllerConstants.h"
#include <QQuickItem>
#include <QQuickWindow>
#include <QJsonDocument>
#include "VSM_Map.h"
#include "VsmMapSettingAndTheme.h"
#include "../common/mapinclusterlayout.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "ApplicationConstants.h"
#include "TestConstants.h"
#include "DebugUtils.h"
#include "SettingProxy.h"
#include "VSM_Map.h"
#include "VSM_Map_Settings.h"
#include "SettingConstants.h"

using namespace SKT;

#define TMAP_CLUSTER_ADJUST_SKYBOX_HEIGHT

#define DEFAULT_TRAFFIC_NODATA_COLOR                        0x188BF4FF  // 0x6c6c6cff
#define DEFAULT_TRAFFIC_NODATA_OUTLINE_COLOR                0x1E5E94FF  // 0x464646ff
#define DEFAULT_TRAFFIC_GOOD_COLOR                          0x03bc6fff
#define DEFAULT_TRAFFIC_GOOD_OUTLINE_COLOR                  0x0c753dff
#define DEFAULT_TRAFFIC_SLOW_COLOR                          0xfb9a0dff
#define DEFAULT_TRAFFIC_SLOW_OUTLINE_COLOR                  0x9f5804ff
#define DEFAULT_TRAFFIC_BAD_COLOR                           0xf21616ff // 0xf1522aff
#define DEFAULT_TRAFFIC_BAD_OUTLINE_COLOR                   0x823a2fff

#define DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR                  0x00B4FFFF  // 0x727272ff
#define DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR          0x036A94FF  // 0x464646ff
#define DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR                    0x13bc62ff
#define DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR            0x046f36ff
#define DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR                    0xfb9a0dff
#define DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR            0x9f5804ff
#define DEFAULT_TRAFFIC_BAD_NIGHT_COLOR                     0xf21616ff//0xf1522aff
#define DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR             0x823a2fff

static QString decRGBA2Hex(qint32 rgba)
{
    return QString("#%1").arg((uint)rgba, 8, 16, QLatin1Char('0'));
}

MapInClusterSideViewMediator::MapInClusterSideViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    mRenderer = findChild(QStringLiteral("renderer"));
}

MapInClusterSideViewMediator::~MapInClusterSideViewMediator()
{
}

QList<int> MapInClusterSideViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        MapConstants::RespondToGlobalMapViewStatus,
        MapConstants::GlobalMapViewIsNaviOrAndoChanged,
        MapConstants::GlobalMapViewIsOnlineChanged,
        MapConstants::GlobalMapViewShowTrafficInfoChanged,
        MapConstants::GlobalMapViewEnableTrafficInfoChanged,
        MapConstants::GlobalMapViewShowTrafficInfoOnMapChanged,
        MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineChanged,
        MapConstants::GlobalMapViewShowTrafficInfoOnRouteLineForceChanged,
        MapConstants::GlobalMapViewViewLevelChanged,
        ApplicationConstants::ActivatedView,
        ApplicationConstants::DeactivatedView,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        TestConstants::UpdateMapTheme
    };

    return notes;
}

bool MapInClusterSideViewMediator::onEvent( int event, const QVariant& data )
{
    switch( event ) {
    case Mediated:
        {
            auto m = data.toMap();
            mRenderer = m["renderer"].value<QObject*>();
            auto handle = qvariant_cast<void*>(mRenderer->property("nativeHandle"));

            VSMMap_SetSkyType(handle, VSM_SKY_CLUSTER);
            VSMMap_SetTheme(handle, VSM_THEME_CLUSTER);

            m.clear();
            m["mapView"] = QVariant::fromValue( mRenderer );
            m["type"] = VSM_VIEW_TYPE_CLUSTER;
            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);

            m["type"] = QVariant::fromValue( (int)VsmMapType::Cluster );
            sendNotification(MapConstants::ApplyMapSetting, m);

            QJsonObject root;
            QJsonObject carvatar;
            carvatar["width"] = 100.0f;
            root["carvatar"] = carvatar;
            auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());
#ifdef HIDE_CLUSTER_POI
            VSMMap_Setting_SetShowPOI(handle, VSM_FALSE);
#endif

            sendNotification(MapConstants::RequestGlobalMapViewStatus);

            sendNotification(NavigationControllerConstants::UpdateCavatarIcon,
                 QVariantMap{
                     {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )}
                 });

//            DebugUtils::updateMapTheme(TestConstants::CS, mRenderer, TestConstants::C_S_CS_C);
//            DebugUtils::updateMapTheme(TestConstants::CS, mRenderer, TestConstants::C_S_RLT);
//            DebugUtils::updateMapTheme(TestConstants::CS, mRenderer, TestConstants::C_S_RLTT);

            // route line thickness
            {
                QJsonObject routeLine;

                float newSize = 20;

                routeLine["width"] = newSize;
                QJsonObject root;
                root["routeLine"] = routeLine;
                auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

                VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());

                qDebug() << "[UpdateMapTheme]C_C_RLT: " << newSize;
            }

            // route line tbt
            {
                float newSize = 20;

                QJsonObject turnPoint;
                turnPoint["width"] = newSize;
                turnPoint["floatGap"] = 10;

                QJsonObject routeLine;
                routeLine["turnPoint"] = turnPoint;

                QJsonObject congestion;
                congestion["noDataFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_NIGHT_COLOR);
                congestion["noDataStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_NODATA_NIGHT_OUTLINE_COLOR);
                congestion["goodFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_NIGHT_COLOR);
                congestion["goodStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_GOOD_NIGHT_OUTLINE_COLOR);
                congestion["slowFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_NIGHT_COLOR);
                congestion["slowStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_SLOW_NIGHT_OUTLINE_COLOR);
                congestion["badFillColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_NIGHT_COLOR);
                congestion["badStrokeColor"] = decRGBA2Hex(DEFAULT_TRAFFIC_BAD_NIGHT_OUTLINE_COLOR);
                routeLine["congestion"] = congestion;

                QJsonObject root;
                root["routeLine"] = routeLine;
                auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

                VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());

                VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, "{ \"tile\":{\"backgroundResourceCode\":\"BGIMAGE_BLACK\"} }");

                qDebug() << "[UpdateMapTheme]C_C_RLTT: " << newSize;
            }

#ifdef TMAP_CLUSTER_ADJUST_SKYBOX_HEIGHT
            qDebug() << "[Cluster]RendererCreated #2";

            // skybox
            QTimer::singleShot(1, this, [handle, this]()
            {
                qDebug() << "[Cluster]RendererCreated #3 handle: " << handle;

                VSMCameraConfig cameraConfig;
                VSMMap_Setting_GetDefaultCameraConfig(cameraConfig);

                // TMCRSM-3995 set the default tilt angle to be always shown in 3d bird view
                QQmlProperty::write( mRenderer, QStringLiteral("tiltAngle"), cameraConfig.max3dAngles[10] );

                VSMSkyBoxConfig skyboxConfig;
                VSMMap_Setting_GetDefaultSkyboxConfig(skyboxConfig);

                // TODO: copy and paste skybox params here
                // 2018_06_28_16_41_10 - generated param -- for boundingbox, landmarks
                std::array<float, VSM_VIEWLEVEL_COUNT> max3dAngles {30.00f, 30.00f, 30.00f, 30.00f, 30.00f, 30.00f, 30.00f, 30.00f, 61.00f, 65.00f, 68.00f, 76.00f, 80.00f, 83.00f
                };

                const float scale = 0.625;
                float base = max3dAngles.at(8);
                for (int i = 8, count = max3dAngles.size(); i < count; i++ ) {
                    auto oldAngle = max3dAngles.at(i);
                    auto newAngle = oldAngle - (oldAngle - base) * scale;
                    max3dAngles[i] = newAngle;
                }

                std::array<float, VSM_VIEWLEVEL_COUNT> skyWidthWeight {1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f, 1.00f
                };
                std::array<float, VSM_VIEWLEVEL_COUNT> poiClipY {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 30.00f, 30.00f, 35.00f, 45.00f, 40.00f, 40.00f
                };
                std::array<float, VSM_VIEWLEVEL_COUNT> buildingClipY {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 20.00f, 90.00f, 65.00f, 55.00f, 40.00f, 20.00f
                };
                std::array<float, VSM_VIEWLEVEL_COUNT> landmarkClipY {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 75.00f, 50.00f, 50.00f, 50.00f, 0.00f
                };
                std::array<float, VSM_VIEWLEVEL_COUNT> fogHeight {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 32.00f, 34.00f, 36.00f, 38.00f, 40.00f, 30.00f
                };
                std::array<float, VSM_VIEWLEVEL_COUNT> horizonAngle {77.00f, 77.00f, 77.00f, 77.00f, 77.00f, 77.00f, 77.00f, 77.00f, 77.00f, 79.00f, 80.00f, 85.50f, 87.00f, 88.00f
                };
                // End of TODO: copy and paste skybox params here

//                for (auto& val : max3dAngles ) {
//                    val = 0;
//                }
//                for (auto& val : skyWidthWeight ) {
//                    val = 0;
//                }
//                for (auto& val : poiClipY ) {
//                    val = 0;
//                }
//                for (auto& val : buildingClipY ) {
//                    val = 0;
//                }
//                for (auto& val : landmarkClipY ) {
//                    val = 0;
//                }
//                for (auto& val : fogHeight ) {
//                    val = 0;
//                }
//                for (auto& val : horizonAngle ) {
//                    val = 0;
//                }

                skyboxConfig.smoothMode = VSM_TRUE;
                skyboxConfig.visible = VSM_TRUE;

                std::copy(max3dAngles.begin(), max3dAngles.end(), std::begin(cameraConfig.max3dAngles));
                std::copy(skyWidthWeight.begin(), skyWidthWeight.end(), std::begin(skyboxConfig.skyWidthWeight));
                std::copy(poiClipY.begin(), poiClipY.end(), std::begin(skyboxConfig.poiClipY));
                std::copy(buildingClipY.begin(), buildingClipY.end(), std::begin(skyboxConfig.buildingClipY));
                std::copy(fogHeight.begin(), fogHeight.end(), std::begin(skyboxConfig.fogHeight));
                std::copy(horizonAngle.begin(), horizonAngle.end(), std::begin(skyboxConfig.horizonAngle));
                std::copy(landmarkClipY.begin(), landmarkClipY.end(), std::begin(skyboxConfig.landmarkClipY));

                VSMMap_Setting_SetCameraConfig(handle, cameraConfig);
                VSMMap_Setting_SetSkyboxConfig(handle, skyboxConfig);

                QQmlProperty::write( mRenderer, QStringLiteral("viewLevel"), 11 );
            });
#endif
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                { CommonConstants::Section, SettingConstants::MapSubCarIcon },
                { CommonConstants::Key, SettingConstants::Icon }
            });
        }
        break;
    case RendererOnDestruction:
        {
            sendNotification(MapConstants::DetachMapViewFromVsmNavi,data);
        }
        break;
    default:
        return true;
    }
        
    return false;
}

bool MapInClusterSideViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    auto m = note->getBody().toMap();

    switch ( note->getId() ) {
    case MapConstants::RespondToGlobalMapViewStatus:
        QQmlProperty::write( mViewComponent, "isNaviOrAndo", m["isNaviOrAndo"].toBool() );
        QQmlProperty::write( mViewComponent, "isOnline", m["isOnline"].toBool() );
        QQmlProperty::write( mViewComponent, "showTrafficInfo", m["showTrafficInfo"].toBool() );
        QQmlProperty::write( mViewComponent, "enableTrafficInfo", m["enableTrafficInfo"].toBool() );
        QQmlProperty::write( mViewComponent, "showTrafficInfoOnMap", m["showTrafficInfoOnMap"].toBool() );
        QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLine", m["showTrafficInfoOnRouteLine"].toBool() );
        QQmlProperty::write( mViewComponent, "showTrafficInfoOnRouteLineForce", m["showTrafficInfoOnRouteLineForce"].toBool() );
        QQmlProperty::write( mRenderer, "viewLevel", m["viewLevel"].toInt() );
        break;
    case ApplicationConstants::ActivatedView:
        {
            auto m = note->getBody().value<QVariantMap>();
            auto objName = m[CommonConstants::ObjectName];
            if ( objName == QStringLiteral( "MapControlView" ) ) {
                QQmlProperty::write( mViewComponent, "isUnderControl", true );
            }
        }
        break;
    case ApplicationConstants::DeactivatedView:
        {
            auto m = note->getBody().value<QVariantMap>();
            auto objName = m[CommonConstants::ObjectName];
            if ( objName == QStringLiteral( "MapControlView" ) ) {
                QQmlProperty::write( mViewComponent, "isUnderControl", false );
            }
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
    case MapConstants::GlobalMapViewViewLevelChanged:
        //QQmlProperty::write( mRenderer, "viewLevel", note->getBody() );
        break;
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
        {
            // cavatar
            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::MapSubCarIcon ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::Icon ) {
                    auto type = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    qDebug() << "Icon Changed icon:" << type;
                    sendNotification( NavigationControllerConstants::SetCavatarIcon,
                      QVariantMap{
                            {QStringLiteral("mapView"), QVariant::fromValue( mRenderer )},
                            {QStringLiteral("cavatarType"), type}
                      }
                    );
                }
            }

            // TMCRSM-3995 set the default tilt angle to be always shown in 3d bird view
            if (section == SettingConstants::MapSubSpeedResponse) {
                T_DEBUG("[MapViewMediator]ApplySetting MapSubSpeedResponse");
                auto setting = data[CommonConstants::Setting].value<SettingDtoPtr>();
                auto key = data[CommonConstants::Key].toString();
                if (key == SettingConstants::SpeedResponseMapUse) {
                    auto speedResponseMapUse = setting->value().toBool();

                    VSMCameraConfig cameraConfig;
                    VSMMap_Setting_GetDefaultCameraConfig(cameraConfig);

                    // TMCRSM-3995 set the default tilt angle to be always shown in 3d bird view
                    QQmlProperty::write( mRenderer, QStringLiteral("tiltAngle"), cameraConfig.max3dAngles[10] );
                }
             }
        }
        break;
    case TestConstants::UpdateMapTheme:
        {
            auto m = note->getBody().toMap();
            DebugUtils::updateMapTheme(TestConstants::CS, mRenderer, m["item"].toString());
        }
        break;

    }

    return false;
}
