#include "TStringUtils.h"

using namespace SKT;

QString TStringUtils::format( const char* format, ... )
{
	QString ret;

	va_list ap;
	va_start( ap, format );

	ret.vsprintf( format, ap );

	va_end( ap );

	return ret;
}

QString TStringUtils::toString( int value, bool toLiteral )
{
	QString ret;

	if ( toLiteral ) {
		ret.append( '\'' );
	}

	ret = QLocale::c().toString( value );

	if ( toLiteral ) {
		ret.append( '\'' );
	}

	return ret;
}

QString TStringUtils::randomString( int length )
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	QString ret;

	for ( int i=0; i < length; ++i ) {
		ret.append(alphanum[qrand() % (sizeof(alphanum) - 1)]);
	}
	return ret;
}

QPoint TStringUtils::toPoint( const QString& s )
{
	QPoint ret;

	auto temp = s.split( "," );
	ret.setX( temp[0].toInt() );
	ret.setY( temp[1].toInt() );

	return ret;
}



QString TStringUtils::charArrayToHexString( const char* data, int len, bool withAscii )
{
    QString ret;

    QString hex;
    QString ascii;

    char const xchars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    for( int i = 0; i < len; ++i ) {
        char byte = data[i];

        hex.append( xchars[ ( byte & 0xF0 ) >> 4 ] );
        hex.append( xchars[ ( byte & 0x0F ) >> 0 ] );
        if ( i < len - 1 ) {
            hex.append( ' ' );
        }

        QChar ch = data[i];
        if ( !ch.isLetterOrNumber() ) {
            ascii.append( '.' );
        }
        else {
            ascii.append( ch );
        }
    }

    ret.append( hex );
    ret.append( ' ' );
    ret.append( ascii );

    return ret;
}

QString TStringUtils::stringListToBase64(const QStringList &sl)
{
    QByteArray ba;
    QDataStream out(&ba, QIODevice::WriteOnly);
    out << sl;

    return ba.toBase64();
}

QStringList TStringUtils::stringListFromBase64(const QString &s)
{
    QByteArray ba = QByteArray::fromBase64(s.toUtf8());
    QDataStream in(&ba, QIODevice::ReadOnly);
    QStringList sl;
    in >> sl;

    return sl;
}
