#ifndef NMEATYPEDEF_H
#define NMEATYPEDEF_H

#define NMEA_KNOT_TO_KM_PER_HOUR    1.852f  // 1 knot = 1.852 km/h
#define NMEA_GSV_MESSAGE_COUNT      5
#define NMEA_GSV_SAT_IN_MESSAGE     4

namespace SKT {

namespace NMEA {

enum Talker {
    UnknownTalker = -1,
    GP,     // Global Positioning System receiver
    GA,     // Galileo Positioning System
    GL,     // GLONASS, according to IEIC 61162-1
    GN,     // Mixed GPS and GLONASS data, according to IEIC 61162-1
    BD,     // BeiDou (China)
    QZ,     // QZSS regional GPS augmentation system (Japan)
    TalkerMax
};

enum NmeaType {
    UNKNOWN = 0x0000,
    ALL     = 0xFFFF,
    GGA     = 0x0001,
    GSA     = 0x0002,
    GSV     = 0x0004,
    RMC     = 0x0008
};

struct NmeaTime {
    int     year;   // since 1900
    int     month;  // [0,11]
    int     day;    // [1,31]
    int     hour;   // [0,23]
    int     min;    // [0,59]
    int     sec;    // [0,59]
    int     msec;   // [0,999]
};

struct NmeaSatellite {
    Talker  satelliteType;
    int     id;
    int     elevation;
    int     azimuth;
    int     snr;        // 00-99 dB
    bool    tracked;
};

struct NmeaGGA {
    NmeaTime utc;
    double  lat;
    char    ns;
    double  lon;
    char    ew;
    int     sig;
    int     satInUse;
    double  HDOP;
    double  altitude;
    char    altitudeUnit;
    double  diff;
    char    diffUnit;
    double  dgpsAge;
    int     dgpsSid;
};

struct NmeaGSA {
    char    fixMode;
    int     fixType;
    int     satCount;
    int     satPrn[12];
    double  PDOP;
    double  HDOP;
    double  VDOP;
};

struct NmeaGSV {
    int     messageCount;
    int     messageIndex;
    int     totalSatCount;
    int     satCount;
    NmeaSatellite satData[NMEA_GSV_SAT_IN_MESSAGE];
};

struct NmeaRMC {
    NmeaTime utc;
    char    status;
    double  lat;
    char    ns;
    double  lon;
    char    ew;
    double  speed;
    double  direction;
    double  declination;
    char    declination_ew;
    char    mode;
};

}

}

#endif // NMEATYPEDEF_H
