#ifndef EXTAGENTTYPE_H
#define EXTAGENTTYPE_H
#include <QDBusArgument>

/**
 * @brief The TimeZoneData struct
 */
struct TimeZoneData {
    TimeZoneData() : TimeZoneDifference(0), DSTDifference(0) { }
    int TimeZoneDifference;
    uchar DST_State;
    int DSTDifference;
};

QDBusArgument &operator<<(QDBusArgument &argument, const TimeZoneData &timezonedata);
const QDBusArgument &operator>>(const QDBusArgument &argument, TimeZoneData &timezonedata);

Q_DECLARE_METATYPE(TimeZoneData)


/**
 * @brief The MountAngles struct
 */
struct MountAngles {
    MountAngles() : AngRX(0), AngRY(0), AngRZ(0), AngSX(0), AngSY(0), AngSZ(0), AngTX(0), AngTY(0), AngTZ(0) { }
    uchar AngRX;
    uchar AngRY;
    uchar AngRZ;
    uchar AngSX;
    uchar AngSY;
    uchar AngSZ;
    uchar AngTX;
    uchar AngTY;
    uchar AngTZ;
};

QDBusArgument &operator<<(QDBusArgument &argument, const MountAngles &value);
const QDBusArgument &operator>>(const QDBusArgument &argument, MountAngles &value);

Q_DECLARE_METATYPE(MountAngles)


enum MountingSource
{
   SRC_MOUNTING_INVALID = 0,
   SRC_MOUNTING_KDS,
   SRC_MOUNTING_CALIB
};


/**
 * @brief The extAgentType class
 */

class extAgentType
{
public:
    static void registerMetaTypes();
};

#endif // EXTAGENTTYPE_H
