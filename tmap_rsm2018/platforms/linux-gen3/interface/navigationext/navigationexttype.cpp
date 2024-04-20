#include <QMetaType>
#include <QDBusMetaType>
#include "navigationexttype.h"

void NavigationExtType::registerMetaTypes()
{
    qDBusRegisterMetaType<NXDevMediaInfo>();
    qDBusRegisterMetaType<NXDateTimeDetailInfo>();
    qDBusRegisterMetaType<NXMountingAngle>();
    qDBusRegisterMetaType<ProfileTypes>();
    qDBusRegisterMetaType<copyInfo>();
	qDBusRegisterMetaType<NXLockStateInfo>();
    qDBusRegisterMetaType<NXLockStateInfoList>();
    qDBusRegisterMetaType<NXConfigInfoItem>();
    qDBusRegisterMetaType<NXConfigInfoItemList>();
}



QDBusArgument &operator<<(QDBusArgument &argument, const copyInfo &info)
{
    argument.beginStructure();
    argument << info.m_source;
    argument << info.m_destination;
    argument.endStructure();

    return argument;
}
const QDBusArgument &operator>>(const QDBusArgument &argument, copyInfo &info)
{
    argument.beginStructure();
    argument >> info.m_source;
    argument >> info.m_destination;
    argument.endStructure();

    return argument;

}

QDBusArgument &operator<<(QDBusArgument &argument, const NXDateTimeDetailInfo &info)
{
    argument.beginStructure();
    argument << info.m_year;
    argument << info.m_month;
    argument << info.m_day;
    argument << info.m_hours;
    argument << info.m_minutes;
    argument << info.m_seconds;
    argument << info.m_weekday;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NXDateTimeDetailInfo &info)
{
    argument.beginStructure();
    argument >> info.m_year;
    argument >> info.m_month;
    argument >> info.m_day;
    argument >> info.m_hours;
    argument >> info.m_minutes;
    argument >> info.m_seconds;
    argument >> info.m_weekday;
    argument.endStructure();

    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const NXDevMediaInfo &devMediaInfo)
{
    argument.beginStructure();
    argument << devMediaInfo.m_mediaStatus;
    argument << devMediaInfo.m_mediaPath;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NXDevMediaInfo &devMediaInfo)
{
    argument.beginStructure();
    argument >> devMediaInfo.m_mediaStatus;
    argument >> devMediaInfo.m_mediaPath;
    argument.endStructure();

    return argument;
}
QDBusArgument &operator<<(QDBusArgument &argument, const ProfileTypes &info)
{
    argument.beginStructure();
    argument << info.m_type;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ProfileTypes &info)
{
    argument.beginStructure();
    argument >> info.m_type;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NXMountingAngle &mountingAngle)
{
    argument.beginStructure();
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NXMountingAngle &mountingAngle)
{
    argument.beginStructure();
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const NXConfigInfoItem &item)
{
    argument.beginStructure();
    argument << item.m_key;
    argument << item.m_value;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NXConfigInfoItem &item)
{
    argument.beginStructure();
    argument >> item.m_key;
    argument >> item.m_value;
    argument.endStructure();

    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const NXLockStateInfo &lockInfo)
{
    argument.beginStructure();
    argument << lockInfo.m_lockname;
    argument << lockInfo.m_lockstate;
    argument << lockInfo.m_ownerappid;
    argument << lockInfo.m_userlist;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NXLockStateInfo &lockInfo)
{
    argument.beginStructure();
    argument >> lockInfo.m_lockname;
    argument >> lockInfo.m_lockstate;
    argument >> lockInfo.m_ownerappid;
    argument >> lockInfo.m_userlist;
    argument.endStructure();

    return argument;
}
