#include "PreferenceVo.h"

using namespace SKT;

PreferenceVo::PreferenceVo()
{

}

PreferenceVo::PreferenceVo( const QString& section,
    const QString& key,
    const QVariant& value )
    : mSection( section ),
      mKey( key ),
      mValue( value )	
{
}

PreferenceVo::PreferenceVo( const PreferenceVo& other )
    : mSection( other.mSection ),
    mKey( other.mKey ),
    mValue( other.mValue )
{

}

PreferenceVo& PreferenceVo::operator=( const PreferenceVo& other )
{
    mSection = other.mSection;
    mKey = other.mKey;
    mValue = other.mValue;

    return *this;
}

QString PreferenceVo::section() const
{
    return mSection;
}

void PreferenceVo::setSection( const QString& value )
{
    mSection = value;
}

QString PreferenceVo::key() const
{
    return mKey;
}

void PreferenceVo::setKey( const QString& value )
{
    mKey = value;
}

QVariant PreferenceVo::value() const
{
    return mValue;
}

void PreferenceVo::setValue( const QVariant& value )
{    
    mValue = value;
}

