#ifndef NAVIGATIONSDSSERVICETYPE_H
#define NAVIGATIONSDSSERVICETYPE_H

#include <QDBusArgument>
#include <float.h>
#include <math.h>
#include "../navigation/navigationservicetype.h"

/**
 * @brief The NSPositionStatusInfo struct
 */
struct NSPositionStatusInfo {
    NSPositionStatusInfo() : m_roadClass(RoadClass_ROAD_CLASS_NONE), m_vehicleSpeed(0), m_vehicleHeading(0) { }
    NSPositionWGS84 m_positionWGS84;
    QString m_countryCode;
    QString m_stateCode;
    uint m_roadClass;
    double m_vehicleSpeed;
    double m_vehicleHeading;

    bool operator==(const NSPositionStatusInfo &other) const {
        if (m_positionWGS84 == other.m_positionWGS84 &&
                m_countryCode == other.m_countryCode &&
                m_stateCode == other.m_stateCode &&
                m_roadClass == other.m_roadClass &&
                (fabs(m_vehicleSpeed - other.m_vehicleSpeed) <= DBL_EPSILON) &&
                (fabs(m_vehicleHeading - other.m_vehicleHeading) <= DBL_EPSILON)) {
                //m_vehicleSpeed == other.m_vehicleSpeed &&
                //m_vehicleHeading == other.m_vehicleHeading) {
            return true;
        }
        return false;
    }

    bool operator!=(const NSPositionStatusInfo &other) const {
        return !(*this == other);
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionStatusInfo& info);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionStatusInfo& info);

Q_DECLARE_METATYPE(NSPositionStatusInfo)


/**
 * @brief The NDSNamedObjectData struct
 */
struct NDSNamedObjectData {
    NDSNamedObjectData() : m_poiCategoryId(0), m_poiCategoryVisibilityOnMap(false) { }
    QString m_poiCategoryName;
    uint m_poiCategoryId;
    ushort m_poiCategoryVisibilityOnMap;

    bool operator==(const NDSNamedObjectData &other) const {
        if (m_poiCategoryName == other.m_poiCategoryName &&
                m_poiCategoryId == other.m_poiCategoryId &&
                m_poiCategoryVisibilityOnMap == other.m_poiCategoryVisibilityOnMap) {
            return true;
        }
        return false;
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NDSNamedObjectData& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NDSNamedObjectData& data);

Q_DECLARE_METATYPE(NDSNamedObjectData);


/**
 * @brief The NSPositionInfo struct
 */
struct NSPositionInfo {
    QString m_country;
    QString m_province;
    QString m_city;
    QString m_cityDistrict;
    QString m_street;
    QString m_streetNumber;
    NDSNamedObjectData m_currentCountry;
    NDSNamedObjectData m_currentState;
    NDSNamedObjectData m_currentCity;
    NDSNamedObjectData m_currentCityDistrict;
    NDSNamedObjectData m_currentStreet;
    NDSNamedObjectData m_currentIntersection;
    NDSNamedObjectData m_currentZipCode;

    bool operator==(const NSPositionInfo &other) const {
        if (m_country == other.m_country &&
                m_province == other.m_province &&
                m_city == other.m_city &&
                m_cityDistrict == other.m_cityDistrict &&
                m_street == other.m_street &&
                m_streetNumber == other.m_streetNumber &&
                m_currentCountry == other.m_currentCountry &&
                m_currentState == other.m_currentState &&
                m_currentCity == other.m_currentCity &&
                m_currentCityDistrict == other.m_currentCityDistrict &&
                m_currentStreet == other.m_currentStreet &&
                m_currentIntersection == other.m_currentIntersection &&
                m_currentZipCode == other.m_currentZipCode) {
            return true;
        }
        return false;
    }

    bool operator!=(const NSPositionInfo &other) const {
        return !(*this == other);
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionInfo& info);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionInfo& info);

Q_DECLARE_METATYPE(NSPositionInfo);



/**
 * @brief The NavigationSDSServiceType class
 */
class NavigationSDSServiceType
{
public:
    static void registerMetaTypes();
};

#endif // NAVIGATIONSDSSERVICETYPE_H
