#include "SettingDto.h"

using namespace SKT;

SettingDto::SettingDto()
{

}

SettingDto::SettingDto( const QVariant& type,
    const QVariant& section,
    const QVariant& key,
    const QVariant& value )
    : mType( type ),
      mSection( section ),
      mKey( key ),
      mValue( value )
{
}

QVariant SettingDto::id() const
{
    return mId;
}

void SettingDto::setId( const QVariant& value )
{
    mId = value;
}

QVariant SettingDto::type() const
{
    return mType;
}

void SettingDto::setType( const QVariant& value )
{
    mType = value;
}

QVariant SettingDto::section() const
{
    return mSection;
}

void SettingDto::setSection( const QVariant& value )
{
    mSection = value;
}

QVariant SettingDto::key() const
{
    return mKey;
}

void SettingDto::setKey( const QVariant& value )
{
    mKey = value;
}

QVariant SettingDto::value() const
{
    return mValue;
}

void SettingDto::setValue( const QVariant& value )
{    
    mValue = value;
}
