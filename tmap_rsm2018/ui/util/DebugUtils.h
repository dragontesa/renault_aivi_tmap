#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#include "VsmMapSettingAndTheme.h"
#include "TestConstants.h"

//#if __has_include("__tmap_rsm_ui_dbg__")
//    #define TMAP_RSM_UI_DEBUG_SETTING_HANDLER(KEY,FUNC) DebugUtils::handleSetting(KEY,FUNC)
//#else
    #ifdef QT_DEBUG
        #define TMAP_RSM_UI_DEBUG_SETTING_HANDLER(KEY,FUNC) DebugUtils::handleSetting(KEY,FUNC)
    #else
        #define TMAP_RSM_UI_DEBUG_SETTING_HANDLER(KEY,FUNC)
    #endif
//#endif

namespace SKT {

class DebugUtils
{
public:
    static void handleSetting( const QString& key, const std::function<void(const QVariant& value)>& handler );
    static void printMap(const QString& description, const QVariantMap& m);

    static void setMapThemeValue(const QString& key, const QString& value);
    static QString getMapThemeValue(const QString& key);
    static void updateMapTheme(TestConstants::MapType mapType, QObject* renderer, const QString& item);

    static QString name(const QObjectPtr& obj);
};

};

#endif // DEBUGUTILS_H
