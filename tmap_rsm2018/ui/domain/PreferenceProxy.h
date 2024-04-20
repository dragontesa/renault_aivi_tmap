#ifndef PREFERENCEPROXY_H
#define PREFERENCEPROXY_H

#include "AbstractProxy.h"
#include "PreferenceVo.h"

namespace SKT {

T_SHAREABLE( PreferenceProxy );
T_SHAREABLE( TSettings );

class PreferenceProxy : public AbstractProxy
{
    Q_OBJECT
public:
    PreferenceProxy( const QString& dirPath );

    TSettingsPtr addPreferences( const QString& prefsName );
    void removePreferences( const QString& prefsName );

    //void addItem( const QString& prefsName, const QString& section, int type, const QString& key, const QVariant& value);

    PreferenceVo preference( const QString& prefsName, const QString& section, const QString& key );
    QMap<QString,PreferenceVo> preferencesInSection( const QString& prefsName, const QString& section );
    void setPreference( const QString& prefsName, const QString& section, const QString& key, const QVariant& value );

private:
    QString mDirPath;
    QMap<QString,TSettingsPtr> mSettingsSet;
};

}

#endif // PREFERENCEPROXY_H

