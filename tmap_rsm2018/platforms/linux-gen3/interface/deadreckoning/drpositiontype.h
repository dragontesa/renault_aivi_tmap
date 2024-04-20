#ifndef DRPOSITIONTYPE_H
#define DRPOSITIONTYPE_H

#include <QDBusArgument>

/**
 * @brief The DrCandidate struct
 */
struct MMCandidate {
    MMCandidate() : m_deltaLongitude(0), m_deltaLatitude(0),
        m_probability(0), m_segmentFlag(0), m_segmentHeading(0),
        m_streetWidth(0) { }
    double m_deltaLongitude;
    double m_deltaLatitude;
    uchar m_probability;
    uchar m_segmentFlag;
    short m_segmentHeading;
    uchar m_streetWidth;


    bool operator==(const MMCandidate &other) const {
        if (m_deltaLongitude        == other.m_deltaLongitude &&
                m_deltaLatitude     == other.m_deltaLatitude &&
                m_probability       == other.m_probability &&
                m_segmentFlag       == other.m_segmentFlag &&
                m_segmentHeading    == other.m_segmentHeading &&
                m_streetWidth       == other.m_streetWidth) {
            return true;
        }
        return false;
    }
    bool operator!=(const MMCandidate &other) const {
        return !(*this == other);
    }
    void setInit(MMCandidate &other) const {
        other.m_deltaLongitude = 0;
        other.m_deltaLatitude  = 0;
        other.m_probability    = 0;
        other.m_segmentFlag    = 0;
        other.m_segmentHeading = 0;
        other.m_streetWidth    = 0;
    }
};

QDBusArgument &operator<<(QDBusArgument &argument, const MMCandidate &c);
const QDBusArgument &operator>>(const QDBusArgument &argument, MMCandidate &c);
Q_DECLARE_METATYPE(MMCandidate)

typedef QList<MMCandidate> MMCandidateList;
Q_DECLARE_METATYPE(MMCandidateList)

/**
 * @brief The VersionInfo struct
 */
struct VersionInfo {
    VersionInfo() : m_major(0), m_minor(0), m_micro(0) { }
    ushort m_major;
    ushort m_minor;
    ushort m_micro;
    QString m_date;
};

QDBusArgument &operator<<(QDBusArgument &argument, const VersionInfo &version);
const QDBusArgument &operator>>(const QDBusArgument &argument, VersionInfo &version);
Q_DECLARE_METATYPE(VersionInfo)


/**
 * @brief The GNSS_CHANNEL Struct
 */
struct GNSS_CHANNEL {
    GNSS_CHANNEL() : m_svID(0), m_status(0), m_azimuth(0), m_elevation(0), m_SNR(0) { }
    ushort m_svID;
    ushort m_status;
    ushort m_azimuth;
    ushort m_elevation;
    ushort m_SNR;
};

QDBusArgument &operator<<(QDBusArgument &argument, const GNSS_CHANNEL &gnss);
const QDBusArgument &operator>>(const QDBusArgument &argument, GNSS_CHANNEL &gnss);
Q_DECLARE_METATYPE(GNSS_CHANNEL)

typedef QList<GNSS_CHANNEL> SvList;
Q_DECLARE_METATYPE(SvList)

/**
 * @brief The DrPositionType class
 */
class DrPositionType
{
public:
    static void registerMetaTypes();
};

#endif // DRPOSITIONTYPE_H
