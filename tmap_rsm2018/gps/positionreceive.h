#ifndef POSITIONRECEIVE_H
#define POSITIONRECEIVE_H

#include <QObject>
#include <QString>
#include <QMutex>

#include "locationdataptr.h"
#include "satellitedataptr.h"
#include "nmeadata.h"

#ifdef BUILD_TARGET
#ifndef PLATFORM_QNX
#include "drpositiontype.h"
#endif
#endif

namespace  SKT {

// JLR2019
typedef struct tagJLR_UTC_CONST
{
    // Timestamp
    uint64_t time_stamp;

    // UTC year
    uint16_t utc_year;

    // UTC month
    uint8_t  utc_month;

    // UTC day
    uint8_t  utc_day;

    // UTC hour
    uint8_t  utc_hour;

    // UTC minute
    uint8_t  utc_minute;

    // UTC second
    uint8_t  utc_second;

    // PDOP (unit : 2^-8)
    uint16_t pdop;

    // HDOP (unit : 2^-8)
    uint16_t hdop;

    // VDOP (unit : 2^-8)
    uint16_t vdop;

    // 0 : Doing position fixes
    // 2 : Need time
    // 3 : PDOP too high
    // 5 : No usable SVs
    // 6 : One usable SV
    // 7 : Two usable SVs
    // 8 : Three usable SVs
    uint8_t  gnss_stat;

    // 0 : System clock
    // 1 : RTC
    // 2 : GPS (<10ms)
    // 3 : GPS (< 1ms)
    uint8_t  time_src;

    // Number of visible satellites
    uint8_t  num_vis_sv;

} JLR_UTC_CONST;

typedef struct tagRSM_GNSS_CONST
{
    double latitude;
    double longitude;

    double  HDOP;
    double  altitude;
    int     fixType;
    bool    isCheckGood;
    int     bad_count;
    int     good_dist;
    int     good_x;
    int     good_y;
} RSM_GNSS_CONST;


/**
 * @brief The GNSS_CHANNEL Struct
 */
typedef struct tagRSM_GNSS_CHANNEL {
    ushort m_svID;
    ushort m_status;
    ushort m_azimuth;
    ushort m_elevation;
    ushort m_SNR;
}RSM_GNSS_CHANNEL;

typedef QList<RSM_GNSS_CHANNEL> RsmSvList;


class LogFile;
class PositionReceive
{
public:
    PositionReceive();
    ~PositionReceive();

    void SetGpsTimeBias(int bias);
    void SetGpsLogUsage(bool enabled);
    void SetLogPath(QString path);


    // RSM2018
    bool InpuDeadReckPosition(uint Timestamp, uint TimePosix, double Longitude, double Latitude, int PosErrCepLarge, int PosErrCepSmall, int PosErrCepHeading, int Heading, int HeadingError, double Turnrate, double TurnrateError, short Speed, short SpeedError, short Acceleration, short AccelerationError, int Height, int HeightError, ushort SensorFlags, ushort DrInfoValidity, LocationDataPtr &locationOut);
    bool InputGNSSPosition(uint Timestamp, ushort Year, uchar Month, uchar Day, uchar Hour, uchar Minute, uchar Second, uchar FixStatus, double Longitude, double Latitude, short Altitude, double PDOP, double HDOP, LocationDataPtr &locationOut);
    bool InputDetailsGNSS(uint Timestamp, ushort SatellitesUsed, ushort Speed, ushort Heading, RsmSvList SpaceVehicles, SatelliteDataPtr &satelliteOut);

    bool InputGpsPosition(uint Timestamp, const char *buffer, size_t bufferLen, LocationDataPtr &locationOut);

    bool InpuDeadReckPosition(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut, bool bNewType);
    bool InputGNSSPosition(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut, bool bNewType);
    bool InputDetailsGNSS(const char *buffer, size_t bufferLen, SatelliteDataPtr &satelliteOut, bool bNewType);


    // JLR2019
    bool InpuRawDrData(
            // System time of data collection
            uint32_t time_tag,

            // 0 : Invalid
            // 1 : Valid
            uint8_t position_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t altitude_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t heading_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t speed_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t dir_switch_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t delta_distance_stat,

            // 0 : Not calibrated
            // 1 : Calibrated
            uint8_t gyro_cal_stat,

            // 0 : Not calibrated
            // 1 : Calibrated
            uint8_t tacho_cal_stat,

            // 0 : Forward
            // 1 : Backward
            uint8_t direction,

            // 0 : System clock
            // 1 : RTC
            // 2 : GPS (<10ms)
            // 3 : GPS (< 1ms)
            uint8_t time_src,

            // 0 : DR is not fully calibrated or DR position is invalid
            // 1 : DR is fully calibrated and DR position is valid
            uint8_t dr_solution_status,

            // GPS Time of week (unit : 1ms)
            uint32_t tow,

            // Latitude (unit : WGS84 in degrees)
            float latitude,

            // Longitude (unit : WGS84 in degrees)
            float longitude,

            // Altitude (unit : MSL in meters)
            int16_t altitude,

            // Heading (unit : degrees)
            float heading,

            // Speed (unit : cm/s)
            uint16_t speed,

            // Delta distance (unit : cm)
            int16_t delta_distance,

            // Position accuracy (unit : meters)
            uint16_t position_accy,

            // Altitude accuracy (unit : meters)
            uint16_t altitude_accy,

            // Heading accuracy
            uint16_t heading_accy,

            // Speed accuracy (unit : cm/s)
            uint16_t speed_accy,

            // Number of gyro samples
            uint8_t gyro_sample_num,

            // Tacho counts
            uint16_t tacho_counts,

            LocationDataPtr &locationOut
        );

    bool InputGnssFixData(
            // GPS Time of week (unit : 1ms)
            uint32_t tow,

            // 0 : No fix
            // 17 : Fix valid
            uint8_t fix_src,

            // 0 : Full position (3D fix)
            // 1 : Altitude-hold (2D fix)
            uint8_t altitude_hold,

            // 0 : No DGPS correction
            // 1 : DGPS-corrected
            uint8_t dgps_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t position_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t altitude_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t heading_stat,

            // 0 : Invalid
            // 1 : Valid
            uint8_t speed_stat,

            // 0 : System clock
            // 1 : RTC
            // 2 : GPS (<10ms)
            // 3 : GPS (< 1ms)
            uint8_t time_src,

            // Latitude (unit : WGS84 in degrees)
            float latitude,

            // Longitude (unit : WGS84 in degrees)
            float longitude,

            // Altitude (unit : MSL in meters)
            int16_t altitude,

            // Heading (unit : degrees)
            float heading,

            // Speed (unit : cm/s)
            uint16_t speed,

            // Vertical speed (unit : cm/s)
            uint16_t vertical_speed,

            // Position accuracy (unit : meters)
            uint16_t position_accy,

            // Altitude accuracy (unit : meters)
            uint16_t altitude_accy,

            // Heading accuracy
            uint16_t heading_accy,

            // Speed accuracy (unit : cm/s)
            uint16_t speed_accy,

            // Vertical speed accuracy (unit : cm/s)
            uint16_t vertical_speed_accy,

            LocationDataPtr &locationOut
            );

    bool InputUtcConstData(
            // Timestamp
            uint64_t time_stamp,

            // UTC year
            uint16_t utc_year,

            // UTC month
            uint8_t utc_month,

            // UTC day
            uint8_t utc_day,

            // UTC hour
            uint8_t utc_hour,

            // UTC minute
            uint8_t utc_minute,

            // UTC second
            uint8_t utc_second,

            // PDOP (unit : 2^-8)
            uint16_t pdop,

            // HDOP (unit : 2^-8)
            uint16_t hdop,

            // VDOP (unit : 2^-8)
            uint16_t vdop,

            // 0 : Doing position fixes
            // 2 : Need time
            // 3 : PDOP too high
            // 5 : No usable SVs
            // 6 : One usable SV
            // 7 : Two usable SVs
            // 8 : Three usable SVs
            uint8_t gnss_stat,

            // 0 : System clock
            // 1 : RTC
            // 2 : GPS (<10ms)
            // 3 : GPS (< 1ms)
            uint8_t time_src,

            // Number of visible satellites
            uint8_t num_vis_sv
            );

    bool InpuRawDrData(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut);
    bool InputGnssFixData(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut);
    bool InputUtcConstData(const char *buffer, size_t bufferLen);

    bool IsJrlGpsGood() {return m_jlrGpsGood;}
    void SetJrlGpsGood() {m_jlrGpsGood = true;}

    void SetFilterGnss(bool bFilter) {m_bFilterGnss = bFilter;};

    SatelliteDataPtr GetSatellite();

private:
    int m_nTimeBias;
    bool m_useGpsLog; // gps log usage , default is true, by lckxx 2018.1.3;
    LogFile *m_pLogFilePosition;

    QDateTime m_GNSStime;
    QDateTime m_Gpstime;
    RSM_GNSS_CONST m_rsmGnssConst;

    QVector<NMEA::NmeaSatellite> m_satelliteInfo;
    QSet<int> m_activeSatellites;

    NmeaData m_nmea;
    QMutex m_mutex;

    int m_drHeading;
    short m_drSpeed;

    JLR_UTC_CONST m_jlrUtcConst;
    int m_jlrDrStatus; // 0:초기상태, 1:UTC수신, 2:GNSS수신 - DR 수신시 상태가 1이면 음영지역, 2이면 개활지역, 0이면 버린다.
    bool m_jlrGpsGood;
    bool m_bFilterGnss;
};

}

#endif // POSITIONRECEIVE_H
