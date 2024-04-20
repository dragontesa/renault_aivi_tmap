#include <QMetaType>
#include <QDBusMetaType>
#include "navigationsdsservicetype.h"

void NavigationSDSServiceType::registerMetaTypes()
{
    qDBusRegisterMetaType<NSPositionStatusInfo>();
    qDBusRegisterMetaType<NDSNamedObjectData>();
    qDBusRegisterMetaType<NSPositionInfo>();
}


QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionStatusInfo& info)
{
    argument.beginStructure();
    argument << info.m_positionWGS84;
    argument << info.m_countryCode;
    argument << info.m_stateCode;
    argument << info.m_roadClass;
    argument << info.m_vehicleSpeed;
    argument << info.m_vehicleHeading;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionStatusInfo& info)
{
    argument.beginStructure();
    argument >> info.m_positionWGS84;
    argument >> info.m_countryCode;
    argument >> info.m_stateCode;
    argument >> info.m_roadClass;
    argument >> info.m_vehicleSpeed;
    argument >> info.m_vehicleHeading;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NDSNamedObjectData &data)
{
    argument.beginStructure();
    argument << data.m_poiCategoryName;
    argument << data.m_poiCategoryId;
    argument << data.m_poiCategoryVisibilityOnMap;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NDSNamedObjectData &data)
{
    argument.beginStructure();
    argument >> data.m_poiCategoryName;
    argument >> data.m_poiCategoryId;
    argument >> data.m_poiCategoryVisibilityOnMap;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionInfo &info)
{
    argument.beginStructure();
    argument << info.m_country;
    argument << info.m_province;
    argument << info.m_city;
    argument << info.m_cityDistrict;
    argument << info.m_street;
    argument << info.m_streetNumber;
    argument << info.m_currentCountry;
    argument << info.m_currentState;
    argument << info.m_currentCity;
    argument << info.m_currentCityDistrict;
    argument << info.m_currentStreet;
    argument << info.m_currentIntersection;
    argument << info.m_currentZipCode;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionInfo &info)
{
    argument.beginStructure();
    argument >> info.m_country;
    argument >> info.m_province;
    argument >> info.m_city;
    argument >> info.m_cityDistrict;
    argument >> info.m_street;
    argument >> info.m_streetNumber;
    argument >> info.m_currentCountry;
    argument >> info.m_currentState;
    argument >> info.m_currentCity;
    argument >> info.m_currentCityDistrict;
    argument >> info.m_currentStreet;
    argument >> info.m_currentIntersection;
    argument >> info.m_currentZipCode;
    argument.endStructure();
    return argument;
}
