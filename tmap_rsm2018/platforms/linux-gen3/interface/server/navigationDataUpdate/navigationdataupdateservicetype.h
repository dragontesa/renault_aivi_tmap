#ifndef NAVIGATIONDATAUPDATESERVICETYPE_H
#define NAVIGATIONDATAUPDATESERVICETYPE_H

#include <QDBusArgument>

/**
 * @brief The NavigationDataUpdateStatus enum
 *
 * Enum describing the status of NavDataUpdate (checking, downloading, installing, etc…).
 *
 */
enum NavigationDataUpdateStatus {
    NavDataUpdateState_IDLE = 0,
    NavDataUpdateState_AVAILABLE_UPDATES_STARTED,
    NavDataUpdateState_AVAILABLE_UPDATES_IN_PROGRESS,
    NavDataUpdateState_AVAILABLE_UPDATES_FINISHED,
    NavDataUpdateState_AVAILABLE_UPDATES_ABORTED_BY_ERROR,
    NavDataUpdateState_AVAILABLE_UPDATES_ABORTED_BY_USER,
    NavDataUpdateState_DOWNLOAD_STARTED,
    NavDataUpdateState_DOWNLOAD_IN_PROGRESS,
    NavDataUpdateState_DOWNLOAD_STOPPED,
    NavDataUpdateState_DOWNLOAD_FINISHED,
    NavDataUpdateState_DOWNLOAD_ABORTED_BY_ERROR,
    NavDataUpdateState_DOWNLOAD_ABORTED_BY_USER,
    NavDataUpdateState_INSTALLATION_STARTED,
    NavDataUpdateState_INSTALLATION_IN_PROGRESS,
    NavDataUpdateState_INSTALLATION_STOPPED,
    NavDataUpdateState_INSTALLATION_FINISHED,
    NavDataUpdateState_INSTALLATION_ABORTED_BY_USER,
    NavDataUpdateState_UPDATE_ABORTED_BY_USER, // must be
    NavDataUpdateState_DOWNLOAD_UPDATE_SUCCESSFULLY_FINISHED,
    NavDataUpdateState_USB_REMOVED, // must be
    NavDataUpdateState_UPDATE_SOURCE_ERROR, //must be
    NavDataUpdateState_FULL_UPDATE_REQUIRED,
    NavDataUpdateState_UNKNOWN,
    NavDataUpdateState_FAILED_NO_UPDATES_FOUND=100,
    NavDataUpdateState_FAILED_INVALID_USB_DATA,
    NavDataUpdateState_FAILED_OTHERS,
    NavDataUpdateState_TIMEOUT_NO_USB
};

/**
 * @brief The NavigationDataDeleteStatus enum
 *
 * Enum describing the acknowledgement whether Navigation accept to process the request of delete map data
 *
 */
enum NavigationDataDeleteStatus {
    NavDataDeleteStatus_MAPDATA_DELETE_STARTED = 0,
    NavDataDeleteStatus_MAPDATA_DELETE_IN_PROGRESS,
    NavDataDeleteStatus_MAPDATA_DELETE_FINISHED,
    NavDataDeleteStatus_MAPDATA_DELETE_FAILED,
};


/**
 * @brief The NavigationDataDeleteCannotStatus enum
 * Internal Status types
 */
enum NavigationDataDeleteCannotStatus {
    NavDataDeleteCannotStatus_NoMapdata = 100,
    NavDataDeleteCannotStatus_FilePermissionDenied,
    NavDataDeleteCannotStatus_BusyWorking,
    NavDataDeleteCannotStatus_Others,
};

/**
 * @brief The NavigationDataUpdateServiceAck enum
 *
 * Enum describing the acknowledgement whether Navigation accept to process the request
 *
 */
enum NavigationDataUpdateServiceAck {
    NavDataUpdateServiceAck_ACCEPTED = 0,
    NavDataUpdateServiceAck_DECLINED_CANNOT_PROCESS,
    NavDataUpdateServiceAck_DECLINED_REQUEST_ACTIVE,
};

/**
 * @brief The NavigationDataDeleteServiceAck enum
 *
 * Enum describing the acknowledgement whether Navigation accept to process the request
 *
 */
enum NavigationDataDeleteServiceAck {
   NavDataDeleteServiceAck_ACCEPTED = 0,
   NavDataDeleteServiceAck_DECLINED_NO_MAP_DATA,
   NavDataDeleteServiceAck_DECLINED_CANNOT_PROCESS,
   NavDataDeleteServiceAck_DECLINED_REQUEST_ACTIVE,
};


/**
 * @brief The NavigationDataUpdateStatusInfo struct
 */
struct NavigationDataUpdateStatusInfo {
    NavigationDataUpdateStatusInfo() : m_status(NavDataUpdateState_IDLE), m_progressValue(0), m_remainingTimeInSecond(0), m_productName(""), m_regionName("") { }
    uint m_status;
    uchar m_progressValue;
    uint m_remainingTimeInSecond;
    QString m_productName;
    QString m_regionName;

    bool operator==(const NavigationDataUpdateStatusInfo &other) const {
        if (m_status == other.m_status &&
                m_progressValue == other.m_progressValue &&
                m_remainingTimeInSecond == other.m_remainingTimeInSecond &&
                m_productName == other.m_productName &&
                m_regionName == other.m_regionName) {
            return true;
        }
        return false;
    }

    bool operator!=(const NavigationDataUpdateStatusInfo &other) const {
        return !(*this == other);
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NavigationDataUpdateStatusInfo& info);
const QDBusArgument &operator>>(const QDBusArgument &argument, NavigationDataUpdateStatusInfo& info);

Q_DECLARE_METATYPE(NavigationDataUpdateStatusInfo)

/**
 * @brief The NavigationDataDeleteStatusInfo struct
 */
struct NavigationDataDeleteStatusInfo {
    NavigationDataDeleteStatusInfo() : m_status(NavDataDeleteStatus_MAPDATA_DELETE_STARTED), m_progressValue(0), m_remainingTimeInSecond(0), m_productName(""), m_regionName("") { }
    uint m_status;
    uchar m_progressValue;
    uint m_remainingTimeInSecond;
    QString m_productName;
    QString m_regionName;

    bool operator==(const NavigationDataDeleteStatusInfo &other) const {
        if (m_status == other.m_status &&
                m_progressValue == other.m_progressValue &&
                m_remainingTimeInSecond == other.m_remainingTimeInSecond &&
                m_productName == other.m_productName &&
                m_regionName == other.m_regionName) {
            return true;
        }
        return false;
    }

    bool operator!=(const NavigationDataDeleteStatusInfo &other) const {
        return !(*this == other);
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NavigationDataDeleteStatusInfo& info);
const QDBusArgument &operator>>(const QDBusArgument &argument, NavigationDataDeleteStatusInfo& info);

Q_DECLARE_METATYPE(NavigationDataDeleteStatusInfo)



/**
 * @brief The NavigationDataUpdateServiceType class
 */
class NavigationDataUpdateServiceType
{
public:
    static void registerMetaTypes();
};

#endif // NAVIGATIONDATAUPDATESERVICETYPE_H
