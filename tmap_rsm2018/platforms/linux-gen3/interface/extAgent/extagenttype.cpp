#include "extagenttype.h"
#include <QMetaType>
#include <QDBusMetaType>


void extAgentType::registerMetaTypes() {
    qDBusRegisterMetaType<TimeZoneData>();
    qDBusRegisterMetaType<MountAngles>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const TimeZoneData &timezonedata)
{
    argument.beginStructure();
    argument << timezonedata.TimeZoneDifference;
    argument << timezonedata.DST_State;
    argument << timezonedata.DSTDifference;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, TimeZoneData &timezonedata)
{
    argument.beginStructure();
    argument >> timezonedata.TimeZoneDifference;
    argument >> timezonedata.DST_State;
    argument >> timezonedata.DSTDifference;
    argument.endStructure();

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const MountAngles &value)
{
    argument.beginStructure();
    argument << value.AngRX;
    argument << value.AngRY;
    argument << value.AngRZ;
    argument << value.AngSX;
    argument << value.AngSY;
    argument << value.AngSZ;
    argument << value.AngTX;
    argument << value.AngTY;
    argument << value.AngTZ;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MountAngles &value)
{
    argument.beginStructure();
    argument >> value.AngRX;
    argument >> value.AngRY;
    argument >> value.AngRZ;
    argument >> value.AngSX;
    argument >> value.AngSY;
    argument >> value.AngSZ;
    argument >> value.AngTX;
    argument >> value.AngTY;
    argument >> value.AngTZ;
    argument.endStructure();

    return argument;
}
