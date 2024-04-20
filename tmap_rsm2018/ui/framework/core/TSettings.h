#pragma once

// TSettings
// QSettings...

namespace SKT
{

struct TMAP_RSM_UI_DLLSHARED_EXPORT TSettingItem {
	enum Type {
        String = 0,
		Bool,
		Integer,
		Float,
		Color,
		Directory,
		File,
        Custom,
        StringList
	};

	Type type;
	QString name;
	QVariant defValue;

	TSettingItem() :
		type( String ) {
	}

	TSettingItem( Type type, const QString& name, const QVariant& defValue = QString() ) 
		: type( type ), name( name ), defValue( defValue ) {
	}
};

T_SHAREABLE( TSettings );

class TMAP_RSM_UI_DLLSHARED_EXPORT TSettings : public QObject
{
	Q_OBJECT

public:
	TSettings();

	~TSettings();

	// ini

	void setIni( const QString& path );

	// seiton / item

    void addItem( const QString& section, const TSettingItem& item );

	QList<QString> sections() const;

    QList<QString> keys( const QString& section ) const;

    TSettingItem item( const QString& section, const QString& name ) const;

	// value

    QVariant value( const QString& section, const QString& key ) const;

    void setValue( const QString& section, const QString& key, const QVariant& val );

	QString stringValue( const QString& section, const QString& key ) const;

	void setStringValue( const QString& section, const QString& key, const QString& val );

	QStringList stringListValue( const QString& section, const QString& key ) const;

	void setStringListValue( const QString& section, const QString& key, const QStringList& val );

	int intValue( const QString& section, const QString& key ) const;

	void setIntValue( const QString& section, const QString& key, int val );

	bool boolValue( const QString& section, const QString& key ) const;

	void setBoolValue( const QString& section, const QString& key, bool val );

	QList<QString> childKeys( const QString& group ) const;

    void sync();

private:    

	QHash<QString,QHash<QString,TSettingItem> > mItems;
	QSharedPointer<QSettings> mSettings;
};

}
