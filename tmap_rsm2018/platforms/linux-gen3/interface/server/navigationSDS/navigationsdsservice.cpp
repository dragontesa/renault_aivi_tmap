#include "navigationsdsservice.h"
#include "navigationsdsserviceadaptor.h"
#include "build.h"
#include "VSM_Util.h"

static const QString serviceName = QStringLiteral("App.Core.AppHmi_Navigation");
static const QString navigationSDSObjectPath = QStringLiteral("/com/bosch/cm/ai/navigationsds");
static const QString dBusPropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString dBusPropertiesChangedSignal = QStringLiteral("PropertiesChanged");
static const QString NavigationSDSServiceInterface = QStringLiteral("org.bosch.cm.navigation.NavigationSDSService");

#define SEARCH_RESULT_MAX 2000

class AddressSearchResultCache: public QVector<NSAddressListElement>
{
    friend class NavigationSDSService;
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
        mStartIndex = 0;
        mResultMax  = 0;
        mResultCount = 0;
        mPrimaryMax = 9999;
        mPrimaryMin = 1;
        mSecondaryMax = 0;
        mSecondaryMin = 0;
        clear();
        reserve(SEARCH_RESULT_MAX);
    }

    void resetSession(const NSSDSAddress& address) {
        mSearchAddress.clear();
        mSearchAddress = address;
        mStartIndex = 0;
        mResultMax  = 0;
        mResultCount = 0;
        mPrimaryMax = 9999;
        mPrimaryMin = 1;
        mSecondaryMax = 0;
        mSecondaryMin = 0;
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

NavigationSDSService::NavigationSDSService(QObject* parent)
    : QObject(parent)
    , m_adaptor(new NavigationSDSServiceAdaptor(this))
    , m_registered(false)
    , m_maxWaypointCount(3)
    , m_addressSearchResultCache(new AddressSearchResultCache)
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "Failed to connect to DBus";
    } else if (!connection.registerObject(navigationSDSObjectPath, this)) {
        qDebug() << "Failed to register object path";
    }
}

NavigationSDSService::~NavigationSDSService()
{
    unregisterService();
}

void NavigationSDSService::registerService()
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

void NavigationSDSService::unregisterService()
{
    if (m_registered) {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.unregisterService(serviceName);
        m_registered = false;
    }
}

// TODO: sending too frequently?
void NavigationSDSService::notifyPropertiesChanged(const QString& interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) const
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "Failed to connect to DBus";
        return;
    }

    QDBusMessage message = QDBusMessage::createSignal(navigationSDSObjectPath,
                                                      dBusPropertiesInterface,
                                                      dBusPropertiesChangedSignal);

    QList<QVariant> arguments;
    arguments << QVariant(interfaceName) << QVariant(changedProperties) << QVariant(invalidatedProperties);
    message.setArguments(arguments);

    if (!connection.send(message)) {
        qDebug() << "Failed to send DBus property notification signal";
    }
}

/**
 * @brief sdsAddressWithOptions
 * @return NSSDSAddressWithOptions
 *
 * Attribute provides the current SDS address entry.
 *
 */
NSSDSAddressWithOptions NavigationSDSService::sdsAddressWithOptions() const {
    return m_sdsAddressWithOptions;
}

void NavigationSDSService::setSdsAddressWithOptions(const NSSDSAddressWithOptions &value) {
    QVariantMap changedProperties;
    changedProperties[QStringLiteral("sdsAddressWithOptions")] = QVariant::fromValue(value);
    notifyPropertiesChanged(NavigationSDSServiceInterface, changedProperties, QStringList());
}

/**
 * @brief positionStatusInfo
 * @return NSPositionStatusInfo
 *
 * Attribute provides current vehicle position status information.
 *
 */
NSPositionStatusInfo NavigationSDSService::positionStatusInfo() const {
//    qDebug() << "NavigationSDSService::: called NavigationSDSService::positionStatusInfo.";
    return m_positionStatusInfo;
}

void NavigationSDSService::setPositionStatusInfo(const NSPositionStatusInfo &info) {
//    qDebug() << "NavigationSDSService::: called NavigationSDSService::setPositionStatusInfo.";
    if (info != m_positionStatusInfo) {
        m_positionStatusInfo = info;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("positionStatusInfo")] = QVariant::fromValue(m_positionStatusInfo);
        notifyPropertiesChanged(NavigationSDSServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief positionInfo
 * @return NSPositionInfo
 *
 * Attribute provides current vehicle position information.
 *
 */
NSPositionInfo NavigationSDSService::positionInfo() const {
//    qDebug() << "NavigationSDSService::: called NavigationSDSService::positionInfo.";
    return m_positionInfo;
}

void NavigationSDSService::setPositionInfo(const NSPositionInfo &info) {
//    qDebug() << "NavigationSDSService::: called NavigationSDSService::setPositionInfo.";
    if (info != m_positionInfo) {
        m_positionInfo = info;
        QVariantMap changedProperties;
        changedProperties[QStringLiteral("positionInfo")] = QVariant::fromValue(m_positionInfo);
        notifyPropertiesChanged(NavigationSDSServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief NavigationSDSService::sdsCheckAddress
 *
 * Request will validate the given address, as result it update the attribute sdsAddressWithOptions.
 *
 */
void NavigationSDSService::sdsCheckAddress(const NSSDSAddress& requestedAddress)
{
    if (m_addressSearchResultCache->diffSession(AddressSearchResultCache(requestedAddress))) {
        m_addressSearchResultCache->resetSession(requestedAddress);
    }

    if (requestedAddress.size() == 0) {
        qWarning() << "sds check address canceled due to empty address";
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

/**
 * @brief NavigationSDSService::getMaxWaypointCount
 * @return uint
 *
 * Returns max number of waypoint base on project variant.
 *
 */
uint NavigationSDSService::getMaxWaypointCount() const {
//    qDebug() << "NavigationSDSService::: called NavigationSDSService::getMaxWaypointCount.";
    return m_maxWaypointCount;
}

/**
 * @brief NavigationSDSService::sdsGetHouseNumberPattern
 * @return QString
 *
 * Request will get house number patterns and the minimum and maximum values of the current SDSAddress.
 *
 */
QString NavigationSDSService::sdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns) const {
    qDebug() << "hhtest sdsGetHouseNumberPattern]" << m_addressSearchResultCache->mPrimaryMin << "-" << m_addressSearchResultCache->mSecondaryMin << " ~ " << m_addressSearchResultCache->mPrimaryMax << "-" << m_addressSearchResultCache->mSecondaryMax;
    QString ret = QString::asprintf("%d-%d", m_addressSearchResultCache->mPrimaryMin, m_addressSearchResultCache->mSecondaryMin);
    maxHouseNumber = QString::asprintf("%d-%d", m_addressSearchResultCache->mPrimaryMax, m_addressSearchResultCache->mSecondaryMax);

    return ret;
}

/**
 * @brief NavigationSDSService::sdsGetRefinementList
 * @return NSRefinementWithIdInfos [Array of RefinementListElement]
 *
 * Request will get refinement list for the validated address.
 *
 */
NSRefinementWithIdInfos NavigationSDSService::sdsGetRefinementList(uint sdsAddressElementType) const {
//    qDebug() << "NavigationSDSService::: called NavigationSDSService::sdsGetRefinementList.";
    NSRefinementWithIdInfos refinementList;
    NSAddressWithIdInfos    searchResult = m_addressSearchResultCache->result();
    for (int ii = 0; ii < searchResult.size(); ii++) {
        NSRefinementListElement ref;
        ref.m_data      = searchResult.at(ii).m_data;
        ref.m_id        = searchResult.at(ii).m_id;
        ref.m_direction = searchResult.at(ii).m_direction;
        ref.m_distance  = searchResult.at(ii).m_distance;
        refinementList.append(ref);
    }

    return refinementList;
}

/**
 * @brief Request will select the refinement with the id from refinement list and add internal to SDSAddress. As result it update the attribute sdsAddressWithOptions.
 *
 */
void NavigationSDSService::selectSdsRefinement(uint id)
{
    if (!m_addressSearchResultCache) {
        qWarning() << "address search cache is not found";
        return;
    }

    if (id >= m_addressSearchResultCache->size()) {
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
    location.m_address = m_addressSearchResultCache->at(id).m_address;
    location.m_location.m_name = m_addressSearchResultCache->at(id).m_data;
    location.m_location.m_positionWGS84 = m_addressSearchResultCache->at(id).m_coordinates;

    for (int ii = 0; ii < m_addressSearchResultCache->mSearchAddress.size(); ii++) {
        uint elmType = m_addressSearchResultCache->mSearchAddress.at(ii).m_addressElementType;
        switch (elmType) {
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_COUNTRY:
//            mDetailForSelectedLocation.m_countryName = m_addressSearchResultCache->mSearchAddress.at(ii).m_data;
//            mDetailForSelectedLocation.m_countryCode = QStringLiteral("KOR");
            break;
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_STATE:
//            mDetailForSelectedLocation.m_stateName   = m_addressSearchResultCache->mSearchAddress.at(ii).m_data;
            break;
        case SDSAddressElementType_SDS_ADDRESS_ELEMENT_TYPE_PLACE:
//            mDetailForSelectedLocation.m_cityName = m_addressSearchResultCache->mSearchAddress.at(ii).m_data;
            break;
        default:
            break;
        }
    }

    setLocation(location);
}

/**
 * @brief Request to set the latitude and longitude value. TCU,SXM,VR
 *
 */
void NavigationSDSService::setLocationWithSdsInput()
{
    emit notifySetLocationWithDetails(m_currentSetLocation);
}

void NavigationSDSService::handleAddressSearchResult(const QVariant& results)
{
    QVariantList res = results.toList();
    int start = m_addressSearchResultCache->mStartIndex;
    m_addressSearchResultCache->mResultCount = res.size();

    for (int ii = 0; ii < res.size(); ii++) {
        QVariantList item = res.at(ii).toList();
        NSAddressListElement address;
        address.m_data      = item.value(2, QString()).toString(); // Poi Name
        address.m_address   = item.value(4, QString()).toString(); // Address
        address.m_distance  = item.value(9, QString()).toString(); // Distance
        float degree        = item.value(10, 0).toInt();
        for (uint jj = 0; jj < 16; jj++) {
            if (jj * 22.5f <= degree && degree < (jj + 1) * 22.5f) {
                address.m_direction = jj;
        break;
            }
        }
        qDebug() << "navi, name: "<< address.m_data << "\n address: " << address.m_address
        << "\n distance:" << address.m_distance << "\n deg:" << address.m_direction;

        VSMPoint2f geo;
        VSMPoint2i world;
        world.x = item.value(12, 0).toInt();
        world.y = item.value(13, 0).toInt();
        bool ok = VSMMap_Util_ConvertWorldTo(world, VSM_COORDINATE_WGS84, geo);
        address.m_coordinates.m_longitude = geo.x;
        address.m_coordinates.m_latitude  = geo.y;

        // cache
        if (start + ii < m_addressSearchResultCache->size()) {
            address.m_id = start + ii;
            m_addressSearchResultCache->operator[](start + ii) = address;
        }
        else {
            address.m_id = m_addressSearchResultCache->size();
            m_addressSearchResultCache->append(address);
        }

        // VR 주소 검색은 어차피 결과를 1개만 내야 하므로 아래와 같이 처리함. 2019.6.5 by hheo
        m_addressSearchResultCache->mPrimaryMax = item.value(42, 0).toInt();
        m_addressSearchResultCache->mPrimaryMin = item.value(43, 0).toInt();
        m_addressSearchResultCache->mSecondaryMax = item.value(44, 0).toInt();
        m_addressSearchResultCache->mSecondaryMin = item.value(45, 0).toInt();
    }
#if 0
    if (m_addressSearchResultCache->mSearchAddress.size() > 0) {
        qDebug() << "address element cnt: " << m_addressSearchResultCache->mSearchAddress.size();
        qDebug() << "address element : " << m_addressSearchResultCache->mSearchAddress.last().m_data;
    }
    else {
        qDebug() << "search address invalid !!!";
        return;
    }
#endif

    // update sdsAddressWithOptions property only for result status
    NSSDSAddressWithOptions& address = m_sdsAddressWithOptions;
    address.m_address.clear();
    address.m_addressElementType.clear();
    address.m_address = m_addressSearchResultCache->mSearchAddress;
    if (m_addressSearchResultCache->mResultCount == 0) {
        m_addressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = false;
        address.m_addressOptions.navigable = false;
        address.m_addressOptions.ambigious = false;
        address.m_addressOptions.removedInvalidEntry = false; // we can't verify whether the address is validated so take always false 2019-04-02
    } else if (m_addressSearchResultCache->mResultCount == 1) {
        m_addressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = false;
        address.m_addressOptions.navigable = true;
        address.m_addressOptions.ambigious = false;
        address.m_addressOptions.removedInvalidEntry = false;
    } else if ( m_addressSearchResultCache->mResultCount > 1 ) {
        m_addressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = true;
        address.m_addressOptions.navigable = false;
        address.m_addressOptions.ambigious = false; // 2019-04-01 it should be false in korea
        address.m_addressOptions.removedInvalidEntry = false;
    } else { // exception
        m_addressSearchResultCache->mSearchAddress.last().m_elementOptions.m_isAmbigious = false;
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
    for (int ii = 0; ii < m_addressSearchResultCache->mSearchAddress.size(); ii++) {
        const NSSDSAddressElement& addr = m_addressSearchResultCache->mSearchAddress.at(ii);
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
    // next address elements
    // set next element to check if this is ambigious
    // if (value.m_addressOptions.ambigious == true) {  // fixed, https://tde.sktelecom.com/pms/browse/TMCRSM-2415
                                                        // it has nothing to do with that results
    if (m_addressSearchResultCache->mSearchAddress.size() > 0) {
        SDSAddressElementType type = (SDSAddressElementType) m_addressSearchResultCache->mSearchAddress.last().m_addressElementType;
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
        default:
            break;
        }
    }

    // 2019-04-02 it should be save because VR call setLocationWithSds soon when it is navigable
    if (address.m_addressOptions.navigable) {
        selectSdsRefinement(m_addressSearchResultCache->count()-1);
    }

    // retry to search when that results has nothing with house number,
    // requested from bosch join workshop in seoul 2019-04-23
    if (m_addressSearchResultCache->mResultCount == 0 && fullElementType & ElementHouseNumFilled)
    {
       retryWithoutHouseNumber = true;
       NSSDSAddress address2 = m_addressSearchResultCache->mSearchAddress;
       address2.removeLast();
       emit doneSdsCheckAddressResults();
       sdsCheckAddress(address2);
       return;
    }

    setSdsAddressWithOptions(address);

    emit doneSdsCheckAddressResults();
}

void NavigationSDSService::setLocation(const NSPOILocation& location)
{
    m_currentSetLocation = location;
    //mDetailForSelectedLocation.m_poiAddress  = location.m_address;
    //mDetailForSelectedLocation.m_phoneNumber = location.m_phoneNumber;
    //mDetailForSelectedLocation.m_coordinate  = location.m_location.m_positionWGS84;

    qDebug() << "NavigationSDSService location updated:"
             << location.m_address
             << ", "
             << location.m_location.m_positionWGS84.m_longitude
             << ", "
             << location.m_location.m_positionWGS84.m_latitude;

//    emit notifySetLocationWithDetails(location);
}
