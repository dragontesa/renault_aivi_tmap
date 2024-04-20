#include "DebugUtils.h"
#include "TestConstants.h"
#include "SettingConstants.h"
#include "SettingProxy.h"
#include "VSM_Map.h"
#include "TMediator.h"

using namespace SKT;

void DebugUtils::handleSetting( const QString& key, const std::function<void(const QVariant& value)>& handler )
{
#ifdef BUILD_TARGET
    auto iniPath = QStringLiteral("/var/opt/bosch/dynamic/navigation_kor/tmap_rsm_ui_dbg.ini");
#else
    auto iniPath = QDir::homePath() + QChar('/') + QStringLiteral("tmap_rsm_ui_dbg.ini");
#endif
    QFile file( iniPath );
    if (!file.exists()) {
        //qDebug() << "DebugUtils::handleSetting #1 iniPath: " << iniPath << " key: " << key;
    }
    else {
        QSettings settings( iniPath, QSettings::IniFormat );
        if ( settings.contains( key ) ) {
            auto value = settings.value( key );
            handler( value );
        }
    }
}

void DebugUtils::printMap(const QString &description, const QVariantMap &m)
{
    qDebug() << "printMap(" << description << ")";
    for (auto key : m.keys()) {
        qDebug() << "m[" << key << "] = " << m[key];
    }
    qDebug() << "-----------------------------------";
}

#define CUSTOM_MAP_THEME "tmap_rsm_map_theme_190319.ini"

void DebugUtils::setMapThemeValue(const QString& key, const QString& value)
{
#ifdef BUILD_TARGET
    auto iniPath = QStringLiteral("/var/opt/bosch/navdata/log/tmap_rsm_map_theme.ini");
#else
    auto iniPath = QDir::homePath() + QChar('/') + QStringLiteral(CUSTOM_MAP_THEME);
#endif
    QFile file( iniPath );
    QSettings settings( iniPath, QSettings::IniFormat );
    settings.setValue(key, value);
    settings.sync();
}

QString DebugUtils::getMapThemeValue(const QString& key)
{
#ifdef BUILD_TARGET
    auto iniPath = QStringLiteral("/var/opt/bosch/navdata/log//tmap_rsm_map_theme.ini");
#else
    auto iniPath = QDir::homePath() + QChar('/') + QStringLiteral(CUSTOM_MAP_THEME);
#endif
    QFile file( iniPath );
    QSettings settings( iniPath, QSettings::IniFormat );
    return settings.value(key).toString();
}

void DebugUtils::updateMapTheme(TestConstants::MapType mapType, QObject* renderer, const QString& item)
{
    QString cavatarItem;
    QString routeLineItem;
    QString routeLineTbtItem;

    if (mapType == TestConstants::CC) {
        cavatarItem = TestConstants::C_C_CS_C;
        routeLineItem = TestConstants::C_C_RLT;
        routeLineTbtItem = TestConstants::C_C_RLTT;
    }
    else if (mapType == TestConstants::CS) {
        cavatarItem = TestConstants::C_S_CS_C;
        routeLineItem = TestConstants::C_S_RLT;
        routeLineTbtItem = TestConstants::C_S_RLTT;
    }
    else if (mapType == TestConstants::D) {
        cavatarItem = TestConstants::D_CS_C;
        routeLineItem = TestConstants::D_RLT;
        routeLineTbtItem = TestConstants::D_RLTT;
    }
    else if (mapType == TestConstants::GXXL) {
        cavatarItem = TestConstants::GXXL_CS_C;
        routeLineItem = TestConstants::GXXL_RLT;
        routeLineTbtItem = TestConstants::GXXL_RLTT;
    }
    else if (mapType == TestConstants::GXL) {
        cavatarItem = TestConstants::GXL_CS_C;
        routeLineItem = TestConstants::GXL_RLT;
        routeLineTbtItem = TestConstants::GXL_RLTT;
    }
    else if (mapType == TestConstants::GL) {
        cavatarItem = TestConstants::GL_CS_C;
        routeLineItem = TestConstants::GL_RLT;
        routeLineTbtItem = TestConstants::GL_RLTT;
    }

    if (item == cavatarItem) {
        float newSize = 0;
        auto value = DebugUtils::getMapThemeValue(cavatarItem);
        if (value.isEmpty()) {
            return;
        }
        newSize = value.toFloat();
        qDebug() << "[UpdateMapTheme]C_C_CS_C: " << newSize;

        QJsonObject root;
        QJsonObject carvatar;
        carvatar["width"] = newSize;
        root["carvatar"] = carvatar;
        auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

        auto handle = qvariant_cast<void*>(renderer->property("nativeHandle"));

        if (mapType == TestConstants::CC || mapType == TestConstants::CS) {
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());
        }
        else {
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_DAY, json.data());
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_NIGHT, json.data());
        }
    }
    else if (item == routeLineItem) {
        QJsonObject routeLine;

        float newSize = 0;
        auto value = DebugUtils::getMapThemeValue(routeLineItem);
        if (value.isEmpty()) {
            return;
        }
        newSize = value.toFloat();

        routeLine["width"] = newSize;
        QJsonObject root;
        root["routeLine"] = routeLine;
        auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

        auto handle = qvariant_cast<void*>(renderer->property("nativeHandle"));
        if (mapType == TestConstants::CC || mapType == TestConstants::CS) {
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());
        }
        else {
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_DAY, json.data());
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_NIGHT, json.data());
        }

        qDebug() << "[UpdateMapTheme]C_C_RLT: " << newSize;
    }
    else if (item == routeLineTbtItem) {
        float newSize = 0;
        auto value = DebugUtils::getMapThemeValue(routeLineTbtItem);
        if (value.isEmpty()) {
            return;
        }
        newSize = value.toFloat();

        QJsonObject turnPoint;
        turnPoint["width"] = newSize;
        QJsonObject routeLine;
        routeLine["turnPoint"] = turnPoint;
        QJsonObject root;
        root["routeLine"] = routeLine;
        auto json = QJsonDocument(root).toJson(QJsonDocument::Compact);

        auto handle = qvariant_cast<void*>(renderer->property("nativeHandle"));
        if (mapType == TestConstants::CC || mapType == TestConstants::CS) {
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_CLUSTER, json.data());
        }
        else {
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_DAY, json.data());
            VSMMap_CreateOrUpdateThemeStyle(handle, VSM_THEME_NIGHT, json.data());
        }

        qDebug() << "[UpdateMapTheme]C_C_RLTT: " << newSize;
    }
}

QString DebugUtils::name(const QObjectPtr& obj)
{
    QString ret;

    if ( obj ) {
        QString objName = obj->objectName();
        if ( !objName.isEmpty() ) {
            ret = objName;
        }
        else {
            auto mediator = dynamic_cast<mvc::TMediator*>( obj.data() );
            if ( mediator != nullptr ) {
                auto comp = mediator->getViewComponent();
                if ( comp ) {
                    ret = comp->objectName();
                }
                else {
                    ret = mediator->metaObject()->className();
                }
            }
            else {
                ret = obj->metaObject()->className();
            }
        }
    }

    return ret;
}
