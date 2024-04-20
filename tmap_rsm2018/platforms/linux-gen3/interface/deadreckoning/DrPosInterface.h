/*************************************************************************
* FILE:         DrPosInterface.h
* PROJECT:      DR-Position Service
* DESCRIPTION:  Position Service
-------------------------------------------------------------------------
* COPYRIGHT:    (c) 2017-2018 Bosch SoftTec GmbH, Hildesheim
* HISTORY:
* Date       | Author       | Modification
* 29.06.2017 | hjo2hi       | added enums for sensor flags and DR validity
* 04.07.2017 | hro2hi       | added enums for GNSS fix status
**************************************************************************/

#ifndef _DRPOS_INTERFACE_H_
#define _DRPOS_INTERFACE_H_

namespace com {
namespace bosch {
namespace DrPositionService {

#define USE_SESSION_BUS

#define DBUS_PATH_DRPS           "/com/bosch/DrPositionService"
#define DBUS_SERVICE_DRPS        "com.bosch.DrPositionService"
#define DBUS_INTERFACE_DRPS_NAVI "com.bosch.DrPositionService.Navigation"

#define BOSCH_VERSION_MAJOR      1
#define BOSCH_VERSION_MINOR      5
#define BOSCH_VERSION_MICRO      0
#define BOSCH_VERSION_DATE       "2018-09-07"

typedef enum
{
   GNSS_NO_FIX = 0,  // no GNSS fix
   GNSS_2D_FIX = 1,  // 2D-fix
   GNSS_3D_FIX = 2   // 3D-fix
} enFixStatus;

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

typedef enum
{
   SRC_MOUNTING_INVALID = 0, // do not use the mounting information
   SRC_MOUNTING_KDS     = 1, // source of mounting information is KDS
   SRC_MOUNTING_CALIB   = 2  // mounting information is from internal calibration
} enMountingAngleStatus;

// value ranges of SvId
typedef enum
{
   SVID_MIN_GPS     = 0,
   SVID_MAX_GPS     = 99,
   SVID_MIN_GLONASS = 100,
   SVID_MAX_GLONASS = 199,
   SVID_MIN_GALILEO = 200,
   SVID_MAX_GALILEO = 299,
   SVID_MIN_COMPASS = 300,
   SVID_MAX_COMPASS = 399,
   SVID_MIN_SBAS    = 400,
   SVID_MAX_SBAS    = 499,
   SVID_MIN_QZSS    = 500,
   SVID_MAX_QZSS    = 599
} enSvIdRange;

} } }

#endif // _DRPOS_INTERFACE_H_
