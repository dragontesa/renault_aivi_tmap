#include "TXmlUtils.h"

using namespace SKT;

void TXmlUtils::load( QDomDocument& doc, const QString& path )
{
	QFile file( path );

	QString errorStr;
	int errorLine;
	int errorColumn;

	if ( !doc.setContent( &file, true, &errorStr, &errorLine, &errorColumn ) ) {
        T_THROW( QString( "'%1' Parse error at line %2, column %3:\n%4" )
            .arg( path )
            .arg( errorLine )
            .arg( errorColumn )
            .arg( errorStr ) );
	}
}

void TXmlUtils::save( QDomDocument& doc, const QString& path )
{
	QFile file( path );
	if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
		T_THROW( "file not found" );
	}
	QTextStream textStream( &file );
	textStream << doc.toString();
}

QDomElement TXmlUtils::find( const QDomDocument& doc, const QString& paths )
{
	QDomElement ret;

	// 경로대로 다 찾고
	auto pathList = paths.split( "/" );

	auto elem = doc.firstChildElement( pathList.at( 0 ) );
	if ( elem.isNull() ) {
        T_THROW( QString( "could not find the tag named '%1'" ).arg( pathList.at( 0 ) ) );
	}
	for ( int i = 1; i < pathList.size(); i++ ) {		
		elem = elem.firstChildElement( pathList.at( i ) );
		if ( elem.isNull() ) {
            T_THROW( QString( "could not find the tag named '%1'" ).arg( pathList.at( i ) ) );
		}
	}
	
	ret = elem;

	return ret;
}

QDomElement TXmlUtils::find( const QDomDocument& doc, const QString& paths, const QString& name, const QString& value )
{
	QDomElement ret;

	// 경로대로 다 찾고
	auto elem = find( doc, paths );

	bool found = false;
	while ( !elem.isNull() ) {
		if ( elem.firstChildElement( name ).text() == value ) {
			found = true;
			ret = elem;
			break;
		}

		elem = elem.nextSiblingElement();
	}

	if ( !found ) {
		T_THROW( "node not found" );
	}

	return ret;
}
