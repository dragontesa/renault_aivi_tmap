#include "navigationserver.h"
#include "navigationserviceadaptor.h"
#include "build.h"
#include "VSM_Util.h"
#include <QtCore>
#include "environment.h"
#include <QDomDocument>

static const QString serviceName = QStringLiteral("App.Core.AppHmi_Navigation");
static const QString navigationObjectPath = QStringLiteral("/com/bosch/cm/ai/navigation");
static const QString dBusPropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString dBusPropertiesChangedSignal = QStringLiteral("PropertiesChanged");
static const QString NavigationServiceInterface = QStringLiteral("org.bosch.cm.navigation.NavigationService");

#define SEARCH_RESULT_MAX 2000


class FreeTextSearchResultCache: public QVector<NSAddressListElement>
{
    friend class NavigationServer;
    public:
        FreeTextSearchResultCache()
        : mSearchText()
        , mSearchScope(FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_COMBINED_WHOLE_DATA)
        , mSearchType(0)
        , mDestinationIndex(0)
        , mStartIndex(0)
        , mResultMax(0)
        , mResultCount(0) {
            reserve(SEARCH_RESULT_MAX); // search data maximum 2000
        }

        FreeTextSearchResultCache(const QString& text, int scope, int searchtype, int destination, int start, int cnt)
        : mSearchText(text)
        , mSearchScope(scope)
        , mSearchType(searchtype)
        , mDestinationIndex(destination)
        , mStartIndex(start)
        , mResultMax(cnt)
        , mResultCount(0) {
            reserve(SEARCH_RESULT_MAX);
        }
         void resetSession() {
             mSearchText.clear();
             mSearchScope      = 0;
             mSearchType       = 0;
             mDestinationIndex = 0;
             mStartIndex       = 0;
             mResultMax        = 0;
             mResultCount      = 0;
             clear();
             reserve(SEARCH_RESULT_MAX);
         }
         
         void resetSession(const QString& text, int scope, int searchtype, int destination, int start, int cnt) {
             mSearchText       = text;
             mSearchScope      = scope;
             mSearchType       = searchtype;
             mDestinationIndex = destination;
             mStartIndex       = start;
             mResultMax        = cnt;
             mResultCount      = 0;

             clear();
             reserve(SEARCH_RESULT_MAX);
         }

         bool diffSession(const FreeTextSearchResultCache& other) {
            return (mSearchText       != other.mSearchText  ||
                    mSearchScope      != other.mSearchScope ||
                    mSearchType       != other.mSearchType  ||
                    mDestinationIndex != other.mDestinationIndex
                    );
         }

         NSAddressWithIdInfos result() {
             if (mResultCount == 0) return NSAddressWithIdInfos();
             QVector<NSAddressListElement> data = mid(qMax(0, mStartIndex), qMin(mResultCount,mResultMax));
             return data.toList();
         }

    private:
    QString mSearchText;
    int    mSearchScope;
    int    mSearchType;
    int    mDestinationIndex;
    int    mStartIndex;
    int    mResultMax;
    int    mResultCount;

};


class AddressSearchResultCache: public QVector<NSAddressListElement>
{
    friend class NavigationServer;
    public:
    AddressSearchResultCache(const NSSDSAddress& address=NSSDSAddress())
    : mStartIndex(0)
    , mResultMax(0)
    , mResultCount(0)
    , mPrimaryMax(9999)
    , mPrimaryMin(1)
    , mSecondaryMax(0)
    , mSecondaryMin(0) {
        reserve(SEARCH_RESULT_MAX);
        mSearchAddress = address;
    }
    void resetSession() {
        mSearchAddress.clear();
        mStartIndex       = 0;
        mResultMax        = 0;
        mResultCount      = 0;
        mPrimaryMax       = 9999;
        mPrimaryMin       = 1;
        mSecondaryMax     = 0;
        mSecondaryMin     = 0;
        clear();
        reserve(SEARCH_RESULT_MAX);
    }

    void resetSession(const NSSDSAddress& address) {
        mSearchAddress.clear();
        mSearchAddress    = address;
        mStartIndex       = 0;
        mResultMax        = 0;
        mResultCount      = 0;
        mPrimaryMax       = 9999;
        mPrimaryMin       = 1;
        mSecondaryMax     = 0;
        mSecondaryMin     = 0;
        clear();
        reserve(SEARCH_RESULT_MAX);
    }

    bool diffSession(const AddressSearchResultCache& other) {
        bool different = mSearchAddress.size() != other.mSearchAddress.size();
        if (different) return true;

        for (int i=0;i<mSearchAddress.size();i++) {
            different = mSearchAddress.at(i).m_addressElementType != other.mSearchAddress.at(i).m_addressElementType;
            if (!different)
               different = mSearchAddress.at(i).m_data != other.mSearchAddress.at(i).m_data;
            if (!different)
               different = mSearchAddress.at(i).m_elementOptions.m_isAmbigious != other.mSearchAddress.at(i).m_elementOptions.m_isAmbigious;

            if (different) break;
        }
        return different;
    }

    NSAddressWithIdInfos result() {
        if (mResultCount == 0)
            return NSAddressWithIdInfos();
        QVector<NSAddressListElement> data = mid(qMax(0, mStartIndex), mResultCount);
        return data.toList();
    }

  private:
    NSSDSAddress mSearchAddress;
    int mStartIndex;
    int mResultMax;
    int mResultCount;

    int mPrimaryMax;
    int mPrimaryMin;
    int mSecondaryMax;
    int mSecondaryMin;
};


NavigationServer::NavigationServer(QObject* parent)
    : QObject(parent)
    , m_adaptor(new NavigationServiceAdaptor(this))
    , m_registered(false)
    , mNavStatus(NavStatus_NAVSTATUS_BUSY)
    , mTurntoStreetIndex(0)
    , mManeuverSymbol(-1)
    , mTBTIndex(-999)
    , mRoundaboutExitNumber(0)
    , mNSManeuverViewIndex(0)

{
    mIsAddressBookChanged          = false;
    mIsLastDestinationListChanged  = false;
    mIsVoiceGuidanceActive         = false;
    mOverSpeedSoundAlertInfoActive = false;

    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "Failed to connect to DBus";
    } else if (!connection.registerObject(navigationObjectPath, this)) {
        qDebug() << "Failed to register object path";
    }

    mSearchResultCache        = new FreeTextSearchResultCache;
    mAddressSearchResultCache = new AddressSearchResultCache;
}

NavigationServer::~NavigationServer()
{
    unregisterService();
}

void NavigationServer::registerService()
{
    if (!m_registered) {
        QDBusConnection connection = QDBusConnection::sessionBus();

        if (!connection.isConnected()) {
            qWarning() << "Failed to connect to DBus";
            return;
        }

        if (connection.interface()->isServiceRegistered(serviceName)) {
            qWarning() << serviceName << " okay there get registered !";
        }
        else {
            if (!connection.registerService(serviceName)) {
                qWarning() << "Failed to register service: " << serviceName << " Already taken?";
                return;
            }
        }
        m_registered = true;
    }
}

void NavigationServer::unregisterService()
{
    if (m_registered) {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.unregisterService(serviceName);
        m_registered = false;
    }
}

// TODO: sending too frequently?
void NavigationServer::notifyPropertiesChanged(const QString& interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) const
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "Failed to connect to DBus";
        return;
    }

    QDBusMessage message = QDBusMessage::createSignal(navigationObjectPath,
                                                      dBusPropertiesInterface,
                                                      dBusPropertiesChangedSignal);

    QList<QVariant> arguments;
    arguments << QVariant(interfaceName) << QVariant(changedProperties) << QVariant(invalidatedProperties);
    message.setArguments(arguments);

    if (!connection.send(message)) {
        qDebug() << "Failed to send DBus property notification signal";
    }
}


/*******************************************************************
property
********************************************************************/
/**
 * @brief arrivalInformationList
 * @return NSArrivalInformationList
 *
 * Property to send all ETD, ETA and other information.
 *
 */
NSArrivalInformationList NavigationServer::arrivalInformationList() const {
    return mNSArrivalInformationList;
}

void NavigationServer::setArrivalInformationList(const NSArrivalInformationList &value) {

    if(value.size() > 0)
    {
        bool isUpdate = false;
        if(mNSArrivalInformationList.size() != value.size()){
            isUpdate = true;
        }else{
            if(mNSArrivalInformationList.size() != 0 && value.size() != 0){
                for(int i=0;i<value.size();i++){
                    if((mNSArrivalInformationList.at(i).m_destinationIndex != value[i].m_destinationIndex) ||
                       (mNSArrivalInformationList.at(i).m_displayTimeFormat != value[i].m_displayTimeFormat) ||
                       (mNSArrivalInformationList.at(i).m_displayTimeType != value[i].m_displayTimeType) ||
                       (mNSArrivalInformationList.at(i).m_distanceToDestination.m_distanceToDestination != value[i].m_distanceToDestination.m_distanceToDestination) ||
                       (mNSArrivalInformationList.at(i).m_distanceToDestination.m_distanceToDestinationAsString != value[i].m_distanceToDestination.m_distanceToDestinationAsString) ||
                       (mNSArrivalInformationList.at(i).m_distanceToDestination.m_distanceUnit != value[i].m_distanceToDestination.m_distanceUnit) ||
                       (mNSArrivalInformationList.at(i).m_estimatedTimeOfArrivalAsString != value[i].m_estimatedTimeOfArrivalAsString) ||
                       (mNSArrivalInformationList.at(i).m_estimatedTimeOfArrivalHour != value[i].m_estimatedTimeOfArrivalHour) ||
                       (mNSArrivalInformationList.at(i).m_estimatedTimeOfArrivalMinute != value[i].m_estimatedTimeOfArrivalMinute) ||
                       (mNSArrivalInformationList.at(i).m_estimatedTimeType != value[i].m_estimatedTimeType) ||
                       (mNSArrivalInformationList.at(i).m_timeToDestination != value[i].m_timeToDestination) ||
                       (mNSArrivalInformationList.at(i).m_timeToDestinationAsString != value[i].m_timeToDestinationAsString)){
                        isUpdate = true;
                    }
                }
            }else{
                isUpdate = true;
            }
        }

        if(isUpdate)
        {
            mNSArrivalInformationList.clear();

            for (int i =0; i < value.length(); i++) {
                mNSArrivalInformation.m_destinationIndex                    = value[i].m_destinationIndex;
                mNSArrivalInformation.m_displayTimeFormat                   = value[i].m_displayTimeFormat;
                mNSArrivalInformation.m_displayTimeType                     = value[i].m_displayTimeType;
                mNSArrivalInformation.m_distanceToDestination               = value[i].m_distanceToDestination;
                mNSArrivalInformation.m_estimatedTimeOfArrivalAsString      = value[i].m_estimatedTimeOfArrivalAsString;
                mNSArrivalInformation.m_estimatedTimeOfArrivalHour          = value[i].m_estimatedTimeOfArrivalHour;
                mNSArrivalInformation.m_estimatedTimeOfArrivalMinute        = value[i].m_estimatedTimeOfArrivalMinute;
                mNSArrivalInformation.m_estimatedTimeType                   = value[i].m_estimatedTimeType;
                mNSArrivalInformation.m_timeToDestination                   = value[i].m_timeToDestination;
                mNSArrivalInformation.m_timeToDestinationAsString           = value[i].m_timeToDestinationAsString;
                mNSArrivalInformationList.append(mNSArrivalInformation);
            }

            QVariantMap changedProperties;
            changedProperties[QStringLiteral("arrivalInformationList")] = QVariant::fromValue(mNSArrivalInformationList);
            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        }
    }
}

/**
 * @brief barGraphInfo
 * @return
 *
 * If a turn is coming , bargraph will indicate how much(%) remaining distance vehicle need to cover before turn.
 *
 */
NSBarGraphInfo NavigationServer::barGraphInfo() const {
    return mNSBarGraphInfo;
}

void NavigationServer::setBarGraphInfo(const NSBarGraphInfo &value) {
    if((mNSBarGraphInfo.m_distanceUnit  != value.m_distanceUnit) ||
       (mNSBarGraphInfo.m_scale != value.m_scale) ||
       (mNSBarGraphInfo.m_valid != value.m_valid) ||
       (mNSBarGraphInfo.m_value != value.m_value))
        {
            mNSBarGraphInfo.m_distanceUnit  = value.m_distanceUnit;
            mNSBarGraphInfo.m_scale         = value.m_scale;
            mNSBarGraphInfo.m_valid         = value.m_valid;
            mNSBarGraphInfo.m_value         = value.m_value;
            mNSBarGraphInfo.m_barGraphMaxScale         = value.m_barGraphMaxScale;
            mNSBarGraphInfo.m_barGraphMinScale         = value.m_barGraphMinScale;

            QVariantMap changedProperties;
            changedProperties[QStringLiteral("barGraphInfo")] = QVariant::fromValue(mNSBarGraphInfo);

            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        }
}

// feature out
/**
 * @brief currentFreeTextSearchInfo
 * @return NSCurrentFreeTextSearchInfo
 *
 * Property to provide freeText search string and position information
 *
 */
// NSCurrentFreeTextSearchInfo NavigationServer::currentFreeTextSearchInfo() const {
// //    NSCurrentFreeTextSearchInfo mNSCurrentFreeTextSearchInfo;
//     return mNSCurrentFreeTextSearchInfo;
// }

// void NavigationServer::setCurrentFreeTextSearchInfo(const NSCurrentFreeTextSearchInfo &value) {
//     mNSCurrentFreeTextSearchInfo.m_position     = value.m_position;
//     mNSCurrentFreeTextSearchInfo.m_searchString = value.m_searchString;

//     emit currentFreeTextSearchInfoChanged();
// }

/**
 * @brief currentStreet
 * @return string [Current street name]
 *
 * Property to show current street
 *
 */
QString NavigationServer::currentStreet() const {
    return mCurrentStreet;
}

void NavigationServer::setCurrentStreet(const QString &value){
    if(QString::compare(mCurrentStreet, value, Qt::CaseInsensitive) != 0)
    {
        if(value != NULL || value.size() > 0)
        {
            mCurrentStreet = value;

            if(mCurrentStreet != NULL || mCurrentStreet.size() > 0)
            {
                QVariantMap changedProperties;
                changedProperties[QStringLiteral("currentStreet")] = QVariant::fromValue(mCurrentStreet);
                notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
            }
        }
    }
}

/**
 * @brief datasetVersion
 * @return string [Navigation data set version]
 *
 * Property to show navigation data set version.
 *
 */
QString NavigationServer::datasetVersion() const {
//    mDatasetVersion = "datasetVersion";
    return mDatasetVersion;
}

void NavigationServer::setDatasetVersion(const QString &value) {
    if (mDatasetVersion != value) {
        mDatasetVersion = value;
    }
}

/**
 * @brief destinationInformation
 * @return NSDestinationInfoElements
 *
 * Property to provide detail information about destination.
 *
 */
NSDestinationInfoElements NavigationServer::destinationInformation() const {
    return mNSDestinationInfoElements;
}

void NavigationServer::setDestinationInformation(const NSDestinationInfoElements &value) {
    mNSDestinationInfoElements.clear();
    for (int i = 0; i <  value.length(); i++) {
        NSDestinationInfoElement info;
        info.m_countryCode     = value[i].m_countryCode;
        info.m_location        = value[i].m_location;
        info.m_name            = value[i].m_name;
        info.m_subCountryCode  = value[i].m_subCountryCode;
        mNSDestinationInfoElements.append(info);
    }

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("destinationInformation")] = QVariant::fromValue(mNSDestinationInfoElements);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}


/**
 * @brief distanceToDestination
 * @return NSDistanceToDestination
 *
 * Property to show distance both in string and integer.
 *
 */
NSDistanceToDestination NavigationServer::distanceToDestination() const {
    return mNSDistanceToDestination;
}

void NavigationServer::setDistanceToDestination(const NSDistanceToDestination &value) {
    if((mNSDistanceToDestination.m_distanceToDestination != value.m_distanceToDestination) ||
        (mNSDistanceToDestination.m_distanceToDestinationAsString != value.m_distanceToDestinationAsString) ||
        (mNSDistanceToDestination.m_distanceUnit != value.m_distanceUnit)){
            mNSDistanceToDestination.m_distanceToDestination            = value.m_distanceToDestination;
            mNSDistanceToDestination.m_distanceToDestinationAsString    = value.m_distanceToDestinationAsString;
            mNSDistanceToDestination.m_distanceUnit                     = value.m_distanceUnit;
            QVariantMap changedProperties;
            changedProperties[QStringLiteral("distanceToDestination")] = QVariant::fromValue(mNSDistanceToDestination);
            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief homeLocation
 * @return NSHomeLocation
 *
 * Property to provide location of the navigation home location (if set any).
 *
 */
NSHomeLocation NavigationServer::homeLocation() const {
    return mNSHomeLocation;
}

void NavigationServer::setHomeLocation(const NSHomeLocation &value) {
    mNSHomeLocation.m_name          = value.m_name;
    mNSHomeLocation.m_positionWGS84 = value.m_positionWGS84;
    mNSHomeLocation.m_valid         = value.m_valid;
    QVariantMap changedProperties;
    changedProperties[QStringLiteral("homeLocation")] = QVariant::fromValue(mNSHomeLocation);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}

// feature out
/**
 * @brief intersectionMap
 * @return NSIntersectionMap
 *
 * Property to provide carsorPosition in the intersection map.
 *
 */
// NSIntersectionMap NavigationServer::intersectionMap() const {
//     return mNSIntersectionMap;
// }

// void NavigationServer::setIntersectionMap(const NSIntersectionMap &value) {
//     mNSIntersectionMap.m_intersectionNumber = value.m_intersectionNumber;
//     mNSIntersectionMap.m_noEntryIcons       = value.m_noEntryIcons;
//     mNSIntersectionMap.m_road               = value.m_road;
//     mNSIntersectionMap.m_route              = value.m_route;

//     emit intersectionMapChanged();
// }

// feature out
/**
 * @brief intersectionMapCarsorPosition
 * @return NSIntersectionMapCarsorPosition
 *
 * Property to provide carsorPosition in the intersection map.
 *
 */
// NSIntersectionMapCarsorPosition NavigationServer::intersectionMapCarsorPosition() const {
//     return mNSIntersectionMapCarsorPosition;
// }

// void NavigationServer::setIntersectionMapCarsorPosition(const NSIntersectionMapCarsorPosition &value) {
//     mNSIntersectionMapCarsorPosition.m_carsorAngle          = value.m_carsorAngle;
//     mNSIntersectionMapCarsorPosition.m_carsorPosition       = value.m_carsorPosition;
//     mNSIntersectionMapCarsorPosition.m_carsorState          = value.m_carsorState;
//     mNSIntersectionMapCarsorPosition.m_intersectionMapId    = value.m_intersectionMapId;

//     emit intersectionMapCarsorPositionChanged();
// }

/**
 * @brief isAddressBookChanged
 * @return
 *
 * Property to provide info if Address book has to be requested again using the method requestDestinationMemoryEntries
 *
 */
bool NavigationServer::isAddressBookChanged() const {
    return mIsAddressBookChanged;
}

void NavigationServer::setIsAddressBookChanged(bool value) {
    mIsAddressBookChanged = value;
    QVariantMap changedProperties;
    changedProperties[QStringLiteral("isAddressBookChanged")] = QVariant::fromValue(value);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    mIsAddressBookChanged = false;
}

/**
 * @brief isLastDestinationListChanged
 * @return boolean
 *
 * Property to provide info if LastDestination list has to be requested again using the method requestDestinationMemoryEntries
 *
 */
bool NavigationServer::isLastDestinationListChanged() const {
    return mIsLastDestinationListChanged;
}
void NavigationServer::setIsLastDestinationListChanged(bool value) {
    // 20200327, this property must tell system that data got changed if it was done.
    // if (mIsLastDestinationListChanged != value) {
        mIsLastDestinationListChanged = value;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("isLastDestinationListChanged")] = QVariant::fromValue(value);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        // mIsLastDestinationListChanged = false;   // 20200324 update TMCRSM-4678
    // }
}

/**
 * @brief isVoiceGuidanceActive
 * @return boolean [The voice guidance status(Active/Deactive)]
 *
 * Attribute provides voice guidance status.
 *
 */
bool NavigationServer::isVoiceGuidanceActive() const {
    return mIsVoiceGuidanceActive;
}

void NavigationServer::setVoiceGuidanceActive(bool value) {
    if (mIsVoiceGuidanceActive != value) {
        mIsVoiceGuidanceActive = value;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("isVoiceGuidanceActive")] = QVariant::fromValue(value);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}

bool NavigationServer::OverSpeedSoundAlertInfoActive() const {
    return mOverSpeedSoundAlertInfoActive;
}

void NavigationServer::setOverSpeedSoundAlertInfoActive(bool value) {
    if (mOverSpeedSoundAlertInfoActive != value) {
        mOverSpeedSoundAlertInfoActive = value;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("OverSpeedSoundAlertInfoActive")] = QVariant::fromValue(value);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief laneRecommendationList
 * @return NSLaneRecommendationList
 *
 * Property to send number of lanes, lane charecteristics and lane directions
 *
 */
NSLaneRecommendationList NavigationServer::laneRecommendationList() const {
    return mNSLaneRecommendationList;
}
void NavigationServer::setLaneRecommendationList(const NSLaneRecommendationList &value) {
    bool isUpdate = false;
    if(mNSLaneRecommendationList.size() != value.size()){
        isUpdate = true;
    }else{
        if(mNSLaneRecommendationList.size() > 0 && value.size() > 0){
            for(int i=0;i<value.size();i++)
            {
                if((mNSLaneRecommendationList.at(i).m_combinedFollowingLaneDirection != value.at(i).m_combinedFollowingLaneDirection) ||
                    (mNSLaneRecommendationList.at(i).m_guidedFollowingLaneDirection != value.at(i).m_guidedFollowingLaneDirection) ||
                    (mNSLaneRecommendationList.at(i).m_laneStatus != value.at(i).m_laneStatus)){
                    isUpdate = true;
                }
            }
        }
    }

    if(isUpdate){
        mNSLaneRecommendationList.clear();
        mNSLaneRecommendationList = value;

        if(mNSLaneRecommendationList.size() > 0)
        {
            QVariantMap changedProperties;
            changedProperties[QStringLiteral("laneRecommendationList")] = QVariant::fromValue(value);
            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        }
    }
}

/**
 * @brief laneRecommendationList
 * @return NSLaneRecommendationList
 *
 * Property to send number of lanes, lane charecteristics and lane directions
 *
 */
NSTrafficEnforcement NavigationServer::trafficEnforcement() const {
    return mNSTrafficEnforcement;
}
void NavigationServer::setTrafficEnforcement(const NSTrafficEnforcement &value) {
    if((mNSTrafficEnforcement.m_TrafficEnforcementAlertState != value.m_TrafficEnforcementAlertState) ||
        (mNSTrafficEnforcement.m_trafficEnforcementCameraType != value.m_trafficEnforcementCameraType) ||
        (mNSTrafficEnforcement.m_trafficEnforcementDistanceType != value.m_trafficEnforcementDistanceType) ||
        (mNSTrafficEnforcement.m_distance != value.m_distance) ||
        (mNSTrafficEnforcement.m_distanceUnit != value.m_distanceUnit) ||
        (mNSTrafficEnforcement.m_speedLimit != value.m_speedLimit)){
            mNSTrafficEnforcement = value;
            QVariantMap changedProperties;
            changedProperties[QStringLiteral("trafficEnforcement")] = QVariant::fromValue(value);
            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}


/**
 * @brief maneuverSymbol
 * @return enum ManeuverSymbol
 */

uint ConvetTurnCodeToManeuverSymbol(uint turncode){
    uint maneuverSymbol;

    qDebug() << "turncode::::" << turncode << endl;
    switch(turncode){
        case IDV_RG_turn_pt_map:                    //	1	도곽에 의한 점
        case IDV_RG_turn_pt_tile:                   //	2	타일에 의한 점
        case IDV_RG_turn_not_ser_exp:               //	3	고속도로에 의한 안내없음
        case IDV_RG_turn_not_ser_normal:            //	4	일반도로에 의한 안내없음
        case IDV_RG_turn_not_ser_spec:              //	5	특수한 경우 안내없음
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_NO_INFO;
            break;
        case IDV_RG_str_right:                      //	6	Y자 오른쪽
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_KEEP_RIGHT;
            break;
        case IDV_RG_str_left:                       //	7	Y자 왼쪽
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_KEEP_LEFT;
            break;
        case IDV_RG_turn_str:                       //	11	직진
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_STRAIGHT;
            break;
        case IDV_RG_turn_left:                      //	12	좌회전
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_LEFT;
            break;
        case IDV_RG_turn_right:                     //	13	우회전
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_RIGHT;
            break;
        case IDV_RG_turn_Uturn:                     //	14	U-turn
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_UTURN_LEFT;
            break;
        case IDV_RG_turn_pturn:                     //	15	P-turn
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_EXIT_CLOCKWISE_TO_LEFT; //ManeuverSymbol_MANEUVER_SYMBOL_PTURN;
            break;
        case IDV_RG_turn_left_8:                    //	16	8시 방향 좌회전
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_SHARPLEFT;
            break;
        case IDV_RG_turn_left_10:                   //	17	10시 방향 좌회전
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_HALFLEFT;
            break;
        case IDV_RG_turn_rignt_2:                   //	18	2시 방향 우회전
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_HALFRIGHT;
            break;
        case IDV_RG_turn_rignt_4:                   //	19	4시 방향 우회전
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_SHARPRIGHT;
            break;
        case IDV_RG_turn_dir_1:                     //	31	1시 방향
        case IDV_RG_turn_dir_2:                     //	32	2시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_HALFRIGHT;
            break;
        case IDV_RG_turn_dir_3:                     //	33	3시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_RIGHT;
            break;
        case IDV_RG_turn_dir_4:                     //	34	4시 방향
        case IDV_RG_turn_dir_5:                     //	35	5시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_SHARPRIGHT;
            break;
        case IDV_RG_turn_dir_6:                     //	36	6시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_105;
            break;
        case IDV_RG_turn_dir_7:                     //	37	7시 방향
        case IDV_RG_turn_dir_8:                     //	38	8시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_SHARPLEFT;
            break;
        case IDV_RG_turn_dir_9:                     //	39	9시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_LEFT;
            break;
        case IDV_RG_turn_dir_10:                    //	40	10시 방향
        case IDV_RG_turn_dir_11:                    //	41	11시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TURN_HALFLEFT;
            break;
        case IDV_RG_turn_dir_12:                    //	42	12시 방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_001;
            break;
        case IDV_RG_turn_str_right:                 //	43	오른쪽
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_KEEP_RIGHT;
            break;
        case IDV_RG_turn_str_left:                  //	44	왼쪽
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_KEEP_LEFT;
            break;
        case IDV_RG_lane_str:                       //	51	직진차선으로 주행하십시요.
        case IDV_RG_lane_left:                      //	52	왼쪽차선으로 주행하십시요.
        case IDV_RG_lane_right:                     //	53	오른쪽차선으로 주행하십시요.
        case IDV_RG_lane_1:                         //	54	1차선으로 주행하십시요.
        case IDV_RG_lane_2:                         //	55	2차선으로 주행하십시요.
        case IDV_RG_lane_3:                         //	56	3차선으로 주행하십시요.
        case IDV_RG_lane_4:                         //	57	4차선으로 주행하십시요.
        case IDV_RG_lane_5:                         //	58	5차선으로 주행하십시요.
        case IDV_RG_lane_6:                         //	59	6차선으로 주행하십시요.
        case IDV_RG_lane_7:                         //	60	7차선으로 주행하십시요.
        case IDV_RG_lane_8:                         //	61	8차선으로 주행하십시요.
        case IDV_RG_lane_9:                         //	62	9차선으로 주행하십시요.
        case IDV_RG_lane_10:                        //	63	10차선으로 주행하십시요.
        case IDV_RG_first_exit:                     //	71	첫번째 출구입니다.
        case IDV_RG_sec_exit:                       //	72	두번째 출구입니다.
        case IDV_RG_first_lane_right:               //	73	첫번째 오른쪽길입니다.
        case IDV_RG_sec_lane_right:                 //	74	두번째 오른쪽길입니다.
        case IDV_RG_first_lane_left:                //	75	첫번째 왼쪽길입니다.
        case IDV_RG_sec_lane_left:                  //	76	두번째 왼쪽길입니다.
        case IDV_RG_Highpath_left:                  // 80 하이패스차로는 좌측에 있습니다.
        case IDV_RG_Highpath_right:                 // 81 하이패스차로는 우측에 있습니다.
        case IDV_RG_Highpath_center:                // 82 하이패스차로는 중앙에 있습니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_NO_INFO;
            break;
        case IDV_RG_exp_in_right:                   //	101	오른쪽 고속도로입구로 진입하십시요.
        case IDV_RG_saexp_in_right:                 //	111	오른쪽 고속화도로 입구로 진입하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_LANE_ENTER_EXPRESSWAY; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_IN_RIGHT;
            break;
        case IDV_RG_exp_in_left:                    //	102	왼쪽 고속도로입구로 진입하십시요.
        case IDV_RG_saexp_in_left:                  //	112	왼쪽 고속화도로 입구로 진입하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_LEFT_LANE_ENTER_EXPRESSWAY; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_IN_LEFT;
            break;
        case IDV_RG_exp_in_str:                     //	103	직진방향 고속도로입구로 진입하십시요.
        case IDV_RG_saexp_in_str:                   //	113	직진방향 고속화도로 입구로 진입하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ENTRY_STRAIGHT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_IN_STR;
            break;
        case IDV_RG_exp_out_right:                  //	104	오른쪽 고속도로출구로 진출하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_EXIT_RIGHT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_RIGHT;
            break;
        case IDV_RG_exp_out_left:                   //	105	왼쪽 고속도로출구로 진출하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_EXIT_LEFT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_LEFT;
            break;
        case IDV_RG_exp_out_str:                    //	106	직진방향 고속도로출구로 진출하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_EXIT_STRAIGHT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_STR;
            break;
        case IDV_RG_saexp_out_right:                //	114	오른쪽 고속화도로 출구로 진출하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_EXIT_RIGHT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_RIGHT;
            break;
        case IDV_RG_saexp_out_left:                 //	115	왼쪽 고속화도로 출구로 진출하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_EXIT_LEFT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_LEFT;
            break;
        case IDV_RG_saexp_out_str:                  //	116	직진방향 고속화도로 출구로 진출하십시요.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_EXIT_STRAIGHT; //ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_STR;
            break;
        case IDV_RG_road_right:                     //	117	오른쪽 도로입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ENTRY_RIGHT;
            break;
        case IDV_RG_road_left:                      //	118	왼쪽 도로입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ENTRY_LEFT;
            break;
        case IDV_RG_under_in:                       //	119	지하차도 진입입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_UNDERGROUND_ROAD; //ManeuverSymbol_MANEUVER_SYMBOL_UNDER_ROAD;
            break;
        case IDV_RG_sky_in:                         //	120	고가차도 진입입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_OVERPASS;
            break;
        case IDV_RG_tunnel_in:                      //	121	터널입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_TUNNEL;
            break;
        case IDV_RG_bridge:                         //	122	교량
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_BRIDGE;
            break;
        case IDV_RG_under_out:                      //	123	지하차도 옆길 입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_BESIDE_UNDERGROUND_ROAD; //ManeuverSymbol_MANEUVER_SYMBOL_BESIDE_UNDER_ROAD;
            break;
        case IDV_RG_sky_out:                        //	124	고가차도 옆길 입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_RIGHT_BESIDE_OVERPASS; //ManeuverSymbol_MANEUVER_SYMBOL_BESIDE_OVERPASS;
            break;
        case IDV_RG_dir_1:                          //	131	1시방향 출구입니다.
        case IDV_RG_RDir_1:                         //	131	1시방향 출구입니다.
        case IDV_RG_dir_2:                          //	132	2시방향 출구입니다.
        case IDV_RG_RDir_2:                         //	132	2시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_HALFRIGHT;
            break;
        case IDV_RG_dir_3:                          //	133	3시방향 출구입니다.
        case IDV_RG_RDir_3:                         //	133	3시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_RIGHT;
            break;
        case IDV_RG_dir_4:                          //	134	4시방향 출구입니다.
        case IDV_RG_RDir_4:                         //	134	4시방향 출구입니다.
        case IDV_RG_dir_5:                          //	135	5시방향 출구입니다.
        case IDV_RG_RDir_5:                         //	135	5시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_SHARPRIGHT;
            break;
        case IDV_RG_dir_6:                          //	136	6시방향 출구입니다.
        case IDV_RG_RDir_6:                         //	136	6시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_REVERSE;
            break;
        case IDV_RG_dir_7:                          //	137	7시방향 출구입니다.
        case IDV_RG_RDir_7:                         //	137	7시방향 출구입니다.
        case IDV_RG_dir_8:                          //	138	8시방향 출구입니다.
        case IDV_RG_RDir_8:                         //	138	8시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_SHARPLEFT;
            break;
        case IDV_RG_dir_9:                          //	139	9시방향 출구입니다.
        case IDV_RG_RDir_9:                         //	139	9시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_LEFT;
            break;
        case IDV_RG_dir_10:                         //	140	10시방향 출구입니다.
        case IDV_RG_RDir_10:                        //	140	10시방향 출구입니다.
        case IDV_RG_dir_11:                         //	141	11시방향 출구입니다.
        case IDV_RG_RDir_11:                        //	141	11시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_HALFLEFT;
            break;
        case IDV_RG_dir_12:                         //	142	12시방향 출구입니다.
        case IDV_RG_RDir_12:                        //	142	12시방향 출구입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ROUNDABOUT_TRAFFICSENSE_RIGHT_STRAIGHT;
            break;
        case IDV_RG_sa:                             //	151	휴게소
           maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_REST_AREA;
           break;
        case IDV_RG_sa_in:                          //	152 휴게소 진입
           maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_REST_AREA;
            break;
        case IDV_RG_Tol_exp:                        //	153	톨게이트 고속
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ENTER_TOLLGATE; //ManeuverSymbol_MANEUVER_SYMBOL_TOLLGATE;
            break;
        case IDV_RG_Tol_normal:                     //	154	톨게이트 일반
            maneuverSymbol =ManeuverSymbol_MANEUVER_SYMBOL_ENTER_TOLLGATE; //ManeuverSymbol_MANEUVER_SYMBOL_TOLLGATE;
            break;
        case IDV_RG_ferry_in:                       // 155 페리항로 진입
        case IDV_RG_ferry_out:                      // 156 페리항로 진출
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_NO_INFO;
            break;
        case IDV_RG_startdir_1:                     //	161	1시방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_009; // NORTH_NORTH_EAST
            break;
        case IDV_RG_startdir_2:                     //	162	2시방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_009; // NORTH_EAST
            break;
        case IDV_RG_startdir_3:                     //	163	3시방향
             maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_041;// EAST_NORTH_EAST
            break;
        case IDV_RG_startdir_4:                     //	164	4시방향
             maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_041;
            break;
        case IDV_RG_startdir_5:                     //	165	5시방향
             maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_073;
            break;
        case IDV_RG_startdir_6:                     //	166	6시방향
              maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_105;
            break;
        case IDV_RG_startdir_7:                     //	167	7시방향
              maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_137;
            break;
        case IDV_RG_startdir_8:                     //	168	8시방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_137;
            break;
        case IDV_RG_startdir_9:                     //	169	9시방향
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_169;
            break;
        case IDV_RG_startdir_10:                    //	170	10시방향
             maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_169;
            break;
        case IDV_RG_startdir_11:                    //	171	11시방향
             maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_201;
            break;
        case IDV_RG_startdir_12:                    //	172	12시방향
             maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_DIRECTION_001;
            break;
        case IDV_RG_dest_str:                       //	181	직진방향 목적지
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_FOLLOW;
            break;
        case IDV_RG_dest_left:                      //	182	왼쪽방향 목적지
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_REACHED_LEFT;
            break;
        case IDV_RG_dest_right:                     //	183	오른쪽방향 목적지
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_REACHED_RIGHT;
            break;
        case IDV_RG_pos_right:                      //	184	경유지
        case IDV_RG_via_1:                          //	185	첫번째 경유지
        case IDV_RG_via_2:                          //	186	두번째 경유지
        case IDV_RG_via_3:                          //	187	세번째 경유지
        case IDV_RG_via_4:                          //	188	네번째 경유지
        case IDV_RG_via_5:                          //	189	다섯번째 경유지
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_INTERMEDIATEDEST_REACHED;
            break;
//        case IDV_RG_notice_speed:                   //	191	제한속도
//        case IDV_RG_notice_Accident:                //	192	사고다발
//        case IDV_RG_notice_turn:                    //	193	급커브
//        case IDV_RG_notice_rock:                    //	194	낙석주의지역
//        case IDV_RG_notice_pol:                     //	195	경찰청 고속도로 차로 통행준수 안내
//        case IDV_RG_notice_pol2:                    //	195	경찰청 고속도로 차로 통행준수 안내
//        case IDV_RG_start:                          //	200	경로안내를 시작합니다.
        case IDV_RG_pos_goal:                       //	201	목적지가 있습니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_DEST_REACHED;
            break;
        case IDV_RG_go_straight:                    //	202	다음 안내시 까지 직진입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_STRAIGHT;
            break;
        case IDV_RG_in_Route:                       //	210	경로 진입 안내음.
        case IDV_RG_in_exp:                         //	220	고속도로에 있습니까?
        case IDV_RG_in_saexp:                       //	221	도시고속도로에 있습니까?
        case IDV_RG_in_sky:                         //	222	고가도로에 있습니까?
        case IDV_RG_in_under:                       //	223	지하도로에 있습니까?
        case IDV_RG_Tollgate:                       //	249	톨게이트입니다.
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_ENTER_TOLLGATE; //ManeuverSymbol_MANEUVER_SYMBOL_TOLLGATE;
            break;
        case IDV_RG_end:                            //	250	안내를 종료합니다.
            break;
        default:
            maneuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_NO_SYMBOL;
            break;
    }

    return maneuverSymbol;
}

uint NavigationServer::maneuverSymbol() const {
    return mManeuverSymbol;
}

void NavigationServer::setManeuverSymbol(int index, uint value) {
    if(mNavStatus == NavStatus_NAVSTATUS_GUIDANCE_ACTIVE)
    {
        qDebug() << "setManeuverSymbol #1" << index << mTBTIndex << "value : " << mManeuverSymbol << mNavStatus;

        uint maneuverID = ConvetTurnCodeToManeuverSymbol(value);
        qDebug() << "setManeuverSymbol #2" << index << mTBTIndex << "value : " << mManeuverSymbol << maneuverID << mNavStatus;

        if((mManeuverSymbol != maneuverID) || mTBTIndex == -999)
        {
            mTBTIndex = index;
            if(maneuverID != 0 || mManeuverSymbol == -1)
            {
                if(mManeuverSymbol == -1 && maneuverID == 0)
                {
                    mManeuverSymbol = ManeuverSymbol_MANEUVER_SYMBOL_STRAIGHT;
                }else{
                    mManeuverSymbol = maneuverID;
                }
                qDebug() << "setManeuverSymbol #3" << index << "value : " << mManeuverSymbol << mNavStatus;

                QVariantMap changedProperties;
                changedProperties[QStringLiteral("maneuverSymbol")] = mManeuverSymbol;
                notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
            }
        }
    }
}

/**
 * @brief maneuverView
 * @return NSManeuverView
 */
NSManeuverView NavigationServer::maneuverView() const {
    return mNSManeuverView;
}

void NavigationServer::setManeuverView(const NSManeuverView &value) {
    int id = ConvetTurnCodeToManeuverSymbol(value.m_intersectionMapId);

    if( (mNSManeuverViewId != value.m_intersectionMapId) ||
        (mNSManeuverView.m_maneuverViewType != value.m_maneuverViewType)){

        mNSManeuverViewId = value.m_intersectionMapId;

        mNSManeuverView.m_maneuverViewType = value.m_maneuverViewType;

//        if(id == ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_RIGHT || id == ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_RIGHT)
        if(id == ManeuverSymbol_MANEUVER_SYMBOL_EXIT_LEFT)
        {
            mNSManeuverView.m_deformedImage.m_valid = true;
            mNSManeuverView.m_deformedImage.m_deformedImageRoadType = 2; //0: NONE / 1:FORK / 2:EXIT_LEFT / 3:EXIT_RIGHT
            mNSManeuverView.m_deformedImage.m_deformedImageArrowType = 2; //0:NONE / 1:STRAIGHT / 2:LEFT /3:RIGHT
//        }else if(id == ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_LEFT || id == ManeuverSymbol_MANEUVER_SYMBOL_EXP_EXIT_RIGHT){
        }else if(id == ManeuverSymbol_MANEUVER_SYMBOL_EXIT_RIGHT){
            mNSManeuverView.m_deformedImage.m_valid = true;
            mNSManeuverView.m_deformedImage.m_deformedImageRoadType = 3; //0: NONE / 1:FORK / 2:EXIT_LEFT / 3:EXIT_RIGHT
            mNSManeuverView.m_deformedImage.m_deformedImageArrowType = 3; //0:NONE / 1:STRAIGHT / 2:LEFT /3:RIGHT
        }else{
            mNSManeuverView.m_deformedImage.m_valid = false;
            mNSManeuverView.m_deformedImage.m_deformedImageRoadType = 0; //0: NONE / 1:FORK / 2:EXIT_LEFT / 3:EXIT_RIGHT
            mNSManeuverView.m_deformedImage.m_deformedImageArrowType = 0; //0:NONE / 1:STRAIGHT / 2:LEFT /3:RIGHT
        }

        if(mNSManeuverView.m_deformedImage.m_deformedImageRoadType != 0)
        {
            mNSManeuverViewIndex++;
            mNSManeuverView.m_intersectionMapId = mNSManeuverViewIndex;

            QVariantMap changedProperties;
            changedProperties[QStringLiteral("maneuverView")] = QVariant::fromValue(mNSManeuverView);
            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        }
    }
}

/**
 * @brief mapRepresentation
 * @return enum MapRepresentation
 *
 * Property to provide the map representation (orientation and 2D / 3D)
 *
 */
uint NavigationServer::mapRepresentation() const {
    return mMapRepresentation;
}
/**
 * @brief NavigationServer::setMapRepresentation
 * @param representation [MapRepresentation]
 *
 * Select map representation:  dimension/heading combination.
 *
 */
void NavigationServer::setMapRepresentation(uint representation) {
   if (mMapRepresentation != (MapRepresentation)representation) {
       mMapRepresentation = (MapRepresentation)representation;
       QVariantMap changedProperties;
       changedProperties[QStringLiteral("mapRepresentation")] = representation;
       notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
   }
}

/**
 * @brief NavigationServer::mapVideoStreamStatus
 * @return
 *
 * This property is used by the Third party navigation to notify the MapVideoStream status.
 *
 */
NSMapVideoStreamResult NavigationServer::mapVideoStreamStatus() const {
    return mMapVideoStreamStatus;
}

/**
 * @brief NavigationServer::setMapVideoStreamStatus
 * @param result [NSMapVideoStreamResult]
 *
 */
void NavigationServer::setMapVideoStreamStatus(const NSMapVideoStreamResult &result) {
    if (mMapVideoStreamStatus != result) {
        qDebug() << "kdttest setMapVideoStreamStatus "<< result.m_mapVideoStreamStatus;
        mMapVideoStreamStatus = result;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("mapVideoStreamStatus")] = QVariant::fromValue(result);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}


/**
 * @brief menuPoiCategoryShortcuts
 * @return NSPoiCategoryShortcuts
 *
 * Attribute provides POI Category Shortcut Names in current selected language
 *
 */
NSPoiCategoryShortcuts NavigationServer::menuPoiCategoryShortcuts() const {
    return mNSPoiCategoryShortcuts;
}
void NavigationServer::setMenuPoiCategoryShortcuts(const NSPoiCategoryShortcuts &value) {
    mNSPoiCategoryShortcuts.clear();
    for (int i = 0; i < value.length(); i++) {
        mNSPoiCategoryShortcut.m_categoryName   = value[i].m_categoryName;
        mNSPoiCategoryShortcut.m_icon           = value[i].m_icon;
        mNSPoiCategoryShortcut.m_poiCategoryId  = value[i].m_poiCategoryId;
        mNSPoiCategoryShortcut.m_shortcutIndex  = value[i].m_shortcutIndex;

        mNSPoiCategoryShortcuts.append(mNSPoiCategoryShortcut);
    }
    QVariantMap changedProperties;
    changedProperties[QStringLiteral("menuPoiCategoryShortcuts")] = QVariant::fromValue(value);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}

/**
 * @brief navStatus
 * @return enum NavStatus
 *
 * This property is used by the Third party navigation to notify Bosch system about the navigation status.
 *
 */
uint NavigationServer::navStatus() const {
    return mNavStatus;
}

void NavigationServer::setNavStatus(uint value) {
    if (mNavStatus != static_cast<NavStatus>(value)) {
        qDebug() << "setNavStatus << " << value;
        mNavStatus = static_cast<NavStatus>(value);
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("navStatus")] = value;
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief nextManeuverDetails
 * @return NSNextManeuverDetails
 *
 * Property to provide next maneuver details.
 *
 */
NSNextManeuverDetails NavigationServer::nextManeuverDetails() const {
    return mNSNextManeuverDetails;
}

void NavigationServer::setNextManeuverDetails(const NSNextManeuverDetails &value) {
    qDebug()<< "setNextManeuverDetails #1";

    if(mManeuverSymbol != -1)
    {
        qDebug()<< "setNextManeuverDetails #2";

        if(mNSNextManeuverDetails.m_distanceToManeuver != value.m_distanceToManeuver || (value.m_isNewManeuver && mNSNextManeuverDetails.m_distanceToManeuver != "0"))
        {
            qDebug()<< "setNextManeuverDetails #3" << mNSNextManeuverDetails.m_distanceToManeuver << mNSNextManeuverDetails.m_isNewManeuver;

            mNSNextManeuverDetails.m_distanceToManeuver     = value.m_distanceToManeuver;
            mNSNextManeuverDetails.m_distanceToManeuverUnit = value.m_distanceToManeuverUnit;
            mNSNextManeuverDetails.m_isNewManeuver          = value.m_isNewManeuver;
            mNSNextManeuverDetails.m_isReRouted             = value.m_isReRouted;
            mNSNextManeuverDetails.m_recommendationType     = value.m_recommendationType;

            QVariantMap changedProperties;
            changedProperties[QStringLiteral("nextManeuverDetails")] = QVariant::fromValue(mNSNextManeuverDetails);

            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        }
    }
}

/**
 * @brief nextTurntoStreet
 * @return string [The next turn to street name]
 *
 * Property to show next turn street name.
 *
 */
QString NavigationServer::nextTurntoStreet() const {
    return mNextTurntoStreet;
}

void NavigationServer::setNextTurntoStreet(const QString &value) {
    if(QString::compare(mNextTurntoStreet, value, Qt::CaseInsensitive) != 0)
    {
        if(value != NULL || value.size() > 0)
        {
            mNextTurntoStreet = value;

            if(mNextTurntoStreet != NULL || mNextTurntoStreet.size() > 0)
            {
                qDebug() << "setNextTurntoStreet " << value ;
                QVariantMap changedProperties;
                changedProperties[QStringLiteral("nextTurntoStreet")] = QVariant::fromValue(mNextTurntoStreet);
                notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
            }
        }
    }
}

/**
 * @brief positionInformation
 * @return NSPositionInformation
 *
 * Property to provide position information only
 *
 */
NSPositionInformation NavigationServer::positionInformation() const {
    return mPositionInformation;
}

void NavigationServer::setPositionInformation(const NSPositionInformation &positionInformation)
{
    if (positionInformation != mPositionInformation) {
        mPositionInformation = positionInformation;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("positionInformation")] = QVariant::fromValue(mPositionInformation);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}

ushort NavigationServer::roundaboutExitNumber() const {
    return mRoundaboutExitNumber;
}

void NavigationServer::setRoundaboutExitNumber(const ushort &value) {
    if(mRoundaboutExitNumber != value)
    {
        mRoundaboutExitNumber = value;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("roundaboutExitNumber")] = QVariant::fromValue(mRoundaboutExitNumber);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief routeCalculationDuration
 * @return NSRouteCalculationDuration
 *
 * Property to provide route calculation duration
 *
 */
NSRouteCalculationDuration NavigationServer::routeCalculationDuration() const {
    return  mNSRouteCalculationDuration; // fixed value as 90
}

void NavigationServer::setRouteCalculationDuration(NSRouteCalculationDuration &value) {
    // 2020.2.20  this property should be updated no matter what the value
    // if (mNSRouteCalculationDuration.m_routeCalDurationToFinalDest != value.m_routeCalDurationToFinalDest 
    // ||  mNSRouteCalculationDuration.m_routeCalDurationToNextDest != value.m_routeCalDurationToNextDest) {
        mNSRouteCalculationDuration.m_routeCalDurationToFinalDest = value.m_routeCalDurationToFinalDest;
        mNSRouteCalculationDuration.m_routeCalDurationToNextDest = value.m_routeCalDurationToNextDest;

        QVariantMap changedProperties;
        changedProperties[QStringLiteral("routeCalculationDuration")] = QVariant::fromValue(mNSRouteCalculationDuration);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
    // }
}

/**
 * @brief satelliteInfo
 * @return NSSatelliteInfo
 *
 * Property to provide satellite sensor information
 *
 */
NSSatelliteInfo NavigationServer::satelliteInfo() const {
    return mNSSatelliteInfo;
}

void NavigationServer::setSatelliteInfo(NSSatelliteInfo &value) {
    mNSSatelliteInfo.m_visibleSatellites    = value.m_visibleSatellites;
    mNSSatelliteInfo.m_receivedSatellites   = value.m_receivedSatellites;
    mNSSatelliteInfo.m_usedSatellites       = value.m_usedSatellites;
    mNSSatelliteInfo.m_gnssSpeedReliability = value.m_gnssSpeedReliability;
    mNSSatelliteInfo.m_gnssSpeed            = value.m_gnssSpeed;
    mNSSatelliteInfo.m_gnssPosition         = value.m_gnssPosition;
    mNSSatelliteInfo.m_gnssMode             = value.m_gnssMode;

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("satelliteInfo")] = QVariant::fromValue(mNSSatelliteInfo);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}

/**
 * @brief sdsAddressWithOptions
 * @return NSSDSAddressWithOptions
 *
 * Property to provide the current SDS address entry
 *
 */
NSSDSAddressWithOptions NavigationServer::sdsAddressWithOptions() const {
    return mNSSDSAddressWithOptions;
}

void NavigationServer::setSdsAddressWithOptions(const NSSDSAddressWithOptions &value) {

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("sdsAddressWithOptions")] = QVariant::fromValue(value);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}

// feature out
/**
 * @brief sdsPOICategoryShortcuts
 * @return NSPoiCategoryShortcuts
 *
 * Attribute provides POI category shortcut names in current selected language
 *
 */

// NSPoiCategoryShortcuts NavigationServer::sdsPOICategoryShortcuts() const {
//     return mNSPoiCategoryShortcuts;
// }

// void NavigationServer::setSdsPOICategoryShortcuts(NSPoiCategoryShortcuts &value) {
//     mNSPoiCategoryShortcuts.clear();
//     for (int i = 0; i < value.length(); i++) {
//         mNSPoiCategoryShortcut.m_categoryName   = value[i].m_categoryName;
//         mNSPoiCategoryShortcut.m_icon           = value[i].m_icon;
//         mNSPoiCategoryShortcut.m_poiCategoryId  = value[i].m_poiCategoryId;
//         mNSPoiCategoryShortcut.m_shortcutIndex  = value[i].m_shortcutIndex;

//         mNSPoiCategoryShortcuts.append(mNSPoiCategoryShortcut);
//     }
// }

// feature out
/**
 * @brief sdsVoiceTagWithOptions
 * @return NSSDSVoiceTagId
 *
 * Property used as a request by Navgation to Create/Play/Delete voice tags
 *
 */
// NSSDSVoiceTagId NavigationServer::sdsVoiceTagWithOptions() const {
//     return mNSSDSVoiceTagId;
// }

// void NavigationServer::setSdsVoiceTagWithOptions(NSSDSVoiceTagId &value) {
//     mNSSDSVoiceTagId.m_destinationMemoryEntryId     = value.m_destinationMemoryEntryId;
//     mNSSDSVoiceTagId.m_destinationMemoryEntryName   = value.m_destinationMemoryEntryName;
//     mNSSDSVoiceTagId.m_options                      = value.m_options;
//     mNSSDSVoiceTagId.m_sdsVoiceTagId                = value.m_sdsVoiceTagId;
// }

/**
 * @brief NavigationServer::speedLimit
 * @return NSSpeedLimit
 */
NSSpeedLimit NavigationServer::speedLimit() const {
    return mNSSpeedLimit;
}

void NavigationServer::setSpeedLimit(NSSpeedLimit &value) {

    if((mNSSpeedLimit.m_present != value.m_present) ||
        (mNSSpeedLimit.m_speedUnit != value.m_speedUnit) ||
        (mNSSpeedLimit.m_value != value.m_value))
        {
            mNSSpeedLimit.m_countryCode = value.m_countryCode;
            mNSSpeedLimit.m_present     = value.m_present;
            mNSSpeedLimit.m_speedUnit   = value.m_speedUnit;
            if(value.m_value != 0)
                mNSSpeedLimit.m_value       = value.m_value;

            qDebug() << "onSpeedLimit m_countryCode " << mNSSpeedLimit.m_countryCode ;
            qDebug() << "onSpeedLimit m_present " << mNSSpeedLimit.m_present ;
            qDebug() << "onSpeedLimit m_speedUnit " << mNSSpeedLimit.m_speedUnit ;
            qDebug() << "onSpeedLimit m_value " << mNSSpeedLimit.m_value ;
            QVariantMap changedProperties;
            changedProperties[QStringLiteral("speedLimit")] = QVariant::fromValue(mNSSpeedLimit);

            notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
        }

        mNSSpeedLimit.m_value       = value.m_value;
}

// feature out
/**
 * @brief trafficMessagesCount
 * @return uchar [trafficMessagesCount]
 *
 * Property to provides traffic message count
 *
 */

// uchar NavigationServer::trafficMessagesCount() const {
//     return mTrafficMessagesCount;
// }
// void NavigationServer::setTrafficMessagesCount(uchar &value) {
//     mTrafficMessagesCount = value;

//     emit trafficMessagesCountChanged();
// }

// feature out
/**
 * @brief trafficServiceReceptionStatus
 * @return NSTrafficServiceReceptionStatus
 *
 * Attribute provides Traffic service reception status
 *
 */
// NSTrafficServiceReceptionStatus NavigationServer::trafficServiceReceptionStatus() const {
//     return mNSTrafficServiceReceptionStatus;
// }
// void NavigationServer::setTrafficServiceReceptionStatus(NSTrafficServiceReceptionStatus &value) {
//     mNSTrafficServiceReceptionStatus.m_serviceName                      = value.m_serviceName;
//     mNSTrafficServiceReceptionStatus.m_trafficMessageCount              = value.m_trafficMessageCount;
//     mNSTrafficServiceReceptionStatus.m_trafficReceptionState            = value.m_trafficReceptionState;
//     mNSTrafficServiceReceptionStatus.m_trafficServiceReceptionStatus    = value.m_trafficServiceReceptionStatus;

//     emit trafficServiceReceptionStatusChanged();
// }

/**
 * @brief turntoStreet
 * @return string [The turn to street name]
 *
 * Property to show turn street name street
 *
 */
NSTurnToStreet NavigationServer::turntoStreet() const {
    return mTurntoStreet;
}

//void NavigationServer::setTurntoStreet(QString &value) {
//    mTurntoStreet = value;
//    QVariantMap changedProperties;
//    ddfdffd
//    changedProperties[QStringLiteral("turntoStreet")] = QVariant::fromValue(mTurntoStreet);
//    //notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
//}

void NavigationServer::setRouteCancel()
{
    mTurntoStreetIndex = 0;
    mNSManeuverViewId = 0;
    mNSManeuverViewIndex = 0;
    mTBTIndex = -999;
    mManeuverSymbol = -1;
    mTurntoStreet.m_turnToStreet = "";
    mNSNextManeuverDetails.m_distanceToManeuver = "";
    mNSNextManeuverDetails.m_distanceToManeuverUnit = -1;
    mNSNextManeuverDetails.m_recommendationType = -1;
}

void NavigationServer::setTurntoStreet(NSTurnToStreet &value)
{
//    if(mNavStatus != NavStatus_NAVSTATUS_GUIDANCE_ACTIVE)
//    {
//        mTurntoStreet.m_turnToStreet = "";
//        mTurntoStreetIndex = 0;
//    }

        if(mNavStatus == NavStatus_NAVSTATUS_GUIDANCE_ACTIVE)
        {
            if(mManeuverSymbol != -1)
            {
                if(mTurntoStreet.m_turnToStreet != value.m_turnToStreet)
                {
    
                    mTurntoStreetIndex++;
                    mTurntoStreet.m_maneuverId = mTurntoStreetIndex;
                    mTurntoStreet.m_turnToStreet = value.m_turnToStreet;

                    QVariantMap changedProperties;
                    changedProperties[QStringLiteral("turntoStreet")] = QVariant::fromValue(mTurntoStreet);
                    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
                }
            }
        }
}

/**
 * @brief waypointList
 * @return NSWaypointList
 *
 * Property to provide inserted waypoints details on property update
 *
 */
NSWaypointList NavigationServer::waypointList() const {
    return mNSWaypointList;
}

void NavigationServer::setWaypointList(const NSWaypointList &value)
 {
    mNSWaypointList.clear();
    for (int i = 0; i < value.length(); i++) {
        NSWaypointListElement wypoint;
        wypoint.m_location       = value.at(i).m_location;
        wypoint.m_name           = value.at(i).m_name;
        wypoint.m_routeCriterion = value.at(i).m_routeCriterion;
        mNSWaypointList.append(wypoint);
    }

    QVariantMap changedProperties;
    changedProperties[QStringLiteral("waypointList")] = QVariant::fromValue(mNSWaypointList);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}

NSWaypointOpertaionStatusList NavigationServer::waypointOperationStatusList() const
{
    return mWaypointOpertaionStatusList;
}


void NavigationServer::setWaypointOperationStatusList(const NSWaypointOpertaionStatusList& wpstatList)
{
    mWaypointOpertaionStatusList = wpstatList;
    QVariantMap changedProperties;
    changedProperties[QStringLiteral("waypointOperationStatusList")] = QVariant::fromValue(wpstatList);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}


/**
 * @brief workLocation
 * @return NSWorkLocation
 *
 * Property to provide location of the navigation work location (if set any).
 *
 */
NSWorkLocation NavigationServer::workLocation() const {
    return mNSWorkLocation;
}

void NavigationServer::setWorkLocation(NSWorkLocation &value) {
    mNSWorkLocation.m_name          = value.m_name;
    mNSWorkLocation.m_positionWGS84 = value.m_positionWGS84;
    mNSWorkLocation.m_valid         = value.m_valid;
    qDebug() << "navi set worklocation";
    QVariantMap changedProperties;
    changedProperties[QStringLiteral("workLocation")] = QVariant::fromValue(mNSWorkLocation);
    notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
}

/*******************************************************************
method
********************************************************************/

/**
 * @brief NavigationServer::getVersionString
 * @return QString navigation version
 */
QString NavigationServer::getVersionString()
{
    return SKT::Build::versionString();
}

QString NavigationServer::navigationVersion()
{
    return SKT::Build::versionString();
}

// feature out
/**
 * @brief NavigationServer::CheckNavDataCompatibility
 * @param navDataVersion [the Navigation application on the update-medium]
 * @param compatibilityInfo [raw data information as read from update-medium]
 * @return enum NavDataCompatibility
 *
 * NavDataUpdate gets version of Navigation application to be installed and additional compatibility information provided by update-medium.
 *
 */
// uint NavigationServer::CheckNavDataCompatibility(const QString &navDataVersion, const QByteArray &compatibilityInfo) {
//     return NavDataCompatibility_NAVDATA_COMPATIBLE;
// }

/**
 * @brief NavigationServer::applyWaypointListChange
 *
 * Uses modified waypoint list and trigger new route calculation.
 *
 */
// void NavigationServer::applyWaypointListChange() {
// }

/**
 * @brief NavigationServer::cancelRouteGuidance
 *
 * Request will stop the guidance.
 *
 */
// void NavigationServer::cancelRouteGuidance() {
// }

/**
 * @brief NavigationServer::deleteWaypoints
 * @param firstIndex
 * @param lastIndex
 *
 * Delete the waypoints (including firstIndex and lastIndex).
 *
 */
// void NavigationServer::deleteWaypoints(uint firstIndex, uint lastIndex) {
// }

/**
 * @brief NavigationServer::getPoiCategoryShortcuts
 * @return NSPoiCategoryShortcuts [Array of PoiCategoryShortcuts]
 *
 * Get the customized preferred category shortcuts.
 *
 */
NSPoiCategoryShortcuts NavigationServer::getPoiCategoryShortcuts() {
   NSPoiCategoryShortcuts poiShortcutList;
   NSPoiCategoryShortcut poiShortcut;
   QString catName = "test";
   poiShortcut.m_categoryName = catName;
       NSImage iconImage;
       QByteArray byteImage;
       iconImage.m_image = byteImage;
       iconImage.m_imageType = 1;
   poiShortcut.m_icon = iconImage;
   poiShortcut.m_poiCategoryId = 1;
   poiShortcut.m_shortcutIndex = 1;
   poiShortcutList.append(poiShortcut);
   // return poiShortcutList;
   return mNSPoiCategoryShortcuts;
}

/**
 * @brief NavigationServer::getPoiIconCategoriesInMap  // feature out 2018/2/13
 * @return NSMapPOIIconCategories
 *
 * Get the currently visible POI icon categories in map.
 */
// NSMapPOIIconCategories NavigationServer::getPoiIconCategoriesInMap() {
//     NSMapPOIIconCategories mNSMapPOIIconCategories;
//     mNSMapPOIIconCategories.append(1);
//     mNSMapPOIIconCategories.append(2);
//     return mNSMapPOIIconCategories;
// }

/**
 * @brief rNavigationServer::replaceWaypoint
 * @param index
 *
 * Uses current location to replace waypoint.
 *
 */
// void NavigationServer::replaceWaypoint(uint index) {
// }

/**
 * @brief NavigationServer::requestDestinationMemoryEntries
 * @param categoryType
 * @return NSDestinationMemoryDetailsList [Array of DestinationMemoryDetailListElement]
 *
 * Request will return DestinationMemory entries for the given category.
 *
 */
NSDestinationMemoryDetailsList NavigationServer::requestDestinationMemoryEntries(uint categoryType) {
    // TODO: gotta async collabo with app
    qDebug() << "[INTERFACE] NavigationServer::requestDestinationMemoryEntries(" << categoryType << ")";

    mMemoryEntryCategory = categoryType;
    emit toRequestDestinationMemoryEntries(categoryType);
    // wait until responsed
    QEventLoop loop;
    connect(this, &NavigationServer::doneRequestDestinationMemoryEntries,
            &loop, &QEventLoop::quit);
    QTimer::singleShot(5000, &loop,&QEventLoop::quit);
    loop.exec();

    return mDestinationMemoryDetailsList;
}

void NavigationServer::handleRequestDestinationMemoryEntries(const QVariant& result)
{
    mDestinationMemoryDetailsList.clear();
    mDestinationMemoryDetailsList = result.value<NSDestinationMemoryDetailsList>();
    emit doneRequestDestinationMemoryEntries();
}

/**
 * @brief NavigationServer::startMapVideoStream
 * @param mapVideoStreamDimensions, ipAddress, portnumber, videoType
 * @return NSmapVideoStreamResult
 *
 * Requests to render the MapVideoStream in the cluster.
 *
 */
NSMapVideoStreamResult NavigationServer::startMapVideoStream(NSMapVideoStreamDimensions mapVideoStreamDimensions, const QString &ipAddress, ushort portnumber, ushort videoType) {
    return toStartMapVideoStream(mapVideoStreamDimensions, ipAddress, portnumber, videoType);
}

/**
 * @brief NavigationServer::modifyMapVideoStream
 * @param mapVideoStreamHandle, mapVideoStreamDimensions, videoType
 * @return NSmapVideoStreamResult
 *
 * Requests to modify the MapVideoStream of the cluster.
 *
 */
NSMapVideoStreamResult NavigationServer::modifyMapVideoStream(uint mapVideoStreamHandle, NSMapVideoStreamDimensions mapVideoStreamDimensions, ushort videoType) {
    return toModifyMapVideoStream(mapVideoStreamHandle, mapVideoStreamDimensions, videoType);
}


/**
 * @brief NavigationServer::deleteMapVideoStream
 * @param mapVideoStreamHandle
 * @return NSmapVideoStreamResult
 *
 * Requests to delete a MapVideoStream from the cluster.
 *
 */
NSMapVideoStreamResult NavigationServer::deleteMapVideoStream(uint mapVideoStreamHandle) {
    return toDeleteMapVideoStream(mapVideoStreamHandle);
}

/**
 * @brief NavigationServer::requestDetailsForSelectedLocation
 * @return NSLocationDetails
 *
 * Request will respond with location details for the selected location set via setLocationWithXYZ interface.
 *
 */
NSLocationDetails NavigationServer::requestDetailsForSelectedLocation() {
    if (mCurrentSetLocation.m_location.m_positionWGS84.m_latitude != 0  &&
        mCurrentSetLocation.m_location.m_positionWGS84.m_longitude != 0 )
       emit notifyRequestDetailForSelectedLocation(mCurrentSetLocation);
    else
       return NSLocationDetails();

    // wait until responsed
    QEventLoop loop;
    connect(this, SIGNAL(doneRequestDetailForSelectedLocation()),
            &loop, SLOT(quit()));
    QTimer::singleShot(5000, &loop,&QEventLoop::quit);
    loop.exec();

    return mDetailForSelectedLocation;
}

void NavigationServer::handleLocationDetail(const QVariant& results) {
    qDebug() << "[INTERFACE] NavigationServer::handleLocationDetail()";

    QVariantMap m       = results.toMap();
    QString countryCode = QStringLiteral("KOR");
    QString country     = QStringLiteral("South Korea");
    QString city        = m[QStringLiteral("city")].toString();
    QString state       = m[QStringLiteral("state")].toString();
    // QString statecode   = m[QStringLiteral("stateCode")].toString();
    QString phoneNumber = m[QStringLiteral("phone")].toString();
    QString address     = m[QStringLiteral("address")].toString();

    // update property
    mDetailForSelectedLocation.m_coordinate.m_latitude = mCurrentSetLocation.m_location.m_positionWGS84.m_latitude;
    mDetailForSelectedLocation.m_coordinate.m_longitude= mCurrentSetLocation.m_location.m_positionWGS84.m_longitude;
    mDetailForSelectedLocation.m_countryCode = countryCode;
    mDetailForSelectedLocation.m_countryName = country;

    if (!city.isEmpty())
        mDetailForSelectedLocation.m_cityName    = city;
    if (!state.isEmpty())
        mDetailForSelectedLocation.m_stateName   = state;
    if (!phoneNumber.isEmpty()) {
         mDetailForSelectedLocation.m_phoneNumber = phoneNumber;
         mCurrentSetLocation.m_phoneNumber = phoneNumber;
    }
    if (!address.isEmpty()) {
         mDetailForSelectedLocation.m_poiAddress  = address;
         mCurrentSetLocation.m_address  = address;
    }

    qDebug() << "ns, handle location detail: " << mCurrentSetLocation.m_phoneNumber
    << ", " << mCurrentSetLocation.m_address << ", " << address ;
    emit doneRequestDetailForSelectedLocation();
}

// feature out
/**
 * @brief NavigationServer::requestLocationDetailsForCoordinates
 * @param cityCoordinate [Structure contants longitude and latitude values(in doubles )]
 * @return
 *
 * Request will respond with location details belonging to given lat/long coordinates.
 *
 */
// NSLocationDetails NavigationServer::requestLocationDetailsForCoordinates(NSPositionWGS84 cityCoordinate) {
//     NSLocationDetails mNSLocationDetails;
//     return mNSLocationDetails;
// }

/**
 * @brief NavigationServer::requestFreeTextSearchResults
 * @param searchString
 * @param searchScope
 * @param destinationIndex [destinationIndex defines the index of the destination in the guided route]
 * @param startIndex [startIndex of the first element to be returned, use 0 for initial call]
 * @param numElements [numElements to be returned]
 * @return
 *
 * Requests the specified range of free text search results for the search string and scope.
 *
 */
NSAddressWithIdInfos NavigationServer::requestFreeTextSearchResults(
    const QString &searchString,
    uint searchScope,
    uchar destinationIndex,
    uint startIndex,
    uint numElements) {

    QString text       = searchString;
    QString searchWords;
    int scope          = (int)FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_COMBINED_WHOLE_DATA;
    int destination    = (int)destinationIndex;
    int start          = (int)startIndex;
    int cnt            = (int)numElements;
    int searchType     = 0;
    int category       = 0;


    // parse xml search string
    bool isCategorySearch=false;
    QDomDocument dom("searchtext");
    text.replace("<?xml>","<xml>");
    dom.setContent(text);
    QDomElement doc = dom.documentElement();
    if (doc.isNull()) {
        qDebug() << "navis, free text parse xml error";
    } else {
        QDomNode nod = doc.firstChild();
        if (!nod.isNull()) {
            QDomElement em = nod.toElement();
            if (!em.isNull()) {
                QString tag = em.tagName();
                isCategorySearch = (tag == QStringLiteral("CATEGORY") ||
                                    tag == QStringLiteral("CATEGORYID"));

                searchWords = em.text();

                QStringList categories{
                    QStringLiteral("주유소"),
                    QStringLiteral("충전소"),
                    QStringLiteral("주차장"),
                    QStringLiteral("은행"),
                    QStringLiteral("쇼핑"),
                    QStringLiteral("음식점"),
                    QStringLiteral("관광"),
                    QStringLiteral("병원"),
                    QStringLiteral("공공기관"),
                    QStringLiteral("금융"),
                    QStringLiteral("전기차 충전소")
                };

                if (tag == QStringLiteral("CATEGORY")) {
                    category = categories.indexOf(searchWords);
                } else if (tag == QStringLiteral("CATEGORYID")) {
                    int index = em.text().toInt();
                    category  = index % 11;
                    searchWords = categories.at(index%11);
                }
                qDebug() <<"navis, free text tag:"<<em.tagName()<<", val:"<<em.text();
            }
        }
    }

    searchType = isCategorySearch;

    // VR -> Category를 말하세요 -> 'SKT 주유소' 경우 주변검색으로 검색 안되서 통합검색으로
    if (isCategorySearch && category == -1) {
        searchType = 0;
    }


//    qDebug() << "searchString is" << text << "before replacement" << endl;
#if DEPRECATED_SINCE_19255
    QRegExp expression("<?(xml|XML)>|<(|/)([a-z]|[A-Z])*>");
    text.replace("<\?xml>", "");    // ? 문자를 정규표현식이 찾아주지 못해서 <?xml> 태그만 정규표현식이 아닌 방식으로 처리했음. 2019.3.11 by hheo
    text.replace(expression, "");
#endif
//    qDebug() << "searchString is" << text << "after replacement" << endl;

    // check it need to reset session using
    if (mSearchResultCache->diffSession(FreeTextSearchResultCache(searchWords, scope, searchType, destination, start, cnt))) {
        mSearchResultCache->resetSession(searchWords, scope, searchType, destination, start, cnt);
    }

    if (mSearchResultCache->mSearchText.isEmpty())
       return mSearchResultCache->result();

    emit notifyRequestFreeTextSearchResults(searchWords, searchScope, searchType, category, destinationIndex, startIndex, numElements);
    // wait until responsed
    QEventLoop loop;
    connect(this, SIGNAL(doneRequestFreeTextSearchResults()),
            &loop, SLOT(quit()));
    QTimer::singleShot(5000, &loop,&QEventLoop::quit);
    loop.exec();

    return mSearchResultCache->result();
}

/**
 * @brief Request will respond with a contextSwitch to Navigation Destination Free text search
 * @param locationType FTSLocationType , just use only FREETEXT_SEARCH_TYPE_COMBINED
 * @param locationSearchScope FreeTextSearchScope , just use only FreeTextSearchScope_FREETEXT_SEARCH_SCOPE_POI_WHOLE_DATA
 */
void NavigationServer::showDestinationFreeTextSearchInput(uint locationType, uint locationSearchScope, const QString& searchString)
{
    QStringList address = searchString.split(',');
#if (0)
    if (address.length() > 1 && !address.at(1).isEmpty()) {
        emit notifyShowDestinationFreeTextSearchInput(address.at(1));
    } else if (address.length() > 0 && !address.at(0).isEmpty()) {
        emit notifyShowDestinationFreeTextSearchInput(address.at(0));
    }
#else
    // stringlist의 모든 문자열을 합쳐서 하나의 주소를 구성한다.
    // 19.11.14 ymhong.
    auto result = address.join(" ");
    emit notifyShowDestinationFreeTextSearchInput(result);
#endif
}

void NavigationServer::undoFreeTextSearchElementSelection()
{
    qDebug() << "[INTERFACE] NavigationServer::undoFreeTextSearchElementSelection()";

    mCurrentSetLocation.m_address.clear();
    mCurrentSetLocation.m_location.m_positionWGS84.m_latitude  = 0;
    mCurrentSetLocation.m_location.m_positionWGS84.m_longitude = 0;
}

/**
 * handle for search results
 */
void NavigationServer::handleFreeTextSearchResult(const QVariant& results)
{
    QVariantList res = results.toList();
    int start = mSearchResultCache->mStartIndex;
    mSearchResultCache->mResultCount = res.size();

qDebug() << "FTS handle result: " << res.size();
    for (int i=0;i<res.size();i++) {
        QVariantList item = res.at(i).toList();
        NSAddressListElement address;
        // 2019-12-31 fix TMCRSM-4441
        QString poiName     = item.value(3,QString()).toString(); // poi standard name
        if (poiName.isEmpty())
            poiName = item.value(2, QString()).toString(); // poi Name

        address.m_data      = poiName; // Poi Name
        address.m_address   = item.value(4,QString()).toString(); // Address
        address.m_distance  = item.value(9,QString()).toString(); // Distance
        address.m_direction = 0; // No Support
        qDebug() <<"FTS result #" << i << address.m_data << "/ " << address.m_address;

        VSMPoint2f geo;
        VSMPoint2i world;
        world.x = item.value(12,0).toInt();
        world.y = item.value(13,0).toInt();
        bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84,geo);
        address.m_coordinates.m_longitude = geo.x;
        address.m_coordinates.m_latitude  = geo.y;

        // cache
        if (start+i < mSearchResultCache->size()) {
            address.m_id = start+i;
            mSearchResultCache->operator[](start+i) = address;
        }
        else {
            address.m_id = mSearchResultCache->size();
            mSearchResultCache->append(address);
        }

        qDebug() << i << ":" << address.m_address;
    }

    doneRequestFreeTextSearchResults();
}

void NavigationServer::sdsCheckAddress(const NSSDSAddress& requestedAddress)
{
    if (mAddressSearchResultCache->diffSession(AddressSearchResultCache(requestedAddress))) {
        mAddressSearchResultCache->resetSession(requestedAddress);
    }

    if (requestedAddress.size() == 0) {
        qWarning() << "check address canceled due to empty address";
        return;
    }

    emit notifySdsSearchAddress(requestedAddress);

    // 2019-3-12 bosch request not async but sync , It's so bad.
// wait until responsed
    QEventLoop loop;
    connect(this, SIGNAL(doneSdsCheckAddressResults()),
            &loop, SLOT(quit()));
    QTimer::singleShot(5000, &loop,&QEventLoop::quit);
    loop.exec();
}

void NavigationServer::handleAddressSearchResult(const QVariant& results)
{
    QVariantList res = results.toList();
    int start = mAddressSearchResultCache->mStartIndex;
    mAddressSearchResultCache->mResultCount = res.size();
    for (int i=0;i<res.size();i++) {
        QVariantList item = res.at(i).toList();
        NSAddressListElement address;
        address.m_data      = item.value(2,QString()).toString(); // Poi Name
        address.m_address   = item.value(4,QString()).toString(); // Address
        address.m_distance  = item.value(9,QString()).toString(); // Distance
        float degree   = item.value(10,0).toInt();
        for (int j=0;j<16;j++) {
            if (j*22.5 <= degree && degree < (j+1)*22.5) {
                address.m_direction = i;
                break;
            }
        }
        qDebug() << "navi, name: "<< address.m_data << "\n address: " << address.m_address
        << "\n distance:" << address.m_distance << "\n deg:" << address.m_direction;

        VSMPoint2f geo;
        VSMPoint2i world;
        world.x = item.value(12,0).toInt();
        world.y = item.value(13,0).toInt();
        bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84,geo);
        address.m_coordinates.m_longitude = geo.x;
        address.m_coordinates.m_latitude  = geo.y;

        // cache
        if (start+i < mAddressSearchResultCache->size()) {
            address.m_id = start+i;
            mAddressSearchResultCache->operator[](start+i) = address;
        }
        else {
            address.m_id = mAddressSearchResultCache->size();
            mAddressSearchResultCache->append(address);
        }

        // VR 주소 검색은 어차피 결과를 1개만 내야 하므로 아래와 같이 처리함. 2019.6.5 by hheo
        mAddressSearchResultCache->mPrimaryMax = item.value(42, 0).toInt();
        mAddressSearchResultCache->mPrimaryMin = item.value(43, 0).toInt();
        mAddressSearchResultCache->mSecondaryMax = item.value(44, 0).toInt();
        mAddressSearchResultCache->mSecondaryMin = item.value(45, 0).toInt();
    }

#if 0
    if (mAddressSearchResultCache->mSearchAddress.size() > 0) {
        qDebug() << "address element cnt: " << mAddressSearchResultCache->mSearchAddress.size();
        qDebug() << "address element : "<<mAddressSearchResultCache->mSearchAddress.last().m_data;
    }
    else {
        qDebug() << "search address invalid !!!";
        return;
    }
#endif

    // update sdsAddressWithOptions property only for result status
    NSSDSAddressWithOptions& address = mNSSDSAddressWithOptions;
    address.m_address.clear();
    address.m_addressElementType.clear();
    address.m_address = mAddressSearchResultCache->mSearchAddress;
    if (mAddressSearchResultCache->mResultCount == 0) {
        mAddressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = false;
        address.m_addressOptions.navigable = false;
        address.m_addressOptions.ambigious = false;
        address.m_addressOptions.removedInvalidEntry = false; // we can't verify whether the address is validated so take always false 2019-04-02
    } else if (mAddressSearchResultCache->mResultCount == 1) {
        mAddressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = false;
        address.m_addressOptions.navigable = true;
        address.m_addressOptions.ambigious = false;
        address.m_addressOptions.removedInvalidEntry = false;
    } else if ( mAddressSearchResultCache->mResultCount > 1 ) {
        mAddressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = true;
        address.m_addressOptions.navigable = false;
        address.m_addressOptions.ambigious = false; // 2019-04-01 it should be false in korea 
        address.m_addressOptions.removedInvalidEntry = false;
    } else { // exception
        mAddressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = false;
        address.m_addressOptions.navigable = false;
        address.m_addressOptions.ambigious = false;
        address.m_addressOptions.removedInvalidEntry = false;
    }

    // requested from bosch join workshop in seoul 2019-04-23
    static bool retryWithoutHouseNumber = false;
    if (retryWithoutHouseNumber) {
        retryWithoutHouseNumber = false;
        address.m_addressOptions.removedInvalidEntry = true;
    }

    enum {
      ElementCountyFilled   = 0x00000001,
      ElementStateFilled    = 0x00000010,
      ElementPlaceFilled    = 0x00000100,
      ElementRoadFilled     = 0x00001000,
      ElementHouseNumFilled = 0x00010000,
    };

    uint fullElementType = 0;
    for (int i=0;i<mAddressSearchResultCache->mSearchAddress.size();i++) {
        const NSSDSAddressElement& addr = mAddressSearchResultCache->mSearchAddress.at(i);
        SDSAddressElementType type = (SDSAddressElementType) addr.m_addressElementType;
        switch (type) {
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_COUNTRY:
            fullElementType |= ElementCountyFilled;
            break;
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE:
            fullElementType |= ElementStateFilled;
            break;
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE:
            fullElementType |= ElementPlaceFilled;
            break;
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD:
            fullElementType |= ElementRoadFilled;
            break;
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER:
            fullElementType |= ElementHouseNumFilled;
            break;
        }
    }

    // 2019-04-01 Yizhi:  1. The ambiguous flag will be always false in Korea currently, because there is no same road name in the same city/no same city name in on county.
    //                    2. houseNumberAvailable means whether there is house number in the street or not, houseNumberValid means whether the provided house number is in the range.
    //                    3. houseNumberValid can be only evaluated only when the houseNumberAvailable flag is TRUE.
    // Hyunsoo: so houseNumber is available under street in Korea.
    address.m_addressOptions.moreEntriesPossible   = (fullElementType != 0x00011111);
    // address.m_addressOptions.houseNumberAvailable  = (fullElementType & ElementCountyFilled) && (fullElementType & ElementStateFilled) && (fullElementType & ElementPlaceFilled) && !(fullElementType & ElementHouseNumFilled);
    // address.m_addressOptions.houseNumberValid      = (fullElementType & ElementHouseNumFilled) && (address.m_addressOptions.ambigious || address.m_addressOptions.navigable);
    bool filledUnderHouseNumber   =  (fullElementType & ElementCountyFilled)
                                  && (fullElementType & ElementStateFilled)
                                  && (fullElementType & ElementPlaceFilled)
                                  && (fullElementType & ElementRoadFilled);

    // it should be without house number, 2019-4-26 Request by Yizhi
    address.m_addressOptions.houseNumberAvailable  = filledUnderHouseNumber && !(fullElementType & ElementHouseNumFilled);
    address.m_addressOptions.houseNumberValid      = (fullElementType & ElementHouseNumFilled)
                                                     && filledUnderHouseNumber
                                                     && address.m_addressOptions.navigable;

    // 2019.3.25 Bosch SDS team & Neil : The refinement only used for address search, so if there are many results of house number under this street, then you do not need set ambiguous as true, you just set the housenumber available as true.
    // if (address.m_addressOptions.houseNumberAvailable) // 2019-04-01 it should be false in korea 
        //  address.m_addressOptions.ambigious = false;
     
#if 0 // decision: VR only use local search not for server one by 2019.5.23 bosch workshop in seoul
     // 2019-04-26, it could has multiple results under same street in the same city , by Yizhi request
    if (mAddressSearchResultCache->mResultCount > 1/* && address.m_addressOptions.houseNumberAvailable*/ )
        address.m_addressOptions.ambigious = true;
#endif

    // 2019.7.5 Searching CityCenter
    bool searchCityCenter = ( (fullElementType  &ElementStateFilled)  &&
                              (fullElementType  &ElementPlaceFilled)  &&
                             !(fullElementType  &ElementRoadFilled)
                              );

    if (searchCityCenter ||
            // 2019-11-14, fix TMCRSM-4106: System keeps giving same prompt after saying "No house number"
            address.m_addressOptions.houseNumberAvailable) {
        address.m_addressOptions.navigable            = mAddressSearchResultCache->mResultCount > 0;
        address.m_addressOptions.ambigious            = false;
        address.m_addressOptions.removedInvalidEntry  = false;
        address.m_addressOptions.moreEntriesPossible  = true;
        address.m_addressOptions.houseNumberAvailable = false;
        address.m_addressOptions.houseNumberValid     = false;
    }

    // next address elements
        // set next element to check if this is ambigious
    // if (value.m_addressOptions.ambigious == true) {  // fixed, https://tde.sktelecom.com/pms/browse/TMCRSM-2415
                                                        // it has nothing to do with that results
    if (mAddressSearchResultCache->mSearchAddress.size() > 0) {
        SDSAddressElementType type = (SDSAddressElementType) mAddressSearchResultCache->mSearchAddress.last().m_addressElementType;
        switch (type) {
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_COUNTRY:
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE);
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE);
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD);
            // address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER);
            break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE:
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE);
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD);
            break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE:
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD);
            // address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER);
            break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_ROAD:
            address.m_addressElementType.append(SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER);
            break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_HOUSENUMBER:
            address.m_addressElementType.clear();
            break;
            default:break;
        }

    }

    // 2019-04-02 it should be save because VR call setLocationWithSds soon when it is navigable
    if (address.m_addressOptions.navigable) {
//        selectSdsRefinement(mAddressSearchResultCache->count()-1);
          selectSdsRefinement(0);
    }

    // retry to search when that results has nothing with house number,
    // requested from bosch join workshop in seoul 2019-04-23
    if (mAddressSearchResultCache->mResultCount == 0 && fullElementType&ElementHouseNumFilled)
    {
       retryWithoutHouseNumber = true;
       NSSDSAddress address2   = mAddressSearchResultCache->mSearchAddress;
       address2.removeLast();
       emit doneSdsCheckAddressResults();
       sdsCheckAddress(address2);
       return;
    }

    setSdsAddressWithOptions(address);

    emit doneSdsCheckAddressResults();
}

void NavigationServer::setLocationWithCoordinates(NSPositionWGS84 location)
{
    auto lo = location.m_longitude;
    auto la = location.m_latitude;
    qDebug() << "[INTERFACE] NavigationServer::setLocationWithCoordinates( " << la << "," << lo << " )";

//    memset(&mCurrentSetLocation,0, sizeof(NSPOILocation));    QString 타입을 멤버변수로 포함하는 구조체는 memset으로 초기화하면 안 됨. 2019.6.18 by hheo
    mCurrentSetLocation.reset();
    mCurrentSetLocation.m_location.m_positionWGS84 = location;
    // emit notifySetLocationWithCoordinates(location);
}

void NavigationServer::setLocationWithDetails(NSPOILocation location)
{
    setLocation(location);
    // emit notifySetLocationWithDetails(location);
}

void NavigationServer::showAdjustCurrentLocationScreen()
{
    qDebug() << "[INTERFACE] NavigationServer::showAdjustCurrentLocationScreen()";

    emit toShowAdjustCurrentLocationScreen(mCurrentSetLocation.m_location.m_positionWGS84);
}

NSPositionWGS84 NavigationServer::currentSetLocation() const
{
    return mCurrentSetLocation.m_location.m_positionWGS84;
}

void NavigationServer::showAndSetLocationWithDetails(NSPOILocation location)
{
    qDebug() << "[INTERFACE] NavigationServer::showAndSetLocationWithDetails()";
    setLocation(location);
    emit notifyShowAndSetLocationWithDetails(location);
}

void NavigationServer::setLocationWithDestinationMemoryEntry(qulonglong id)
{
    qDebug() << "[INTERFACE] NavigationServer::setLocationWithDestinationMemoryEntry(" << id << ")";

    // 2019-03-12  Don't ui flow, just keep it in cache for all setLocationWith*
    #if 0
    emit notifySetLocationWithDestinationMemoryEntry(id);
    #else
    NSPOILocation location = toLocationWithDestinationMemoryEntry(mMemoryEntryCategory, id);
    #endif

    setLocation(location);
}

void NavigationServer::setLocationWithFreeTextSearchInput(uint id)
{
    if (mSearchResultCache->size() <= id) {
        qWarning() << "setLocation with FTS, invalid search item id: " << id;
        return;
    }
    NSPOILocation location;
    location.m_address                  = mSearchResultCache->at(id).m_address;
    location.m_location.m_name          = mSearchResultCache->at(id).m_data;
    location.m_location.m_positionWGS84 = mSearchResultCache->at(id).m_coordinates;
    location.m_location.m_valid         = true;
    setLocation(location);

    // 2019-03-14 requested from bosch
    // notifyShowAndSetLocationWithDetails(location);
}

void NavigationServer::setLocationWithSdsInput()
{
    qDebug() << "[INTERFACE] NavigationServer::setLocationWithSdsInput()";
//    emit notifySetLocationWithDetails(mCurrentSetLocation);  // lckxx fix TMCRSM-3927, TMCRSM-3925
}

void NavigationServer::setLocation(const NSPOILocation& location)
{
    auto lo = location.m_location.m_positionWGS84.m_longitude;
    auto la = location.m_location.m_positionWGS84.m_latitude;
    qDebug() << "[INTERFACE] NavigationServer::setLocation( " << la << ", " << lo << " )";

    mCurrentSetLocation = location;
    mDetailForSelectedLocation.m_poiAddress  = location.m_address;
    mDetailForSelectedLocation.m_phoneNumber = location.m_phoneNumber;
    mDetailForSelectedLocation.m_coordinate  = location.m_location.m_positionWGS84;

    qDebug() << "navi serv location updated:"
             << location.m_address
             << ", "
             << location.m_location.m_positionWGS84.m_longitude
             << ", "
             << location.m_location.m_positionWGS84.m_latitude;

    // emit notifySetLocationWithDetails(location);
}

/**
 * @brief NavigationServer::sdsGetHouseNumberPattern
 * @param maxHouseNumber
 * @param patterns
 * @return
 *
 * Request will get house number patterns and the minimum and maximum values of the current SDSAddress.
 *
 */
QString NavigationServer::sdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns) {
//    re generator
//    QString ret = toSdsGetHouseNumberPattern(maxHouseNumber, patterns);
    qDebug() << "hhtest sdsGetHouseNumberPattern]" << mAddressSearchResultCache->mPrimaryMin << "-" << mAddressSearchResultCache->mSecondaryMin << " ~ " << mAddressSearchResultCache->mPrimaryMax << "-" << mAddressSearchResultCache->mSecondaryMax;
    QString ret = QString::asprintf("%d-%d", mAddressSearchResultCache->mPrimaryMin, mAddressSearchResultCache->mSecondaryMin);
    maxHouseNumber = QString::asprintf("%d-%d", mAddressSearchResultCache->mPrimaryMax, mAddressSearchResultCache->mSecondaryMax);

    return ret;
}

/**
 * @brief NavigationServer::sdsGetRefinementList
 * @return NSRefinementWithIdInfos [Array of RefinementListElement]
 *
 * Request will get refinement list for the validated address.
 *
 */
NSRefinementWithIdInfos NavigationServer::sdsGetRefinementList() {
    NSRefinementWithIdInfos refineList;
    NSAddressWithIdInfos    searchResult = mAddressSearchResultCache->result();
    for (int i=0;i<searchResult.size();i++) {
        NSRefinementListElement ref;
        ref.m_data = searchResult.at(i).m_data;
        ref.m_id   = searchResult.at(i).m_id;
        ref.m_direction = searchResult.at(i).m_direction;
        ref.m_distance  = searchResult.at(i).m_distance;
        refineList.append(ref);
    }

    return refineList;
}


void NavigationServer::selectSdsRefinement(uint id)
{
    if (!mAddressSearchResultCache) {
        qWarning() << "address search cache is not found";
        return;
    }

    if (id >= mAddressSearchResultCache->size()) {
        qDebug() << "invalid select refinement id: " << id;
        return;
    }

#if 0 // check it needs to update the property 
    NSSDSAddressWithOptions& address = mNSSDSAddressWithOptions;
    address.m_address = mAddressSearchResultCache->mSearchAddress;
    address.m_addressOptions.navigable = true;
    address.m_addressOptions.ambigious = false;
    setSdsAddressWithOptions(address);
#endif

    NSPOILocation location;
    location.m_address = mAddressSearchResultCache->at(id).m_address;
    location.m_location.m_name = mAddressSearchResultCache->at(id).m_data;
    location.m_location.m_positionWGS84 = mAddressSearchResultCache->at(id).m_coordinates;

    for (int i=0; i<mAddressSearchResultCache->mSearchAddress.size();i++) {
        uint elmType = mAddressSearchResultCache->mSearchAddress.at(i).m_addressElementType;
        switch (elmType) {
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_COUNTRY:
            mDetailForSelectedLocation.m_countryName = mAddressSearchResultCache->mSearchAddress.at(i).m_data;
            mDetailForSelectedLocation.m_countryCode = QStringLiteral("KOR");
            break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE:
            mDetailForSelectedLocation.m_stateName   = mAddressSearchResultCache->mSearchAddress.at(i).m_data;
            break;
            case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE:
            mDetailForSelectedLocation.m_cityName = mAddressSearchResultCache->mSearchAddress.at(i).m_data;
            break;
            default:break;
        }
    }

    setLocation(location);
}

uint NavigationServer::routeRecalculationReason() const
{
    return mRouteRecalculationReason;
}


void NavigationServer::setRouteRecalculationReason(uint recalReason)
{
//    if (mRouteRecalculationReason != static_cast<RouteRecalculationReason>(recalReason)) {
        mRouteRecalculationReason = static_cast<RouteRecalculationReason>(recalReason);
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("routeRecalculationReason")] = QVariant::fromValue(recalReason);
        notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
//    }
}


uint NavigationServer::destinationReachedStatus() const
{
    return mDestinationReachedStatus;
}

 void NavigationServer::setDestinationReachedStatus(uint reachedStatus)
 {
     if (mDestinationReachedStatus != (DestinationReachedStatus)reachedStatus){
         mDestinationReachedStatus = (DestinationReachedStatus)reachedStatus;
         QVariantMap changedProperties;
         changedProperties[QStringLiteral("destinationReachedStatus")] = QVariant::fromValue(reachedStatus);
         notifyPropertiesChanged(NavigationServiceInterface, changedProperties, QStringList());
     }
 }


NSNavDataPartNumber NavigationServer::getNavDataPartNumber()
{
    NSNavDataPartNumber partNum;
    partNum.result = NO_PART_NUMBER_FOR_DEVICE_SET;
    partNum.partNumber = "0000000000";
    QFile partnumber(SKT::Environment::getMapDataPath()+QDir::separator()+QStringLiteral("TmapNavi/partnumber"));
    if (partnumber.exists()) {
        bool ok = partnumber.open(QFile::ReadOnly);
        if (ok) {
            QTextStream ts(&partnumber);
            ts >> partNum.partNumber;
            if (!partNum.partNumber.isEmpty()) {
                partNum.result = SUCCESS;
            }
            partnumber.close();
        }
    }
    qDebug() << "navi service part number: " << partNum.partNumber;

    return partNum;
}


void NavigationServer::doStartGuidance()
{
    qDebug() << "[INTERFACE] NavigationServer::doStartGuidance()";

    if (mCurrentSetLocation.m_location.m_positionWGS84.m_latitude != 0 &&
        mCurrentSetLocation.m_location.m_positionWGS84.m_longitude != 0 ) {
        emit notifySetLocationWithDetails(mCurrentSetLocation);
        emit startGuidance();
    }
}

void NavigationServer::doCancelRouteGuidance()
{
    qDebug() << "[INTERFACE] NavigationServer::doCancelRouteGuidance()";

    emit cancelRouteGuidance();
}
