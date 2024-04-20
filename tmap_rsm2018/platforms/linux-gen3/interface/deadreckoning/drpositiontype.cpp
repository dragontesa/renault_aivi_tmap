#include "drpositiontype.h"

#include <QMetaType>
#include <QDBusMetaType>

void DrPositionType::registerMetaTypes()
{
    qDBusRegisterMetaType<VersionInfo>();
    qDBusRegisterMetaType<MMCandidate>();
    qDBusRegisterMetaType<MMCandidateList>();
    qDBusRegisterMetaType<GNSS_CHANNEL>();
    qDBusRegisterMetaType<SvList>();
}


QDBusArgument &operator<<(QDBusArgument &argument, const VersionInfo &version)
{
    argument.beginStructure();
    argument << version.m_major;
    argument << version.m_minor;
    argument << version.m_micro;
    argument << version.m_date;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, VersionInfo &version)
{
    argument.beginStructure();
    argument >> version.m_major;
    argument >> version.m_minor;
    argument >> version.m_micro;
    argument >> version.m_date;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const MMCandidate &candidate)
{
    argument.beginStructure();
    argument << candidate.m_deltaLongitude;
    argument << candidate.m_deltaLatitude;
    argument << candidate.m_probability;
    argument << candidate.m_segmentFlag;
    argument << candidate.m_segmentHeading;
    argument << candidate.m_streetWidth;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MMCandidate &candidate)
{
    argument.beginStructure();
    argument >> candidate.m_deltaLongitude;
    argument >> candidate.m_deltaLatitude;
    argument >> candidate.m_probability;
    argument >> candidate.m_segmentFlag;
    argument >> candidate.m_segmentHeading;
    argument >> candidate.m_streetWidth;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const GNSS_CHANNEL &gnss)
{
    argument.beginStructure();
    argument << gnss.m_svID;
    argument << gnss.m_status;
    argument << gnss.m_azimuth;
    argument << gnss.m_elevation;
    argument << gnss.m_SNR;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, GNSS_CHANNEL &gnss)
{
    argument.beginStructure();
    argument >> gnss.m_svID;
    argument >> gnss.m_status;
    argument >> gnss.m_azimuth;
    argument >> gnss.m_elevation;
    argument >> gnss.m_SNR;
    argument.endStructure();

    return argument;
}
