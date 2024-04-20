#pragma once

namespace SKT
{

class TMAP_RSM_UI_DLLSHARED_EXPORT TStringUtils
{
public:
	static QString format( const char* format, ... );

	static QString toString( int value, bool toLiteral = false );

	static QString randomString( int length );

	static QPoint toPoint( const QString& s );

    static QString charArrayToHexString( const char* data, int len, bool withAscii = true );

    static QString stringListToBase64(const QStringList &sl);

    static QStringList stringListFromBase64(const QString &s);
};

}
