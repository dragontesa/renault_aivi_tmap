#include "TSettings.h"

/*

value
	ini에 없으면 def

setValue

*/

using namespace SKT;

TSettings::TSettings()
{	
}

TSettings::~TSettings()
{
}

void TSettings::setIni( const QString& path )
{
	QSharedPointer<QSettings> settings( new QSettings( path, QSettings::IniFormat ) );
	mSettings = settings;
}

void TSettings::addItem( const QString& section, const TSettingItem& item )
{
	if ( !mItems.contains( section ) ) {
		mItems[section] = QHash<QString,TSettingItem>();
	}

	mItems[section][item.name] = item;
}

QList<QString> TSettings::sections() const
{
	return mItems.uniqueKeys();
}

QList<QString> TSettings::keys( const QString& section ) const
{
	return mItems[section].uniqueKeys();
}

TSettingItem TSettings::item( const QString& section, const QString& name ) const
{
    return mItems[section][name];
}

QVariant TSettings::value( const QString& section, const QString& key ) const
{
    QVariant ret;

    ret = mSettings->value( section + QStringLiteral("/") + key, QVariant() );
    if ( ret.isNull() ) {
        ret = mItems[section][key].defValue;
    }

    return ret;
}

void TSettings::setValue( const QString& section, const QString& key, const QVariant& val )
{
    mSettings->setValue( section + QStringLiteral("/") + key, val );
}

QString TSettings::stringValue( const QString& section, const QString& key ) const
{
	return value( section, key ).toString();
}

void TSettings::setStringValue( const QString& section, const QString& key, const QString& val )
{
    mSettings->setValue( section + QStringLiteral("/") + key, val );
}

QStringList TSettings::stringListValue( const QString& section, const QString& key ) const
{
	return value( section, key ).toStringList();
}

void TSettings::setStringListValue( const QString& section, const QString& key, const QStringList& val )
{
    mSettings->setValue( section + QStringLiteral("/") + key, val );
}

int TSettings::intValue( const QString& section, const QString& key ) const
{
	return value( section, key ).toInt();
}

void TSettings::setIntValue( const QString& section, const QString& key, int val )
{
	mSettings->setValue( section + "/" + key, val );
}

bool TSettings::boolValue( const QString& section, const QString& key ) const
{
	return value( section, key ).toBool();
}

void TSettings::setBoolValue( const QString& section, const QString& key, bool val )
{
    mSettings->setValue( section + QStringLiteral("/") + key, val );
}

QList<QString> TSettings::childKeys( const QString& group ) const
{
	mSettings->beginGroup( group);
	auto keys = mSettings->childKeys();
	mSettings->endGroup();

	return keys;
}

void TSettings::sync()
{
    mSettings->sync();
}
