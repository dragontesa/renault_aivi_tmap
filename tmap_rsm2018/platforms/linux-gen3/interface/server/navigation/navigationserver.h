#ifndef NAVIGATIONSERVER_H
#define NAVIGATIONSERVER_H

#include <QObject>
#include "navigationservicetype.h"

class NavigationServiceAdaptor;
class FreeTextSearchResultCache;
class AddressSearchResultCache;

class NavigationServer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.bosch.cm.navigation.NavigationService")
public:
    explicit NavigationServer(QObject* parent = 0);
    ~NavigationServer();

    void registerService();
    void unregisterService();
    void notifyPropertiesChanged(const QString& interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) const;

public: //property
    NSArrivalInformationList arrivalInformationList() const;
    void setArrivalInformationList(const NSArrivalInformationList &value);

    NSBarGraphInfo barGraphInfo() const;
    void setBarGraphInfo(const NSBarGraphInfo &value);

    // feature out
    // NSCurrentFreeTextSearchInfo currentFreeTextSearchInfo() const;
    // void setCurrentFreeTextSearchInfo(const NSCurrentFreeTextSearchInfo &value);

    QString currentStreet() const;
    void setCurrentStreet(const QString &value);

    QString datasetVersion() const;
    void setDatasetVersion(const QString &value);

    NSDestinationInfoElements destinationInformation() const;
    void setDestinationInformation(const NSDestinationInfoElements &value);

    NSDistanceToDestination distanceToDestination() const;
    void setDistanceToDestination(const NSDistanceToDestination &value);

    NSHomeLocation homeLocation() const;
    void setHomeLocation(const NSHomeLocation &value);

    // feature out
    // NSIntersectionMap intersectionMap() const;
    // void setIntersectionMap(const NSIntersectionMap &value);

    // feature out
    // NSIntersectionMapCarsorPosition intersectionMapCarsorPosition() const;
    // void setIntersectionMapCarsorPosition(const NSIntersectionMapCarsorPosition &value);

    bool isAddressBookChanged() const;
    void setIsAddressBookChanged(bool value);

    bool isLastDestinationListChanged() const;
    void setIsLastDestinationListChanged(bool value);

    bool isVoiceGuidanceActive() const;
    void setVoiceGuidanceActive(bool value);

    bool OverSpeedSoundAlertInfoActive() const;
    void setOverSpeedSoundAlertInfoActive(bool value);

    NSLaneRecommendationList laneRecommendationList() const;
    void setLaneRecommendationList(const NSLaneRecommendationList &value);

    NSTrafficEnforcement trafficEnforcement() const;
    void setTrafficEnforcement(const NSTrafficEnforcement &value);

    uint maneuverSymbol() const;
    void setManeuverSymbol(int index, uint value);

    NSManeuverView maneuverView() const;
    void setManeuverView(const NSManeuverView &value);

    uint mapRepresentation() const;
    void setMapRepresentation(uint representation);

    NSMapVideoStreamResult mapVideoStreamStatus() const;
    void setMapVideoStreamStatus(const NSMapVideoStreamResult &result);

    NSPoiCategoryShortcuts menuPoiCategoryShortcuts() const;
    void setMenuPoiCategoryShortcuts(const NSPoiCategoryShortcuts &value);

    uint navStatus() const;
    void setNavStatus(uint value);

    NSNextManeuverDetails nextManeuverDetails() const;
    void setNextManeuverDetails(const NSNextManeuverDetails &value);

    QString nextTurntoStreet() const;
    void setNextTurntoStreet(const QString &value);

    NSPositionInformation positionInformation() const;
    void setPositionInformation(const NSPositionInformation &positionInformation);

    ushort roundaboutExitNumber() const;
    void setRoundaboutExitNumber(const ushort &value);

    NSRouteCalculationDuration routeCalculationDuration() const;
    void setRouteCalculationDuration(NSRouteCalculationDuration &value);

    NSSatelliteInfo satelliteInfo() const;
    void setSatelliteInfo(NSSatelliteInfo &value);

    NSSDSAddressWithOptions sdsAddressWithOptions() const;
    void setSdsAddressWithOptions(const NSSDSAddressWithOptions &value);

    // feature out
    // NSPoiCategoryShortcuts sdsPOICategoryShortcuts() const;
    // void setSdsPOICategoryShortcuts(NSPoiCategoryShortcuts &value);

    // feature out
    // NSSDSVoiceTagId sdsVoiceTagWithOptions() const;
    // void setSdsVoiceTagWithOptions(NSSDSVoiceTagId &value);

    NSSpeedLimit speedLimit() const;
    void setSpeedLimit(NSSpeedLimit &value);

    uchar trafficMessagesCount() const;
    void setTrafficMessagesCount(uchar &value);

    // feature out
    // NSTrafficServiceReceptionStatus trafficServiceReceptionStatus() const;
    // void setTrafficServiceReceptionStatus(NSTrafficServiceReceptionStatus &value);

    void setRouteCancel();

    NSTurnToStreet turntoStreet() const;
    void setTurntoStreet(NSTurnToStreet &value);

    NSWaypointList waypointList() const;
    void setWaypointList(const NSWaypointList &value);

    NSWorkLocation workLocation() const;
    void setWorkLocation(NSWorkLocation &value);

    NSWaypointOpertaionStatusList waypointOperationStatusList() const;
    void setWaypointOperationStatusList(const NSWaypointOpertaionStatusList& wpstatList);
    
    uint routeRecalculationReason() const;
    void setRouteRecalculationReason(uint voiceSamplePhrase);

    uint destinationReachedStatus() const;
    void setDestinationReachedStatus(uint reachedStatus);

    QString getVersionString();
    QString navigationVersion();

    NSPoiCategoryShortcuts getPoiCategoryShortcuts();

    void showAdjustCurrentLocationScreen();

    NSPositionWGS84 currentSetLocation() const;

/////////// DBUS Methods
public slots:
    NSLocationDetails requestDetailsForSelectedLocation();
    NSAddressWithIdInfos requestFreeTextSearchResults(const QString &searchString, uint searchScope, uchar destinationIndex, uint startIndex, uint numElements);
    void showDestinationFreeTextSearchInput(uint locationType, uint locationSearchScope, const QString& searchString);
    void undoFreeTextSearchElementSelection();
    void handleFreeTextSearchResult(const QVariant& results);
    void sdsCheckAddress(const NSSDSAddress& requestedAddress);
    void handleAddressSearchResult(const QVariant& results);
    NSRefinementWithIdInfos sdsGetRefinementList();
    void selectSdsRefinement(uint id);
    void handleLocationDetail(const QVariant& results);
    // NSLocationDetails requestLocationDetailsForCoordinates(NSPositionWGS84 cityCoordinate); // feature out
    
    void setLocationWithCoordinates(NSPositionWGS84 location);
    void setLocationWithDetails(NSPOILocation location);
    void showAndSetLocationWithDetails(NSPOILocation location);
    void setLocationWithDestinationMemoryEntry(qulonglong id);
    void setLocationWithFreeTextSearchInput(uint id);
    void setLocationWithSdsInput();

    QString sdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns);
    // NSPositionWGS84 showCityInputAndGetCityCoordinates(bool &isValidAddress); // feature out
    NSDestinationMemoryDetailsList requestDestinationMemoryEntries(uint categoryType);
    void handleRequestDestinationMemoryEntries(const QVariant& result);
    NSMapVideoStreamResult startMapVideoStream(NSMapVideoStreamDimensions mapVideoStreamDimensions, const QString &ipAddress, ushort portnumber, ushort videoType);
    NSMapVideoStreamResult modifyMapVideoStream(uint mapVideoStreamHandle, NSMapVideoStreamDimensions mapVideoStreamDimensions, ushort videoType);
    NSMapVideoStreamResult deleteMapVideoStream(uint mapVideoStreamHandle);

    NSNavDataPartNumber getNavDataPartNumber();

    void doStartGuidance();
    void doCancelRouteGuidance();

private:
    void setLocation(const NSPOILocation& location);

signals: 
    void requestWaypointList();
    void applyWaypointListChange();
    void cancelRouteGuidance();
    void deleteWaypoints(uint firstIndex, uint lastIndex);
    void insertWaypoint(uint newIndex);
    void insertWaypointAtDefaultPosition();
    void replaceWaypoint(uint index);

    void notifySetLocationWithCoordinates(NSPositionWGS84 location);
    void notifySetLocationWithDetails(const NSPOILocation& location);
    void notifyShowAndSetLocationWithDetails(const NSPOILocation& location);
    void notifySetLocationWithDestinationMemoryEntry(qulonglong id);
    NSPOILocation toLocationWithDestinationMemoryEntry(uint category, uint id); // 2019-03-12
    // void notifySetLocationWithSdsInput();

    void notifyRequestFreeTextSearchResults(const QString &searchString,
    uint searchScope,
    uint searchType,
    uint category,
    uchar destinationIndex,
    uint startIndex,
    uint numElements
    ); // searchType - 0 : freetext 1: category search
    void doneRequestFreeTextSearchResults();
    void doneSdsCheckAddressResults();

    void notifySdsSearchAddress(const NSSDSAddress& requestedAddress);
    void doneSdsSearchAddress();

    void notifyRequestDetailForSelectedLocation(const NSPOILocation& location);
    void doneRequestDetailForSelectedLocation();

    void notifyShowDestinationFreeTextSearchInput(const QString& searchString);
    
    void selectPoiCategoryShortcut(uchar shortcutIndex);
    void setRouteCriterion(uint routeCriterion);
    void retriggerAcousticOutput();

    // void setSXMTravelLinkSubscriptionStatus(bool status); // feature out
    void setZoomInStep(uchar zoomInStep);
    void setZoomLevel(uint zoomLevel);
    void setZoomOutStep(uchar zoomOutStep);

    void showCalculateTour(QVariant tour, uint screenTransition);
    void showCarsorLocationOnMap();
    void showDeleteWaypointListScreen();
    // void showDestinationOverviewFromTCU(NSLocation currentPosition, NSLocation destination); // feature out
    // void showMapScreenCarsorLockMode(uint mapCameraMode); // feature out
    // void showMapScreenWithMapViewMode(uint MapViewMode); // feature out
    void showOnMap(NSPositionWGS84 location);
    void showPOILocationFromTCU(NSPOILocations poiLocations);
    // void showTrafficInfoScreen(); // feature out
    void showWhereAmIScreen();

    void startGuidance();
    void startGuidanceToHomeLocation();
    void startGuidanceToPosWGS84(NSPositionWGS84 destination);
    void startGuidanceToWorkLocation();
    void storePOILocationToAddressBook(NSPOILocation poiLocation);

    // uint CheckNavDataCompatibility(const QString &navDataVersion, const QByteArray &compatibilityInfo); // feature out
    // void sdsUpdateVoiceTag(NSSDSVoiceTagId voiceTagID); // feature out

    void speakSamplePhrase(uint sampleVoicePhrase);
    void storeLocationToAddressBook(NSLocation location);

////// app side internal signals
signals:
    void toSetMapRepresentation(uint present);
    void toSetVoiceGuidance(bool enable);
    NSDestinationMemoryDetailsList toRequestDestinationMemoryEntries(uint);
    void doneRequestDestinationMemoryEntries();
    
    // NSDestinationMemoryDetailListElement toShowLocationSelectionAndGetAddressDetails();
    QString toSdsGetHouseNumberPattern(QString&, QStringList&);
    NSMapVideoStreamResult toStartMapVideoStream(NSMapVideoStreamDimensions, const QString, ushort, ushort);
    NSMapVideoStreamResult toModifyMapVideoStream(uint mapVideoStreamHandle, NSMapVideoStreamDimensions mapVideoStreamDimensions, ushort videoType);
    NSMapVideoStreamResult toDeleteMapVideoStream(uint mapVideoStreamHandle);

    void toShowAdjustCurrentLocationScreen(NSPositionWGS84 pos);

/////////// DBUS Property Changed Signals
signals:
    // void currentFreeTextSearchInfoChanged(); // feature out
    // void intersectionMapCarsorPositionChanged();
    void routeCalculationDurationChanged();
    // void trafficMessagesCountChanged();
    // void trafficServiceReceptionStatusChanged(); // feature out

private:
    NavigationServiceAdaptor* m_adaptor;
    bool m_registered;
    NSDestinationMemoryDetailsList mDestinationMemoryDetailsList;
    NSPositionInformation mPositionInformation;
    QString mCurrentStreet;
    NSArrivalInformationList mNSArrivalInformationList;
    NSArrivalInformation mNSArrivalInformation;
    NSBarGraphInfo mNSBarGraphInfo;
    NSCurrentFreeTextSearchInfo mNSCurrentFreeTextSearchInfo;
    QString mDatasetVersion;
    NSDestinationInfoElements mNSDestinationInfoElements;
    NSDistanceToDestination mNSDistanceToDestination;
    NSHomeLocation mNSHomeLocation;
    // NSIntersectionMap mNSIntersectionMap; // feature out
    NSIntersectionMapCarsorPosition mNSIntersectionMapCarsorPosition;
    bool mIsAddressBookChanged;
    bool mIsLastDestinationListChanged;
    bool mIsVoiceGuidanceActive;
    bool mOverSpeedSoundAlertInfoActive;
    NSLaneRecommendationList mNSLaneRecommendationList;
    NSTrafficEnforcement mNSTrafficEnforcement;
    int mMemoryEntryCategory;
    int mManeuverSymbol;
    int mTBTIndex;
    RGTurnCode mRGTurnCode;
    NSManeuverView mNSManeuverView;
    int mNSManeuverViewId;
    int mNSManeuverViewIndex;
    MapRepresentation mMapRepresentation;
    NSMapVideoStreamResult mMapVideoStreamStatus;
    NSPoiCategoryShortcuts mNSPoiCategoryShortcuts;
    NSPoiCategoryShortcut mNSPoiCategoryShortcut;
    NavStatus mNavStatus;
    NSNextManeuverDetails mNSNextManeuverDetails;
    QString mNextTurntoStreet;
    NSRouteCalculationDuration mNSRouteCalculationDuration;
    NSSatelliteInfo mNSSatelliteInfo;
    NSSpeedLimit mNSSpeedLimit;
    uchar mTrafficMessagesCount;
    // NSTrafficServiceReceptionStatus mNSTrafficServiceReceptionStatus; // feature out
    NSTurnToStreet mTurntoStreet;
    int mTurntoStreetIndex;
    NSWaypointList mNSWaypointList;
    NSWorkLocation mNSWorkLocation;
    ushort mRoundaboutExitNumber;
    NSSDSAddressWithOptions mNSSDSAddressWithOptions;
    NSLocationDetails mDetailForSelectedLocation;
    // NSSDSVoiceTagId mNSSDSVoiceTagId; // feature out

    RouteRecalculationReason mRouteRecalculationReason;
    NSWaypointOpertaionStatusList mWaypointOpertaionStatusList;
    DestinationReachedStatus mDestinationReachedStatus;

    NSPOILocation mCurrentSetLocation;

    FreeTextSearchResultCache* mSearchResultCache;
    AddressSearchResultCache * mAddressSearchResultCache;
};

#endif // NAVIGATIONSERVER_H
