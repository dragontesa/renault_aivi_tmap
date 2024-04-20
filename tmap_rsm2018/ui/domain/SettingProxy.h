#ifndef SETTINGPROXY_H
#define SETTINGPROXY_H

#include "AbstractProxy.h"

namespace SKT {

T_SHAREABLE( TSettings );
T_SHAREABLE( SettingProxy );
T_SHAREABLE( SettingDto );
class TSettingItem;
class SettingProxy : public AbstractProxy {
    Q_OBJECT
public:
    SettingProxy();

    QList<SettingDtoPtr> settings( const QString& section );

    void setSettings( const QString& section, const QList<SettingDtoPtr>& values );

    SettingDtoPtr setting( const QString& section, const QString& key );
    void setSetting( const QString& section, const SettingDtoPtr& value );
    void setSetting( const QString& section, const QString& key,  const QString& value);
    void initialization();
    QHash<QString,QHash<QString,QHash<int,QVariant>> > getItems();

protected:
    bool addItem(const QString& section, const QString& key, const int& type, const QVariant& value);

private:
    void onRegister();

    void onRemove();

    TSettingsPtr mSettings;
    QHash<QString,QHash<QString,QHash<int,QVariant>> > mItems;
};

}

#endif // SETTINGPROXY_H
