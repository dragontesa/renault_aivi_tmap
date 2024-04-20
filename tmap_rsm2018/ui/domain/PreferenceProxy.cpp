#include "PreferenceProxy.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"
#include "TSettings.h"
#include "LangConstants.h"

namespace SKT {

PreferenceProxy::PreferenceProxy( const QString& dirPath )
    : mDirPath( dirPath )
{
    // rg.ini
    auto settings = addPreferences( PreferenceConstants::Rg );    

    // 경로안내 정상종료 여부
    settings->addItem( "Termination", TSettingItem( TSettingItem::String, "address", "" ) );
    settings->addItem( "Termination", TSettingItem( TSettingItem::Bool, "isNormallyTerminated", true ) );
    settings->addItem( "Termination", TSettingItem( TSettingItem::Integer, "selectedRouteIndex", -1 ) );
    settings->addItem( "Termination", TSettingItem( TSettingItem::Integer, "selectedRouteOption", -1 ) );
    settings->addItem( "Termination", TSettingItem( TSettingItem::StringList, "via", "" ) );

    // test.ini
    settings = addPreferences( PreferenceConstants::Test );
    settings->addItem( "MonkeyTest", TSettingItem( TSettingItem::Bool, "enable", false ) );
    settings->addItem( "MonkeyTest", TSettingItem( TSettingItem::Integer, "repeatsPerSecond", 10 ) );
    settings->addItem( "MonkeyTest", TSettingItem( TSettingItem::Integer, "durationMins", 60 * 3 ) );
    settings->addItem( "Language", TSettingItem( TSettingItem::String, "lang", LangConstants::Ko ) );//QStringLiteral("ko") ) );

    settings = addPreferences( "Profiler" );
}

TSettingsPtr PreferenceProxy::addPreferences( const QString& prefsName )
{
    auto iniPath = mDirPath + QChar('/') + prefsName + QStringLiteral(".ini");

    auto settings = t_new_shared<TSettings>();
    settings->setIni( iniPath );

    mSettingsSet[prefsName] = settings;

    return settings;
}

void PreferenceProxy::removePreferences( const QString& prefsName )
{
    mSettingsSet.remove( prefsName );

    auto iniPath = mDirPath + QChar('/') + prefsName + QStringLiteral(".ini");

    QFile file( iniPath );
    file.remove();

}

PreferenceVo PreferenceProxy::preference( const QString& prefsName, const QString& section, const QString& key )
{
    PreferenceVo ret;

    auto prefs = mSettingsSet[prefsName];

    auto value  = prefs->value( section, key );
    ret.setSection( section );
    ret.setKey( key );
    ret.setValue( value );

    return ret;
}

QMap<QString,PreferenceVo>  PreferenceProxy::preferencesInSection( const QString& prefsName, const QString& section )
{
    QMap<QString,PreferenceVo>  ret;

    auto prefs = mSettingsSet[prefsName];

    auto keys = prefs->keys( section );
    for ( auto key : keys ) {
        PreferenceVo vo;

        auto value  = prefs->value( section, key );
        vo.setSection( section );
        vo.setKey( key );
        vo.setValue( value );

        ret[key] = vo;
    }

    return ret;
}

void PreferenceProxy::setPreference( const QString& prefsName, const QString& section, const QString& key, const QVariant& value )
{
    auto settings = mSettingsSet[prefsName];
    settings->setValue( section, key, value );
    settings->sync();
}

}

