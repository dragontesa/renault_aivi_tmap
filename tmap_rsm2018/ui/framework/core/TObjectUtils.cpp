#include "TObjectUtils.h"

using namespace SKT;

TObjectUtils::TObjectUtils()
{
}


TObjectUtils::~TObjectUtils()
{
}

QString TObjectUtils::metaAttribute( const QMetaObject* mo, const QString& wantedName, const QString& wantedKey )
{
    QString ret;

    auto pos = mo->indexOfClassInfo( wantedName.T_CSTR() );
    auto classInfo = mo->classInfo( pos );
    auto attrs = QString( classInfo.value() ).split( "," );
    for ( int j = 0, count = attrs.count(); j < count; j++ ) {
        auto key = attrs.at( j ).section( "=", 0, 0 ).trimmed();
        auto value = attrs.at( j ).section( "=", 1, 1 ).trimmed();
        if ( key == wantedKey ) {
            ret = value;
            break;
        }
    }

    if ( ret.isEmpty() ) {
    }

    return ret;
}

bool TObjectUtils::checkMetaAttribute( const QMetaObject* mo, const QString& wantedName, const QString& wantedKey, const QString& wantedValue )
{
    bool ret = false;

    auto temp = metaAttribute( mo, wantedName, wantedKey );
    if ( temp == wantedValue ) {
        ret = true;
    }

    return ret;
}

