#ifndef NAVIGATIONEXTTYPE_H
#define NAVIGATIONEXTTYPE_H

#include <QDBusArgument>

/**
 * @brief The NaviAnnouncementType enum
 */
enum NaviAnnouncementType {
    NAVI_ANNOUNCEMENT_TYPE_PURE_TEXT = 0,
    NAVI_ANNOUNCEMENT_TYPE_RECORD_FILE,
    NAVI_ANNOUNCEMENT_TYPE_PCM,
};

/**
 * @brief The NaviAnnouncementType enum
 */
enum NaviAnnouncementAudioSource {
    NAVI_ANNOUNCEMENT_AUDIOSOURCE_REGULAR = 0,
    NAVI_ANNOUNCEMENT_AUDIOSOURCE_SDS,
};

/**
 * @brief The NaviAnnouncementStatus enum
 */
enum NaviAnnouncementStatus {
    NAVI_ANNOUNCEMENT_STATUS_INVALID = 0,
    NAVI_ANNOUNCEMENT_STATUS_PLAY_FAIL,
    NAVI_ANNOUNCEMENT_STATUS_PLAY_OK,
    NAVI_ANNOUNCEMENT_STATUS_ABORT_FAIL,
    NAVI_ANNOUNCEMENT_STATUS_ABORT_OK,
};

/**
 * @brief The NaviSigNaviTTSStatus enum
 */
enum NaviSigNaviTTSStatus {
    NAVI_TTS_STATUS_READY = 0,
    NAVI_TTS_STATUS_NOT_READY,
};

/**
 * @brief The ProfileType enum
 */
enum ProfileType {
    PROFILE_TYPE_EMPTY = 0,
    PROFILE_TYPE_GUEST,
    PROFILE_TYPE_NORMAL,
    PROFILE_TYPE_PROTECTED,
    PROFILE_TYPE_LINKED,
};


/**
 * @brief The ProfileStatus enum
 */
enum ProfileStatus {
    PROFILE_UNLOCKED = 0,
    PROFILE_LOCKED,
    PROFILE_LOADING,
    PROFILE_INITIALIZING,
};

/**
 * @brief The ProfileServerResult enum
 */
enum ProfileServerResult {
    PROFILE_SERVER_RESULT_SUCCESS = 0,
    PROFILE_SERVER_RESULT_PROFILE_ONGOING,
    PROFILE_SERVER_RESULT_PROFILE_FAILED,
    PROFILE_SERVER_RESULT_PROFILE_INTIALIZING,
    PROFILE_SERVER_RESULT_PROFILE_WRONG_PARAMETER,
    PROFILE_SERVER_RESULT_PROFILE_ACCESS_DENIED,
    PROFILE_SERVER_RESULT_PROFILE_IMAGE_FILE_ERROR,
};

/**
 * @brief The ProfileclientResult enum
 */
enum ProfileClientResult {
    PROFILE_CLIENT_RESULT_SUCCESS = 0,
    PROFILE_FAILED,
};

/**
 * @brief The ProfileDataChangedResult enum
 */
enum ProfileDataChangedResult {
    PROFILE_DATA_CHANGED_RESULT_PREPARED = 0,
    PROFILE_DATA_CHANGED_RESULT_PROFILE_CHANGED,
    PROFILE_DATA_CHANGED_RESULT_PROFILE_COPIED,
    PROFILE_DATA_CHANGED_RESULT_PROFILE_DELETED,
    PROFILE_DATA_CHANGED_RESULT_FAILED,
};

/**
 * @brief The ProfileDataChangedReason enum
 */
enum ProfileDataChangedReason {
    PROFILE_DATA_CHANGED_REASON_IDLE,
    PROFILE_DATA_CHANGED_REASON_PROFILE_RESET,
    PROFILE_DATA_CHANGED_REASON_PROFILE_CHANGED,
    PROFILE_DATA_CHANGED_REASON_DOWNLOAD,
    PROFILE_DATA_CHANGED_REASON_PREPARE,
};


/**
 * @brief NaviMethSystemCommand remount type
 */
enum NaviSystemRemountType {
    E_CMD_CODE_MOUNT_UNDEFINED = 0,
    E_CMD_CODE_MOUNT_DIR,
    E_CMD_CODE_REMOUNT_DIR_RO,
    E_CMD_CODE_REMOUNT_DIR_RW,
    E_CMD_CODE_REMOUNT_USB_RO,
    E_CMD_CODE_REMOUNT_USB_RW,
    E_CMD_CODE_UMOUNT_DIR,
    E_CMD_CODE_LXC_CHOWN,
    E_CMD_CODE_SYNC
};


/**
 * @brief RFAntennaType type
 */
enum RFAntennaType {
    AMFM_Antenna1 = 0,  //(VICS antenna in Japan project)
    AMFM_Antenna2,
    DAB_Antenna1,
    GPS_Antenna,
    DTV_Antenna1,
    DTV_Antenna2,
    DTV_Antenna3,
    DTV_Antenna4,
    Unknown_Antenna
};

/**
 * @brief RFAntennaStatus type
 *
 * Note:
 * The voltage limit value as below(+/- 0.2V approximately):
 * ANTENNA_STATE_DEACTIVATED_LOW_VG:	Low Voltage Level is below 6.40V
 * ANTENNA_STATE_DEACTIVATED_HIGH_VG:	High Voltage Level is above 19.00V
 *
 */
enum RFAntennaStatus {
    TUN_MSTR_RF_ANTENNA_STATE_UNKNOWN = 0,
    TUN_MSTR_RF_ANTENNA_STATE_OK,
    TUN_MSTR_RF_ANTENNA_STATE_OPEN,
    TUN_MSTR_RF_ANTENNA_STATE_SHORT,
    TUN_MSTR_RF_ANTENNA_STATE_DEACTIVATED,
    TUN_MSTR_RF_ANTENNA_STATE_DEACTIVATED_LOW_VG,
    TUN_MSTR_RF_ANTENNA_STATE_DEACTIVATED_HIGH_VG,
    TUN_MSTR_RF_ANTENNA_STATE_OPEN_AMPLIFIER,
    TUN_MSTR_RF_ANTENNA_STATE_PASSIVE,
    TUN_MSTR_RF_ANTENNA_STATE_NUMBER_OF_ITEM
};


/**
 * @brief The copyProfile struct
 */
struct copyInfo {
    copyInfo() : m_source(0), m_destination(0){}
    uchar m_source;
    uchar m_destination;
    bool operator==(const copyInfo &other) const {
        if (m_source == other.m_source &&
                m_destination == other.m_destination) {
            return true;
        }
        return false;
    }
    bool operator!=(const copyInfo &other) const {
        return !(*this == other);
    }

};
QDBusArgument &operator<<(QDBusArgument &argument, const copyInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, copyInfo &info);

/**
 * @brief The copyProfileResult struct
 */
struct ProfileTypes {
    ProfileTypes() : m_type(0){}
    int m_type;
};
typedef QList<ProfileTypes> types;



QDBusArgument &operator<<(QDBusArgument &argument, const ProfileTypes &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, ProfileTypes &info);

/**
 * @brief The NXDateTimeDetailInfo struct
 */
struct NXDateTimeDetailInfo {
    NXDateTimeDetailInfo() : m_year(0), m_month(0), m_day(0), m_hours(0), m_minutes(0),
        m_seconds(0), m_weekday(0) { }
    short m_year;
    uchar m_month;
    uchar m_day;
    uchar m_hours;
    uchar m_minutes;
    uchar m_seconds;
    uchar m_weekday;



    bool operator==(const NXDateTimeDetailInfo &other) const {
        if (m_year == other.m_year &&
                m_month == other.m_month &&
                m_day == other.m_day &&
                m_hours == other.m_hours &&
                m_minutes == other.m_minutes &&
                m_seconds == other.m_seconds &&
                m_weekday == other.m_weekday) {
            return true;
        }
        return false;
    }
    bool operator!=(const NXDateTimeDetailInfo &other) const {
        return !(*this == other);
    }

};

QDBusArgument &operator<<(QDBusArgument &argument, const NXDateTimeDetailInfo &info);
const QDBusArgument &operator>>(const QDBusArgument &argument, NXDateTimeDetailInfo &info);

/**
 * @brief The NXDevMediaInfo struct
 */
struct NXDevMediaInfo {
    NXDevMediaInfo() : m_mediaStatus(0) { }
    uchar m_mediaStatus;
    QString m_mediaPath;

    bool operator==(const NXDevMediaInfo &other) const {
        if (m_mediaStatus == other.m_mediaStatus &&
                m_mediaPath == other.m_mediaPath) {
            return true;
        }
        return false;
    }
    bool operator!=(const NXDevMediaInfo &other) const {
        return !(*this == other);
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const NXDevMediaInfo &devMediaInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, NXDevMediaInfo &devMediaInfo);

/**
 * @brief The NXMountingAngle struct
 */
struct NXMountingAngle {
    NXMountingAngle() { }
    // unknown
};

QDBusArgument &operator<<(QDBusArgument &argument, const NXMountingAngle &mountingAngle);
const QDBusArgument &operator>>(const QDBusArgument &argument, NXMountingAngle &mountingAngle);

/**
 * @brief The NXLockStateInfo struct
 */
struct NXLockStateInfo {
    NXLockStateInfo() : m_lockstate(0), m_ownerappid(0) { }
   	QString m_lockname;
    uchar m_lockstate;
    ushort m_ownerappid;
    QList<ushort> m_userlist;
    
    bool operator==(const NXLockStateInfo &other) const {
        if (m_lockname == other.m_lockname &&
                m_lockstate == other.m_lockstate &&
                m_ownerappid == other.m_ownerappid &&
                m_userlist == other.m_userlist ) {
            return true;
        }
        return false;
    }
    bool operator!=(const NXLockStateInfo &other) const {
        return !(*this == other);
    }    
};

QDBusArgument &operator<<(QDBusArgument &argument, const NXLockStateInfo& lockInfo);
const QDBusArgument &operator>>(const QDBusArgument &argument, NXLockStateInfo &lockInfo);

typedef QList<NXLockStateInfo> NXLockStateInfoList;





/**
 * @brief The NXConfigInfoItem struct
 */
struct NXConfigInfoItem {
    QString m_key;
    QString m_value;
};
typedef QList<NXConfigInfoItem> NXConfigInfoItemList;

QDBusArgument &operator<<(QDBusArgument &argument, const NXConfigInfoItem &item);
const QDBusArgument &operator>>(const QDBusArgument &argument, NXConfigInfoItem &item);

Q_DECLARE_METATYPE(copyInfo)
Q_DECLARE_METATYPE(NXDevMediaInfo)
Q_DECLARE_METATYPE(NXDateTimeDetailInfo)
Q_DECLARE_METATYPE(NXMountingAngle)
Q_DECLARE_METATYPE(ProfileTypes)
Q_DECLARE_METATYPE(types)
Q_DECLARE_METATYPE(NXLockStateInfo)
Q_DECLARE_METATYPE(NXLockStateInfoList)
Q_DECLARE_METATYPE(NXConfigInfoItem)
Q_DECLARE_METATYPE(NXConfigInfoItemList)

/**
 * @brief The NavigationExtType class
 */
class NavigationExtType
{
public:
    static void registerMetaTypes();
};

#endif // NAVIGATIONEXTTYPE_H
