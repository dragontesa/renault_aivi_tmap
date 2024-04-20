#include <QtGlobal>
#include <QBuffer>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QTimeZone>

#include <map>
#include <math.h>

#include "gpsutil.h"
#include "locationdatapool.h"
#include "satellitedatapool.h"
#include "datetimeutil.h"

#ifdef BUILD_TARGET
#ifdef PLATFORM_QNX
#else
#include "DrPosInterface.h"
#endif
#endif


#define MSPOS_USE_LOCALTIME

typedef enum
{
   SF_USED_ODOMETER  = 0x0001,  // Odometer data were used for positioning.
   SF_USED_GYROSCOPE = 0x0002,  // Gyroscope data were used for positioning.
   SF_USED_GNSS      = 0x0004,  // GNSS data were used for positioning.
   SF_USED_MMFB      = 0x0008,  // MMFB data were used for positioning.
   SF_CALI_ODOMETER  = 0x0010,  // Odometer is calibrated.
   SF_CALI_GYROSCOPE = 0x0020,  // Gyroscope is calibrated.
                                // ...
   SF_MMFB_TAG       = 0x8000   // position to be used for navigation map matching feedback
} enSensorFlags;

typedef enum
{
   VALID_TIMESTAMP      = 0x0001,  // Bit 0:  1 - Timestamp is valid.
   VALID_POSITION       = 0x0002,  // Bit 1:  1 - Latitude and Longitude are valid.
   VALID_HEADING        = 0x0004,  // Bit 2:  1 - Heading is valid
   VALID_SPEED          = 0x0008,  // Bit 3:  1 - Speed is valid.
   VALID_ALTITUDE       = 0x0010,  // Bit 4:  1 - Altitude is valid.
   VALID_TURNRATE_X     = 0x0020,  // Bit 5:  1 - TurnrateX is valid.
   VALID_TURNRATE_Y     = 0x0040,  // Bit 6:  1 - TurnrateY is valid.
   VALID_TURNRATE_Z     = 0x0080,  // Bit 7:  1 - TurnrateZ is valid.
   VALID_ACCELERATION_X = 0x0100,  // Bit 8:  1 - AccelerationX is valid.
   VALID_ACCELERATION_Y = 0x0200,  // Bit 9:  1 - AccelerationY is valid.
   VALID_ACCELERATION_Z = 0x0400,  // Bit 10: 1 - AccelerationZ is valid.
                                   // ...
   VALID_DEFAULT        = 0x019F,  // All available attributes are valid.
   VALID_NOTHING        = 0x0000   // Completely invalid => ignore position.
} enDrInfoValidity;                // VALID_*

namespace SKT {
namespace GpsUtil {

static const QString kMSPOS_TimeFormat = "yyyyMMddHHmmsszzz";

/**
 * @brief nmeaDegreesToDecimal
 * @param nmeaDegrees
 * @return
 */
double nmeaDegreesToDecimal(double nmeaDegrees)
{
    double deg;
    double min = 100.0 * modf(nmeaDegrees / 100.0, &deg);
    return deg + (min / 60.0);
}

/**
 * @brief decimalToNmeaDegrees
 * @param decimal
 * @return
 */
double decimalToNmeaDegrees(double decimal)
{
    double int_part;
    double fra_part = modf(decimal, &int_part);
    decimal = int_part * 100 + fra_part * 60;
    return decimal;
}

/**
 * @brief nmeaTimeToQDate
 * @param nmea
 * @return
 */
QDate nmeaTimeToQDate(const NMEA::NmeaTime &utc)
{
#ifdef PLATFORM_QNX
    return QDate(utc.year,
                 utc.month,
                 utc.day);
#else
    return QDate(utc.year + 1900,
                 utc.month + 1,
                 utc.day);
#endif
}

/**
 * @brief nmeaTimeToQTime
 * @param nmea
 * @return
 */
QTime nmeaTimeToQTime(const NMEA::NmeaTime &utc)
{
    return QTime(utc.hour,
                 utc.min,
                 utc.sec,
                 utc.msec);
}

/**
 * @brief nmeaTimeToQDateTime
 * @param nmea
 * @return
 */
QDateTime nmeaTimeToQDateTime(const NMEA::NmeaTime &utc)
{
    QDate date = nmeaTimeToQDate(utc);
    if (!date.isValid()) {
        return QDateTime();
    }

    QTime time = nmeaTimeToQTime(utc);
    if (!time.isValid()) {
        return QDateTime();
    }

    return QDateTime(date, time, Qt::UTC);
}

/**
 * @brief getMSPOSFromLocation
 *        1       2 3        4 5 6                 7   8   9 101112
 *        |       | |        | | |                 |   |   | | | |
 * $MSPOS,llll.ll,a,yyyyy.yy,a,a,yyyymmddhhmmssSSS,x.x,x.x,a,a,a*hh<CR><LF>
 *
 * Field Number:
 *  1) Latitude
 *  2) N or S (North or South)
 *  3) Longitude
 *  4) E or W (East or West)
 *  5) Time Type 1 or 2 ( 1:UTC[United Technologies Corporation], 2:KST[Korea Standard Time] )
 *  6) Time
 *  7) Speed over ground, knots
 *  8) Track angle, degrees
 *  9) Altitude
 * 10) Accuracy (meter)
 * 11) GPSMODE 1 or 2 (1 : Phone GPS, 2: DRGPS)
 * 12) Checksum
 * @param buff
 * @param bufferSize
 * @param data
 * @return
 */
int getMSPOSFromLocation(char* buff, int bufferSize, const LocationData &data)
{
#ifdef MSPOS_USE_LOCALTIME
    int timeType = 2;
    QDateTime dateTime = DateTimeUtil::toLocalDateTime(data.dateTime());
//    QDateTime dateTime = data.dateTime().toLocalTime();
#else
    int timeType = 1;
    QDateTime dateTime = data.dateTime();
#endif
    QString dateStr = dateTime.toString(kMSPOS_TimeFormat);

    double lat = data.latitude();
    char latDirection = 'N';
    if (lat < 0) {
        lat *= -1;
        latDirection = 'S';
    }

    double lon = data.longitude();
    char lonDirection = 'E';
    if (lon < 0) {
        lon *= -1;
        lonDirection = 'W';
    }

    double accuracy = data.accuracy();
    if (data.dataStatus() == LocationData::INACTIVE)
        accuracy *= -1;

    int n = snprintf(buff, bufferSize,
                     "$MSPOS,%09.6f,%c,%09.6f,%c,%d,%s,%03.1f,%06.4f,%03.1f,%03.1f,%d",
                     decimalToNmeaDegrees(lat),
                     latDirection,
                     decimalToNmeaDegrees(lon),
                     lonDirection,
                     timeType,
                     dateStr.toLatin1().constData(),
                     data.speed(),
                     data.direction(),
                     data.altitude(),
                     accuracy,
                     data.gpsMode());

    if (n >= 0 && n < bufferSize) {
        int checksum = 0;
        for (int i = 1; i < n; i++)
            checksum ^= buff[i];

        n += snprintf(buff + n, bufferSize - n, "*%02X\r\n", checksum);
    }

    return n;
}

/**
 * @brief getLocationFromNmea
 * @param nmea
 * @return
 */
LocationDataPtr getLocationFromNmea(const NmeaData &nmea)
{
    LocationDataPtr data = LocationDataPool::newDataPtr();

    data->init();

    if (nmea.hasRMC()) {
        // date
        data->setDateTime(nmeaTimeToQDateTime(nmea.m_rmc.utc));

        // status
        data->setDataStatus(nmea.m_rmc.status == 'A' ? LocationData::ACTIVE : LocationData::INACTIVE);

        // latitude
        double lat = nmeaDegreesToDecimal(nmea.m_rmc.lat);
        if (nmea.m_rmc.ns == 'S') {
            lat = -lat;
        }
        data->setLatitude(lat);

        // longitude
        double lon = nmeaDegreesToDecimal(nmea.m_rmc.lon);
        if (nmea.m_rmc.ew == 'W') {
            lon = -lon;
        }
        data->setLongitude(lon);

        // speed
        data->setSpeed(nmea.m_rmc.speed);

        // direction (angle)
        data->setDirection(nmea.m_rmc.direction);
    }

    if (nmea.hasGGA()) {
        // accuracy
        data->setAccuracy(nmea.m_gga.HDOP);

        // altitude
        data->setAltitude(nmea.m_gga.altitude);
    }

    // use system time if gps time is invalid
    if (!data->dateTime().isValid()) {
        data->setDateTime(DateTimeUtil::currentUtcDateTime());
    }

    return data;
}

#ifdef PLATFORM_QNX

/**
 * @brief getLocationFromNmea
 * @param qnxgpsinfo
 * @return
 */
LocationDataPtr getLocationFromNmea(const SKT::QnxGpsInfo* qnxgpsinfo)
{
    LocationDataPtr data = LocationDataPool::newDataPtr();

    data->init();

    //if (qnxgpsinfo.hasRMC())
    {
        // date
        data->setDateTime(nmeaTimeToQDateTime((SKT::NMEA::NmeaTime)qnxgpsinfo->utc));

        // status
        data->setDataStatus(qnxgpsinfo->status == 'A' ? LocationData::ACTIVE : LocationData::INACTIVE);

        // latitude
        double lat = nmeaDegreesToDecimal(qnxgpsinfo->lat);
        if (qnxgpsinfo->ns == 'S') {
            lat = -lat;
        }
        data->setLatitude(lat);

        // longitude
        double lon = nmeaDegreesToDecimal(qnxgpsinfo->lon);
        if (qnxgpsinfo->ew == 'W') {
            lon = -lon;
        }
        data->setLongitude(lon);

        // speed
        data->setSpeed(qnxgpsinfo->speed);

        // direction (angle)
        data->setDirection(qnxgpsinfo->direction);
    }

    if (qnxgpsinfo->gga)
    {
        // accuracy
        data->setAccuracy(qnxgpsinfo->HDOP);

        // altitude
        data->setAltitude(qnxgpsinfo->altitude);
    }

    // use system time if gps time is invalid
    if (!data->dateTime().isValid()) {
        data->setDateTime(QDateTime::currentDateTimeUtc());
    }

    return data;
}
#endif 

/**
 * @brief getSatelliteFromNmea
 * @param nmea
 * @return
 */
SatelliteDataPtr getSatelliteFromNmea(const NmeaData &nmea)
{
    SatelliteDataPtr data = SatelliteDataPool::newDataPtr();

    data->init();

    if (nmea.hasRMC()) {
#ifdef USE_KOREA_TIMEZONE
        QDateTime dateTime = nmeaTimeToQDateTime(nmea.m_rmc.utc).addSecs(9 * 60 * 60); // GMT+9
#else
        QDateTime dateTime = nmeaTimeToQDateTime(nmea.m_rmc.utc);
#endif

        data->date = dateTime.date();
        data->time = dateTime.time();

        data->latitude = nmeaDegreesToDecimal(nmea.m_rmc.lat);
        if (nmea.m_rmc.ns == 'S') {
            data->latitude = -data->latitude;
        }
        data->longitude = nmeaDegreesToDecimal(nmea.m_rmc.lon);
        if (nmea.m_rmc.ew == 'W') {
            data->longitude = -data->longitude;
        }
        data->speed = nmea.m_rmc.speed;
    }

    if (nmea.hasGGA()) {
        data->altitude = nmea.m_gga.altitude;
        data->HDOP = nmea.m_gga.HDOP;
        data->satellitesInUse = nmea.m_gga.satInUse;

        if (!nmea.hasRMC()) {
            data->time = nmeaTimeToQTime(nmea.m_gga.utc);
            data->latitude = nmeaDegreesToDecimal(nmea.m_gga.lat);
            if (nmea.m_gga.ns == 'S') {
                data->latitude = -data->latitude;
            }
            data->longitude = nmeaDegreesToDecimal(nmea.m_gga.lon);
            if (nmea.m_gga.ew == 'W') {
                data->longitude = -data->longitude;
            }
        }
    } else {
        data->satellitesInUse = nmea.m_activeSatellites.count();
    }

    if (nmea.hasGSA()) {
        data->fixType = nmea.m_gsa.fixType;
        if (!nmea.hasGGA()) {
            data->HDOP = nmea.m_gsa.HDOP;
        }
    } else if (nmea.hasRMC() && nmea.m_rmc.status == 'A') {
        data->fixType = 3;  // 3D fix
    } else {
        data->fixType = 1;
    }

    data->activeSatellites = nmea.m_activeSatellites;
    data->satellites = nmea.m_satelliteInfo;

    return data;
}

#ifdef PLATFORM_QNX
LocationDataPtr getLocationFromDr(const drdata &dr) {
//    using namespace com::bosch::DrPositionService;
    static const ushort DR_VALID_FLAGS = VALID_TIMESTAMP | VALID_POSITION | VALID_HEADING | VALID_SPEED | VALID_ALTITUDE;
    static const ushort DR_SENSOR_VALID_FLAGS = SF_USED_ODOMETER | SF_USED_GYROSCOPE | SF_USED_GNSS | SF_USED_MMFB | SF_CALI_ODOMETER | SF_CALI_GYROSCOPE;

    LocationDataPtr data = LocationDataPool::newDataPtr();

    data->init();
    data->setGpsMode(LocationData::DR_GPS);
//    qDebug() << "DR_VALID_FLAGS && dr.mSensorFlags: " << (DR_VALID_FLAGS && dr.mSensorFlags);
//    if (dr.mDrInfoValidity & DR_VALID_FLAGS) {
//        qDebug() << "dr.mDrInfoValidity & DR_VALID_FLAGS: " << (dr.mDrInfoValidity & DR_VALID_FLAGS);
//        if( DR_SENSOR_VALID_FLAGS & dr.mSensorFlags) {
//            qDebug() << "DR_SENSOR_VALID_FLAGS & dr.mSensorFlags: " << (DR_SENSOR_VALID_FLAGS & dr.mSensorFlags);
//            data->setDataStatus(LocationData::ACTIVE);
//        } else {
//            data->setDataStatus(LocationData::INACTIVE);
//        }
//    } else {
//        data->setDataStatus(LocationData::INACTIVE);
//    }
    if ((dr.mDrInfoValidity & DR_VALID_FLAGS) == DR_VALID_FLAGS && dr.mSensorFlags != 0) {
        data->setDataStatus(LocationData::ACTIVE);
        data->setAccuracy(dr.mHdop);
    } else {
        data->setDataStatus(LocationData::INACTIVE);
    }

    data->setLatitude(dr.mLatitude);
    data->setLongitude(dr.mLongitude);
    data->setSpeed(0.036f * dr.mSpeed / NMEA_KNOT_TO_KM_PER_HOUR);
    data->setDirection(dr.mHeading);
    data->setDateTime(dr.mTimePosix);
//    if (dr.mHdop == 99) {
//        data->setAccuracy(0);
//    } else {
//        data->setAccuracy(dr.mHdop);
//    }
    if (dr.mHdop == 99) {
        data->setAccuracy(0);
    }
    data->setTimeStamp(dr.mTimestamp);
    return data;
}
#endif

} //GpsUtil
} //SKT
