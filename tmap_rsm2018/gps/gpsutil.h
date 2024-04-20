#ifndef GPSUTIL_H
#define GPSUTIL_H

#include <QDateTime>
#include "nmeatype.h"
#include "nmeadata.h"
#include "locationdataptr.h"
#include "satellitedataptr.h"
#include "drdata.h"
#ifdef PLATFORM_QNX
#include "qnx_interface_def.h"
#endif

namespace SKT {

namespace GpsUtil {

double nmeaDegreesToDecimal(double nmeaDegrees);
double decimalToNmeaDegrees(double decimal);
QDate nmeaTimeToQDate(const NMEA::NmeaTime &utc);
QTime nmeaTimeToQTime(const NMEA::NmeaTime &utc);
QDateTime nmeaTimeToQDateTime(const NMEA::NmeaTime &utc);


int getMSPOSFromLocation(char* buff, int bufferSize, const LocationData &data);
LocationDataPtr getLocationFromNmea(const NmeaData &nmea);
#ifdef PLATFORM_QNX
LocationDataPtr getLocationFromNmea(const SKT::QnxGpsInfo* qnxgpsinfo);
LocationDataPtr getLocationFromDr(const drdata &dr);
#endif
SatelliteDataPtr getSatelliteFromNmea(const NmeaData &nmea);




}

}
#endif // GPSUTIL_H
