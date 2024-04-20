#pragma once

#include <QDomDocument>

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TXmlUtils
{
public:
	static void load( QDomDocument& doc, const QString& path );

	static void save( QDomDocument& doc, const QString& path );	

	static QDomElement find( const QDomDocument& doc, const QString& paths );

	static QDomElement find( const QDomDocument& doc, const QString& paths, const QString& name, const QString& value );
};

}
