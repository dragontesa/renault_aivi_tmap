#include <QMetaType>
#include <QDBusMetaType>
#include "navigationservicetype.h"

void NavigationServiceType::registerMetaTypes()
{
    qDBusRegisterMetaType<NSAddressListElement>();
    qDBusRegisterMetaType<NSAddressWithIdInfos>();
    qDBusRegisterMetaType<NSArrivalInformation>();
    qDBusRegisterMetaType<NSArrivalInformationList>();
    qDBusRegisterMetaType<NSBarGraphInfo>();
    qDBusRegisterMetaType<NSCurrentFreeTextSearchInfo>();
    qDBusRegisterMetaType<NSDeformedImage>();
    qDBusRegisterMetaType<NSDestinationInfoElement>();
    qDBusRegisterMetaType<NSDestinationInfoElements>();
    qDBusRegisterMetaType<NSDestinationMemoryDetailListElement>();
    qDBusRegisterMetaType<NSDestinationMemoryDetailsList>();
    qDBusRegisterMetaType<NSDestinationMemoryEntry>();
    qDBusRegisterMetaType<NSDestinationMemoryEntryDetails>();
    qDBusRegisterMetaType<NSDestinationMemoryEntryItem>();
    qDBusRegisterMetaType<NSDestinationMemoryEntryItemList>();
    qDBusRegisterMetaType<NSDistanceToDestination>();
    qDBusRegisterMetaType<NSGlobalNamedObjectId>();
    qDBusRegisterMetaType<NSGlobalUpdateRegionId>();
    qDBusRegisterMetaType<NSHomeLocation>();
    qDBusRegisterMetaType<NSImage>();
    qDBusRegisterMetaType<NSIntersectionMap>();
    qDBusRegisterMetaType<NSIntersectionMapCarsorPosition>();
    qDBusRegisterMetaType<NSIntersectionMapRoadLine>();
    qDBusRegisterMetaType<NSIntersectionMapRoadLineList>();
    qDBusRegisterMetaType<NSLaneRecommendation>();
    qDBusRegisterMetaType<NSLaneRecommendationList>();
    qDBusRegisterMetaType<NSTrafficEnforcement>();
    qDBusRegisterMetaType<NSLocation>();
    qDBusRegisterMetaType<NSLocationDetails>();
    qDBusRegisterMetaType<NSManeuverView>();
    qDBusRegisterMetaType<NSNextManeuverDetails>();
    qDBusRegisterMetaType<NSPoiCategoryShortcut>();
    qDBusRegisterMetaType<NSPoiCategoryShortcuts>();
    qDBusRegisterMetaType<NSPOILocation>();
    qDBusRegisterMetaType<NSPOILocations>();
    qDBusRegisterMetaType<NSPositionInformation>();
    qDBusRegisterMetaType<NSPositionWGS84>();
    qDBusRegisterMetaType<NSRefinementListElement>();
    qDBusRegisterMetaType<NSRefinementWithIdInfos>();
    qDBusRegisterMetaType<NSRouteCalculationDuration>();
    qDBusRegisterMetaType<NSSatelliteInfo>();
    qDBusRegisterMetaType<NSScreenCoordinate>();
    qDBusRegisterMetaType<NSScreenCoordinateList>();
    qDBusRegisterMetaType<NSSDSAddressElement>();
    qDBusRegisterMetaType<NSSDSAddress>();
    qDBusRegisterMetaType<NSSDSAddressWithOptions>();
    qDBusRegisterMetaType<NSSDSElementOptions>();
    qDBusRegisterMetaType<NSSDSVoiceTagId>();
    qDBusRegisterMetaType<NSSpeedLimit>();
    qDBusRegisterMetaType<NSTourElement>();
    qDBusRegisterMetaType<NSTour>();
    qDBusRegisterMetaType<NSTrafficServiceReceptionStatus>();
    qDBusRegisterMetaType<NSWaypointListElement>();
    qDBusRegisterMetaType<NSWaypointList>();
    qDBusRegisterMetaType<NSWorkLocation>();
    qDBusRegisterMetaType<NSMapVideoStreamDimensions>();
    qDBusRegisterMetaType<NSMapVideoStreamResult>();
    qDBusRegisterMetaType<NSNavDataPartNumber>();
    qDBusRegisterMetaType<NSTurnToStreet>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSMapVideoStreamDimensions &data)
{
    argument.beginStructure();
    argument << data.m_width;
    argument << data.m_height;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSMapVideoStreamDimensions &data)
{
    argument.beginStructure();
    argument >> data.m_width;
    argument >> data.m_height;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSTurnToStreet &data)
{
    argument.beginStructure();
    argument << data.m_maneuverId;
    argument << data.m_turnToStreet;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSTurnToStreet &data)
{
    argument.beginStructure();
    argument >> data.m_maneuverId;
    argument >> data.m_turnToStreet;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSMapVideoStreamResult &data)
{
    argument.beginStructure();
    argument << data.m_mapVideoStreamStatus;
    argument << data.m_mapVideoStreamHandle;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSMapVideoStreamResult &data)
{
    argument.beginStructure();
    argument >> data.m_mapVideoStreamStatus;
    argument >> data.m_mapVideoStreamHandle;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSGlobalUpdateRegionId& data)
{
    argument.beginStructure();
    argument << data.m_updateRegionId;
    argument << data.m_productId;
    argument << data.m_supplierId;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSGlobalUpdateRegionId &data)
{
    argument.beginStructure();
    argument >> data.m_updateRegionId;
    argument >> data.m_productId;
    argument >> data.m_supplierId;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSGlobalNamedObjectId& data)
{
    argument.beginStructure();
    argument << data.m_globalUpdateRegionId;
    argument << data.m_namedObjectId;
    argument << data.m_isValid;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSGlobalNamedObjectId &data)
{
    argument.beginStructure();
    argument >> data.m_globalUpdateRegionId;
    argument >> data.m_namedObjectId;
    argument >> data.m_isValid;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSAddressListElement &data)
{
    argument.beginStructure();
    argument << data.m_data;
    argument << data.m_id;
    argument << data.m_distance;
    argument << data.m_address;
    argument << data.m_direction;
    argument << data.m_coordinates;
    argument << data.m_sdsNameObjectId;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSAddressListElement &data)
{
    argument.beginStructure();
    argument >> data.m_data;
    argument >> data.m_id;
    argument >> data.m_distance;
    argument >> data.m_address;
    argument >> data.m_direction;
    argument >> data.m_coordinates;
    argument >> data.m_sdsNameObjectId;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSArrivalInformation &ai)
{
    argument.beginStructure();
    argument << ai.m_distanceToDestination;
    argument << ai.m_timeToDestinationAsString;
    argument << ai.m_timeToDestination;
    argument << ai.m_estimatedTimeOfArrivalAsString;
    argument << ai.m_estimatedTimeOfArrivalHour;
    argument << ai.m_estimatedTimeOfArrivalMinute;
    argument << ai.m_estimatedTimeType;
    argument << ai.m_displayTimeType;
    argument << ai.m_displayTimeFormat;
    argument << ai.m_destinationIndex;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSArrivalInformation &ai)
{
    argument.beginStructure();
    argument >> ai.m_distanceToDestination;
    argument >> ai.m_timeToDestinationAsString;
    argument >> ai.m_timeToDestination;
    argument >> ai.m_estimatedTimeOfArrivalAsString;
    argument >> ai.m_estimatedTimeOfArrivalHour;
    argument >> ai.m_estimatedTimeOfArrivalMinute;
    argument >> ai.m_estimatedTimeType;
    argument >> ai.m_displayTimeType;
    argument >> ai.m_displayTimeFormat;
    argument >> ai.m_destinationIndex;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSBarGraphInfo &bgi)
{
    argument.beginStructure();
    argument << bgi.m_valid;
    argument << bgi.m_value;
    argument << bgi.m_scale;
    argument << bgi.m_distanceUnit;
    argument << bgi.m_barGraphMaxScale;
    argument << bgi.m_barGraphMinScale;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSBarGraphInfo &bgi)
{
    argument.beginStructure();
    argument >> bgi.m_valid;
    argument >> bgi.m_value;
    argument >> bgi.m_scale;
    argument >> bgi.m_distanceUnit;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSCurrentFreeTextSearchInfo &info)
{
    argument.beginStructure();
    argument << info.m_position;
    argument << info.m_searchString;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSCurrentFreeTextSearchInfo &info)
{
    argument.beginStructure();
    argument >> info.m_position;
    argument >> info.m_searchString;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDeformedImage &data)
{
    argument.beginStructure();
    argument << data.m_valid;
    argument << data.m_deformedImageRoadType;
    argument << data.m_deformedImageArrowType;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDeformedImage &data)
{
    argument.beginStructure();
    argument >> data.m_valid;
    argument >> data.m_deformedImageRoadType;
    argument >> data.m_deformedImageArrowType;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationInfoElement &info)
{
    argument.beginStructure();
    argument << info.m_location;
    argument << info.m_countryCode;
    argument << info.m_subCountryCode;
    argument << info.m_name;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationInfoElement &info)
{
    argument.beginStructure();
    argument >> info.m_location;
    argument >> info.m_countryCode;
    argument >> info.m_subCountryCode;
    argument >> info.m_name;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryDetailListElement &data)
{
    argument.beginStructure();
    argument << data.m_entry;
    argument << data.m_details;
    argument << data.m_coordinates;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryDetailListElement &data)
{
    argument.beginStructure();
    argument >> data.m_entry;
    argument >> data.m_details;
    argument >> data.m_coordinates;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryEntry &data)
{
    argument.beginStructure();
    argument << data.m_name;
    argument << data.m_entryID;
    argument << data.m_voiceTagId;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryEntry &data)
{
    argument.beginStructure();
    argument >> data.m_name;
    argument >> data.m_entryID;
    argument >> data.m_voiceTagId;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryEntryItem& data)
{
    argument.beginStructure();
    argument << data.m_itemString;
    argument << data.m_addressType;
    argument << data.m_itemPhoneme;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryEntryItem &data)
{
    argument.beginStructure();
    argument >> data.m_itemString;
    argument >> data.m_addressType;
    argument >> data.m_itemPhoneme;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDestinationMemoryEntryDetails &data)
{
    argument.beginStructure();
    argument << data.m_countryName;
    argument << data.m_stateName;
    argument << data.m_cityName;
    argument << data.m_streetName;
    argument << data.m_houseno;
    argument << data.m_junctionName;
    argument << data.m_poiName;
    argument << data.m_poiCategoryName;
    argument << data.m_poiBrandName;
    argument << data.m_countryCode;
    argument << data.m_addressDetail;
    argument << data.m_sdsDestinationMemoryEntries;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDestinationMemoryEntryDetails &data)
{
    argument.beginStructure();
    argument >> data.m_countryName;
    argument >> data.m_stateName;
    argument >> data.m_cityName;
    argument >> data.m_streetName;
    argument >> data.m_houseno;
    argument >> data.m_junctionName;
    argument >> data.m_poiName;
    argument >> data.m_poiCategoryName;
    argument >> data.m_poiBrandName;
    argument >> data.m_countryCode;
    argument >> data.m_addressDetail;
    argument >> data.m_sdsDestinationMemoryEntries;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSDistanceToDestination &distanceToDestination)
{
    argument.beginStructure();
    argument << distanceToDestination.m_distanceToDestinationAsString;
    argument << distanceToDestination.m_distanceToDestination;
    argument << distanceToDestination.m_distanceUnit;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSDistanceToDestination &distanceToDestination)
{
    argument.beginStructure();
    argument >> distanceToDestination.m_distanceToDestinationAsString;
    argument >> distanceToDestination.m_distanceToDestination;
    argument >> distanceToDestination.m_distanceUnit;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSHomeLocation &loc)
{
    argument.beginStructure();
    argument << loc.m_name;
    argument << loc.m_positionWGS84;
    argument << loc.m_valid;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSHomeLocation &loc)
{
    argument.beginStructure();
    argument >> loc.m_name;
    argument >> loc.m_positionWGS84;
    argument >> loc.m_valid;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSImage &data)
{
    argument.beginStructure();
    argument << data.m_image;
    argument << data.m_imageType;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSImage &data)
{
    argument.beginStructure();
    argument >> data.m_image;
    argument >> data.m_imageType;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSIntersectionMap &data)
{
    argument.beginStructure();
    argument << data.m_road;
    argument << data.m_route;
    argument << data.m_noEntryIcons;
    argument << data.m_intersectionNumber;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSIntersectionMap &data)
{
    argument.beginStructure();
    argument >> data.m_road;
    argument >> data.m_route;
    argument >> data.m_noEntryIcons;
    argument >> data.m_intersectionNumber;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSIntersectionMapCarsorPosition &data)
{
    argument.beginStructure();
    argument << data.m_intersectionMapId;
    argument << data.m_carsorState;
    argument << data.m_carsorPosition;
    argument << data.m_carsorAngle;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSIntersectionMapCarsorPosition &data)
{
    argument.beginStructure();
    argument >> data.m_intersectionMapId;
    argument >> data.m_carsorState;
    argument >> data.m_carsorPosition;
    argument >> data.m_carsorAngle;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSIntersectionMapRoadLine &data)
{
    argument.beginStructure();
    argument << data.m_coordinates;
    argument << data.m_roadClass;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSIntersectionMapRoadLine &data)
{
    argument.beginStructure();
    argument >> data.m_coordinates;
    argument >> data.m_roadClass;
    argument.endStructure();
    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const NSTrafficEnforcement &data)
{
    argument.beginStructure();
    argument << data.m_TrafficEnforcementAlertState;
    argument << data.m_trafficEnforcementCameraType;
    argument << data.m_distance;
    argument << data.m_distanceUnit;
    argument << data.m_trafficEnforcementDistanceType;
    argument << data.m_speedLimit;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSTrafficEnforcement &data)
{
    argument.beginStructure();
    argument >> data.m_TrafficEnforcementAlertState;
    argument >> data.m_trafficEnforcementCameraType;
    argument >> data.m_distance;
    argument >> data.m_distanceUnit;
    argument >> data.m_trafficEnforcementDistanceType;
    argument >> data.m_speedLimit;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSLaneRecommendation &data)
{
    argument.beginStructure();
    argument << data.m_laneStatus;
    argument << data.m_combinedFollowingLaneDirection;
    argument << data.m_guidedFollowingLaneDirection;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSLaneRecommendation &data)
{
    argument.beginStructure();
    argument >> data.m_laneStatus;
    argument >> data.m_combinedFollowingLaneDirection;
    argument >> data.m_guidedFollowingLaneDirection;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSLocation &data)
{
    argument.beginStructure();
    argument << data.m_name;
    argument << data.m_positionWGS84;
    argument << data.m_valid;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSLocation &data)
{
    argument.beginStructure();
    argument >> data.m_name;
    argument >> data.m_positionWGS84;
    argument >> data.m_valid;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSLocationDetails &data)
{
    argument.beginStructure();
    argument << data.m_cityName;
    argument << data.m_countryName;
    argument << data.m_countryCode;
    argument << data.m_stateName;
    argument << data.m_stateCode;
    argument << data.m_phoneNumber;
    argument << data.m_poiAddress;
    argument << data.m_streetName;
    argument << data.m_zipCode;
    argument << data.m_coordinate;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSLocationDetails &data)
{
    argument.beginStructure();
    argument >> data.m_cityName;
    argument >> data.m_countryName;
    argument >> data.m_countryCode;
    argument >> data.m_stateName;
    argument >> data.m_stateCode;
    argument >> data.m_phoneNumber;
    argument >> data.m_poiAddress;
    argument >> data.m_streetName;
    argument >> data.m_zipCode;
    argument >> data.m_coordinate;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSManeuverView &data)
{
    argument.beginStructure();
    argument << data.m_intersectionMapId;
    argument << data.m_maneuverViewType;
    argument << data.m_deformedImage;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSManeuverView &data)
{
    argument.beginStructure();
    argument >> data.m_intersectionMapId;
    argument >> data.m_maneuverViewType;
    argument >> data.m_deformedImage;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSNextManeuverDetails &data)
{
    argument.beginStructure();
    argument << data.m_distanceToManeuver;
    argument << data.m_distanceToManeuverUnit;
    argument << data.m_recommendationType;
    argument << data.m_isNewManeuver;
    argument << data.m_isReRouted;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSNextManeuverDetails &data)
{
    argument.beginStructure();
    argument >> data.m_distanceToManeuver;
    argument >> data.m_distanceToManeuverUnit;
    argument >> data.m_recommendationType;
    argument >> data.m_isNewManeuver;
    argument >> data.m_isReRouted;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSPoiCategoryShortcut &data)
{
    argument.beginStructure();
    argument << data.m_categoryName;
    argument << data.m_poiCategoryId;
    argument << data.m_icon;
    argument << data.m_shortcutIndex;
    argument << data.m_sdsNameObjectId;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPoiCategoryShortcut &data)
{
    argument.beginStructure();
    argument >> data.m_categoryName;
    argument >> data.m_poiCategoryId;
    argument >> data.m_icon;
    argument >> data.m_shortcutIndex;
    argument >> data.m_sdsNameObjectId;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSPOILocation &data)
{
    argument.beginStructure();
    argument << data.m_location;
    argument << data.m_phoneNumber;
    argument << data.m_address;
    argument << data.m_icon;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPOILocation &data)
{
    argument.beginStructure();
    argument >> data.m_location;
    argument >> data.m_phoneNumber;
    argument >> data.m_address;
    argument >> data.m_icon;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionInformation &data)
{
    argument.beginStructure();
    argument << data.m_positionWGS84;
    argument << data.m_mapMatchingPositionAccuracy;
    argument << data.m_countryCode;
    argument << data.m_stateCode;
    argument << data.m_roadClass;
    argument << data.m_vehicleSpeed;
    argument << data.m_vehicleHeading;
    argument << data.m_altitude;
    argument << data.m_country;
    argument << data.m_province;
    argument << data.m_city;
    argument << data.m_cityDistrict;
    argument << data.m_street;
    argument << data.m_streetNumber;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionInformation &data)
{
    argument.beginStructure();
    argument >> data.m_positionWGS84;
    argument >> data.m_mapMatchingPositionAccuracy;
    argument >> data.m_countryCode;
    argument >> data.m_stateCode;
    argument >> data.m_roadClass;
    argument >> data.m_vehicleSpeed;
    argument >> data.m_vehicleHeading;
    argument >> data.m_altitude;
    argument >> data.m_country;
    argument >> data.m_province;
    argument >> data.m_city;
    argument >> data.m_cityDistrict;
    argument >> data.m_street;
    argument >> data.m_streetNumber;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSPositionWGS84 &pos)
{
    argument.beginStructure();
    argument << pos.m_longitude;
    argument << pos.m_latitude;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPositionWGS84 &pos)
{
    argument.beginStructure();
    argument >> pos.m_longitude;
    argument >> pos.m_latitude;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSRefinementListElement &data)
{
    argument.beginStructure();
    argument << data.m_data;
    argument << data.m_id;
    argument << data.m_distance;
    argument << data.m_direction;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSRefinementListElement &data)
{
    argument.beginStructure();
    argument >> data.m_data;
    argument >> data.m_id;
    argument >> data.m_distance;
    argument >> data.m_direction;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSRouteCalculationDuration &data)
{
    argument.beginStructure();
    argument << data.m_routeCalDurationToFinalDest;
    argument << data.m_routeCalDurationToNextDest;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSRouteCalculationDuration &data)
{
    argument.beginStructure();
    argument >> data.m_routeCalDurationToFinalDest;
    argument >> data.m_routeCalDurationToNextDest;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSSatelliteInfo &data)
{
    argument.beginStructure();
    argument << data.m_visibleSatellites;
    argument << data.m_receivedSatellites;
    argument << data.m_usedSatellites;
    argument << data.m_gnssMode;
    argument << data.m_gnssSpeed;
    argument << data.m_gnssSpeedReliability;
    argument << data.m_gnssPosition;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSSatelliteInfo &data)
{
    argument.beginStructure();
    argument >> data.m_visibleSatellites;
    argument >> data.m_receivedSatellites;
    argument >> data.m_usedSatellites;
    argument >> data.m_gnssMode;
    argument >> data.m_gnssSpeed;
    argument >> data.m_gnssSpeedReliability;
    argument >> data.m_gnssPosition;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSScreenCoordinate &data)
{
    argument.beginStructure();
    argument << data.m_xPos;
    argument << data.m_yPos;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSScreenCoordinate &data)
{
    argument.beginStructure();
    argument >> data.m_xPos;
    argument >> data.m_yPos;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSAddressElement &data)
{
    argument.beginStructure();
    argument << data.m_addressElementType;
    argument << data.m_data;
    argument << data.m_elementOptions;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSAddressElement &data)
{
    argument.beginStructure();
    argument >> data.m_addressElementType;
    argument >> data.m_data;
    argument >> data.m_elementOptions;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSAddressWithOptions &data)
{
    argument.beginStructure();
    argument << data.m_address;
    argument << data.m_addressOptions;
    argument << data.m_addressElementType;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSAddressWithOptions &data)
{
    argument.beginStructure();
    argument >> data.m_address;
    argument >> data.m_addressOptions;
    argument >> data.m_addressElementType;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSElementOptions &data)
{
    argument.beginStructure();
    argument << data.m_isAmbigious;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSElementOptions &data)
{
    argument.beginStructure();
    argument >> data.m_isAmbigious;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSAddressOptions& data)
{
    argument.beginStructure();
    argument << data.navigable;
    argument << data.ambigious;
    argument << data.removedInvalidEntry;
    argument << data.moreEntriesPossible;
    argument << data.houseNumberAvailable;
    argument << data.houseNumberValid;
    argument.endStructure();
}


const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSAddressOptions &data)
{
    argument.beginStructure();
    argument >> data.navigable;
    argument >> data.ambigious;
    argument >> data.removedInvalidEntry;
    argument >> data.moreEntriesPossible;
    argument >> data.houseNumberAvailable;
    argument >> data.houseNumberValid;
    argument.endStructure();
}



QDBusArgument &operator<<(QDBusArgument &argument, const NSSDSVoiceTagId &data)
{
    argument.beginStructure();
    argument << data.m_destinationMemoryEntryId;
    argument << data.m_destinationMemoryEntryName;
    argument << data.m_sdsVoiceTagId;
    argument << data.m_options;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSSDSVoiceTagId &data)
{
    argument.beginStructure();
    argument >> data.m_destinationMemoryEntryId;
    argument >> data.m_destinationMemoryEntryName;
    argument >> data.m_sdsVoiceTagId;
    argument >> data.m_options;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSSpeedLimit &data)
{
    argument.beginStructure();
    argument << data.m_present;
    argument << data.m_countryCode;
    argument << data.m_value;
    argument << data.m_speedUnit;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSSpeedLimit &data)
{
    argument.beginStructure();
    argument >> data.m_present;
    argument >> data.m_countryCode;
    argument >> data.m_value;
    argument >> data.m_speedUnit;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSTourElement &data)
{
    argument.beginStructure();
    argument << data.m_location;
    argument << data.m_useDefaultRouteCriterion;
    argument << data.m_routeCriterion;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSTourElement &data)
{
    argument.beginStructure();
    argument >> data.m_location;
    argument >> data.m_useDefaultRouteCriterion;
    argument >> data.m_routeCriterion;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSTrafficServiceReceptionStatus &data)
{
    argument.beginStructure();
    argument << data.m_serviceName;
    argument << data.m_trafficServiceReceptionStatus;
    argument << data.m_trafficReceptionState;
    argument << data.m_trafficMessageCount;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSTrafficServiceReceptionStatus &data)
{
    argument.beginStructure();
    argument >> data.m_serviceName;
    argument >> data.m_trafficServiceReceptionStatus;
    argument >> data.m_trafficReceptionState;
    argument >> data.m_trafficMessageCount;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSWaypointListElement &data)
{
    argument.beginStructure();
    argument << data.m_location;
    argument << data.m_routeCriterion;
    argument << data.m_name;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSWaypointListElement &data)
{
    argument.beginStructure();
    argument >> data.m_location;
    argument >> data.m_routeCriterion;
    argument >> data.m_name;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSWorkLocation &loc)
{
    argument.beginStructure();
    argument << loc.m_name;
    argument << loc.m_positionWGS84;
    argument << loc.m_valid;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSWorkLocation &loc)
{
    argument.beginStructure();
    argument >> loc.m_name;
    argument >> loc.m_positionWGS84;
    argument >> loc.m_valid;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSNavDataPartNumber& data)
{
    argument.beginStructure();
    argument << data.result;
    argument << data.partNumber;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSNavDataPartNumber &data)
{
    argument.beginStructure();
    argument >> data.result;
    argument >> data.partNumber;
    argument.endStructure();

    return argument;
}
