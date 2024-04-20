#include "ApplicationMediator.h"
#include "CommonConstants.h"
#include "SettingConstants.h"
#include "RgConstants.h"
#include "HiddenSettingConstants.h"
#include "SettingDto.h"
#include <QQuickItem>
#include "../map/MapViewMediator.h"
#include "VSM_Navi.h"
#include "QmlAppHelper.h"

namespace SKT {

extern VSM_DriveAutoConfig vsmAutoLevelDefaultConfig(int speedMode=-1);
extern bool applyVsmMapAutoConfig(const QVariantMap& config);

void ApplicationMediator::handleHiddenSetting( const mvc::INotificationPtr& note )
{
    // show vsm map level setting ui
    auto showVsmAutoLevelSettingUi = [this](const QVariantMap& config) {
        // show portable vsm auto level config GUI
        QObject* mapAutoConfigObj =  mViewComponent->findChild<QObject*>( QStringLiteral("mapAutoConfigGui"));
        qDebug() << QStringLiteral("mapautolevel gui show-parent found:%1").arg(quintptr(mapAutoConfigObj));
        QQuickItem* mapAutoConfigItem = qobject_cast<QQuickItem*>( mapAutoConfigObj );
        //                    QMetaObject::invokeMethod(mapAutoConfigItem, "showGui");
        QQmlComponent comp(mQmlEngine,QUrl("qrc:/rsm_ui/qml/view/setting/HiddenVsmMapAutoLevelConfig.qml"), mapAutoConfigObj);
        QQuickItem* item = qobject_cast<QQuickItem*>(comp.create());
       qDebug() << QStringLiteral("mapautolevel gui creation:%1").arg(quintptr(item));

        item->setParentItem(mapAutoConfigItem);
        QQmlProperty::write(mapAutoConfigObj, "configGui", QVariant::fromValue(item));
        QQmlProperty::write(item, "viewMode", QVariant::fromValue(mAutoLevelMode));
        QMetaObject::invokeMethod(item, "setCustomConfig", Q_ARG(QVariant, QVariant::fromValue(config)));
        QMetaObject::invokeMethod(item, "showRestoreButton", Q_ARG(QVariant, false));
        QMetaObject::invokeMethod(mapAutoConfigObj, "hideGui");

        // auto mapview = mQmlEngine->rootContext()->contextProperty("mapView").value<QObject*>();
        auto mapviewMediator = Facade::instance()->retrieveMediator<MapViewMediator>();
        if (!mapviewMediator.isNull()) {
            auto mapview = mapviewMediator.data();
            QObject::connect(item, SIGNAL(applyClicked(const QVariant&)), mapview, SLOT(onAppliedMapAutoLevelConfig(const QVariant&)) );
        }
    };

    auto quitVsmAutoLevelSettingGui = [this]() {
        QObject* mapAutoConfigObj =  mViewComponent->findChild<QObject*>( QStringLiteral("mapAutoConfigGui"));
        qDebug() << QStringLiteral("mapautolevel gui hide-parent found:%1").arg(quintptr(mapAutoConfigObj));
        QMetaObject::invokeMethod(mapAutoConfigObj, "quitGui"); 
    };

    // show vsm debug setting ui
    auto showVsmDebugSettingui = [this]() {
        qDebug() << "create mapconfigDebugUi #1";
        // show portable vsm auto level config GUI
        QObject* mapDebuguiObj =  mViewComponent->findChild<QObject*>( QStringLiteral("mapConfigDebugUi"));
        QQuickItem* mapDebuguiItem = qobject_cast<QQuickItem*>( mapDebuguiObj );
        //                    QMetaObject::invokeMethod(mapAutoConfigItem, "showGui");
        QQmlComponent comp(mQmlEngine,QUrl(QStringLiteral("qrc:/rsm_ui/qml/view/map/MapConfigDebugUi.qml")), mapDebuguiObj);
        if (mapDebuguiObj)
          qDebug() << "create mapconfigDebugUi #2 -1 ok";
        else
            qDebug() << "create mapconfigDebugUi #2 - 2 fails ";

        QQuickItem* item = qobject_cast<QQuickItem*>(comp.create());
        if (item != 0) {
            qDebug() << "create mapconfigDebugUi #3-1 ok ";
            item->setParentItem(mapDebuguiItem);
            QVariant ctx = mQmlEngine->rootContext()->contextProperty("mapView");
            QObject* mapviewObj = ctx.value<QObject*>();
            auto mapviewHandle = qvariant_cast<void*>(mapviewObj->property("nativeHandle"));
            QQmlProperty::write(item, "mapView", QVariant::fromValue(mapviewObj));
            QQmlProperty::write(mapDebuguiObj, "configGui", QVariant::fromValue(item));
            QMetaObject::invokeMethod(mapDebuguiObj, "hideGui");
        } else {
            qDebug() << "create mapconfigDebugUi #3-2 fails ";
            qDebug() <<  comp.errorString();
        }
        // auto mapview = mQmlEngine->rootContext()->contextProperty("mapView").value<QObject*>();
    };


    switch (note->getId()) {
    case SettingConstants::ResponseHiddenSetting: {
        auto data = note->getBody().value<QVariantMap>();
        auto section = data[CommonConstants::Section].toString();
        if ( section != HiddenSettingConstants::HiddenSetting &&
             section != HiddenSettingConstants::HiddenVsmSetting) break;

        auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
        for ( auto setting : settings ) {
            auto key = setting->key().toString();
            auto value = setting->value();
            // map matching
            if ( key == HiddenSettingConstants::MapMatching) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeGpsMapmatch, QVariant(enabled));
            }
            #ifdef TMAP_TMC_QC_TESTING
            // map matching feedback
            else if ( key == HiddenSettingConstants::MapmatchingFeedback) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeMapmatchingFeedback, QVariant(enabled));
            }
            else if ( key == HiddenSettingConstants::MapmatchingDebugText) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeMapmatchingDebugText, QVariant(enabled));
            }
            else if ( key == HiddenSettingConstants::OnlyTunnelMMFB ) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeOnlyTunnelMMFB, enabled);
            }
            else if ( key == HiddenSettingConstants::MMFBLog ) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackLog, enabled);
            }
            else if ( key == HiddenSettingConstants::MmfbGpsMethod ) {
                int method = value.toInt();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackGpsMethod, method);
            }
            else if ( key == HiddenSettingConstants::MmfbAngleOnTunnel ) {
                int method = value.toInt();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackAngleOnTunnel, method);
            }
            else if ( key == HiddenSettingConstants::MmfbDrOnly ) {
                bool enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrOnly, enabled);
            }
            else if ( key == HiddenSettingConstants::MmfbDrCep ) {
                bool enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrCEP, enabled);
            }
            else if ( key == HiddenSettingConstants::MmfbDrCepRange ) {
                int method = value.toFloat();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrCEPrange, method);
            }
            else if ( key == HiddenSettingConstants::MmfbDrCepMMRange ) {
                int method = value.toFloat();
                sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrCEPMMrange, method);
            }
            // mapview FPS, is done already on the controller side before UI startup
            else if ( key == HiddenSettingConstants::VsmUseFps) {
            }
            #endif
            // gps log
            else if ( key == HiddenSettingConstants::GpsLog ) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeGpsLogUsage, QVariant(enabled));
            }
            // screen capture
            else if ( key == HiddenSettingConstants::UseScreenCapture ) {
                mUseScreenShot = value.toBool();
                QQmlProperty::write(mViewComponent, "enableScreenCapture", mUseScreenShot);
            }
            // ndds server
            #if 0 // Do Not USE on startup
            else if ( key == HiddenSettingConstants::NddsServer ) {
                auto server = value.toInt();
                sendNotification(HiddenSettingConstants::ChangeNddsServer, server);
            }
            else if ( key == HiddenSettingConstants::AvNextServer ) {
                auto server = value.toInt();
                sendNotification(HiddenSettingConstants::ChangeAvNextServer, server);
            }
            #endif
            else if ( key == HiddenSettingConstants::TvasVersion ) {
                auto ver = value.toInt();
                sendNotification(HiddenSettingConstants::ChangeTvasVersion, ver);
            }
            // gps bias
            else if ( key == HiddenSettingConstants::GpsTimeBias) {
                auto bias   = value.toString().toFloat();
                sendNotification(HiddenSettingConstants::ApplyGpsTimeBias, bias);
            }
            else if ( key == HiddenSettingConstants::VsmLevelSettingUi ) {
                auto enabled = value.toBool();
                sendNotification(HiddenSettingConstants::ChangeVsmLevelSettingUi, enabled);
            }
            else if ( key == HiddenSettingConstants::VsmDebugSettingUi ) {
                auto enabled = value.toBool();
                qDebug() << "mapconfigDebugUi enabled" << enabled;
                if (enabled) 
                    showVsmDebugSettingui();
            }
            else if ( key == HiddenSettingConstants::VsmAutoLevelMap) {
                auto method = value.toInt();
                qDebug() << "mapautolevel enabled: " << method;
                // Must set RSM2018's default configurations
                // apply to VSM with auto level config saved
                VSM_DriveAutoConfig tv = vsmAutoLevelDefaultConfig();

                float lowLevel  = tv.mLowSpeedViewLevel_P;
                float midLevel  = tv.mMiddleSpeedViewLevel_P;
                float highLevel = tv.mHighSpeedViewLevel_P;
                float lowTilt   = tv.mLowSpeedTiltAngle_P;
                float midTilt   = tv.mMiddleSpeedTiltAngle_P;
                float highTilt  = tv.mHighSpeedTiltAngle_P;

                for ( auto setting : settings ) {
                    auto key = setting->key().toString();
                    auto value = setting->value();
                    if (key == HiddenSettingConstants::VsmAutoLevelLowLevel)
                    { lowLevel = value.toFloat(); continue; }
                    if (key == HiddenSettingConstants::VsmAutoLevelMiddleLevel)
                    { midLevel = value.toFloat(); continue; }
                    if (key == HiddenSettingConstants::VsmAutoLevelHighLevel)
                    { highLevel = value.toFloat(); continue; }
                    if (key == HiddenSettingConstants::VsmAutoLevelLowTilt)
                    { lowTilt = value.toFloat(); continue; }
                    if (key == HiddenSettingConstants::VsmAutoLevelMiddleTilt)
                    { midTilt = value.toFloat(); continue; }
                    if (key == HiddenSettingConstants::VsmAutoLevelHighTilt)
                    { highTilt = value.toFloat(); continue; }
                }


                QVariantMap config;
                config[HiddenSettingConstants::VsmAutoLevelLowLevel]     = lowLevel;
                config[HiddenSettingConstants::VsmAutoLevelMiddleLevel]  = midLevel;
                config[HiddenSettingConstants::VsmAutoLevelHighLevel]    = highLevel;
                config[HiddenSettingConstants::VsmAutoLevelLowTilt]      = lowTilt;
                config[HiddenSettingConstants::VsmAutoLevelMiddleTilt]   = midTilt;
                config[HiddenSettingConstants::VsmAutoLevelHighTilt]     = highTilt;

                applyVsmMapAutoConfig(config);

                // show tunning gui on advanced mode
                if (method == 1) {
                   showVsmAutoLevelSettingUi(config);
                }
            }
            else if ( key == HiddenSettingConstants::Theme ) {                
                auto color = note->getBody().toInt();
                T_DEBUGV( "HiddenSettingConstants::ResponseHiddenSetting #1 %x", color );
                QmlAppHelper::instance()->setMultiSenseColorStateFromHiddenMenu( color );
            }
        }
    }
        break;
    case HiddenSettingConstants::ChangeVsmDebugSettingUi: {
        auto enabled = note->getBody().toBool();
        if (enabled)
            showVsmDebugSettingui();
    }
        break;
    case HiddenSettingConstants::ChangeTheme: {            
            auto color = note->getBody().toInt();
            T_DEBUGV( "HiddenSettingConstants::ChangeTheme #1 %x", color );
            QmlAppHelper::instance()->setMultiSenseColorStateFromHiddenMenu( color );
        }
        break;
    default:
        break;
    }

}


}
