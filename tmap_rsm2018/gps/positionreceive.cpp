#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <stdint.h>
#include <stdio.h>

#include "positionreceive.h"
#include "locationdatapool.h"
#include "gpsutil.h"
#include "datetimeutil.h"
#include <QTimeZone>
#include "nmeatype.h"
#include "satellitedatapool.h"
#include "VSM_Map.h"
#include "VSM_Navi.h"

#define DRPOSITION_VERBOSE_LOG
#ifdef DRPOSITION_VERBOSE_LOG
    #include <fstream>
    #include <iostream>
    #include <iomanip>
#endif
#include "logfile.h"

namespace SKT {

PositionReceive::PositionReceive()
{
    m_nTimeBias = 0;
    m_useGpsLog = false;
    m_pLogFilePosition = NULL;

    m_nmea.reset();
    m_drHeading = 0;
    m_drSpeed = 0;

    m_GNSStime = DateTimeUtil::currentUtcDateTime();

    memset(&m_rsmGnssConst, 0x00, sizeof(RSM_GNSS_CONST));
    memset(&m_jlrUtcConst, 0x00, sizeof(JLR_UTC_CONST));
    m_satelliteInfo.reserve(64);
    m_jlrDrStatus = 0;
    m_jlrGpsGood = false;
    m_bFilterGnss = true;
}

PositionReceive::~PositionReceive()
{
    if(m_pLogFilePosition)
    {
        delete m_pLogFilePosition;
        m_pLogFilePosition = NULL;
    }
}

void PositionReceive::SetGpsTimeBias(int bias)
{
    m_nTimeBias = bias * 60 * 60;
}

void PositionReceive::SetGpsLogUsage(bool enabled)
{
    m_useGpsLog = enabled;
}

void PositionReceive::SetLogPath(QString path)
{
    if(m_pLogFilePosition != NULL)
        delete m_pLogFilePosition;
    m_pLogFilePosition = new LogFile(eLogFileType_GPS, path);
}

bool PositionReceive::InpuDeadReckPosition(uint Timestamp, uint TimePosix, double Longitude, double Latitude, int PosErrCepLarge, int PosErrCepSmall, int PosErrCepHeading, int Heading, int HeadingError, double Turnrate, double TurnrateError, short Speed, short SpeedError, short Acceleration, short AccelerationError, int Height, int HeightError, ushort SensorFlags, ushort DrInfoValidity, LocationDataPtr &locationOut)
{
    m_GNSStime = DateTimeUtil::currentUtcDateTime();
//        if(m_nTimeBias != 0)
//            m_GNSStime = m_GNSStime.addSecs(m_nTimeBias);

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << Timestamp            << ","
            << m_GNSStime.toString("yyyy-M-d H:m:s.zzz") << ","
            << QString::number(Longitude, 'f', 8) << ","
            << QString::number(Latitude, 'f', 8) << ","
            << PosErrCepLarge       << ","
            << PosErrCepSmall       << ","
            << PosErrCepHeading     << ","
            << Heading              << ","
            << HeadingError         << ","
            << QString::number(Turnrate, 'f', 8) << ","
            << QString::number(TurnrateError, 'f', 8) << ","
            << Speed                << ","
            << SpeedError           << ","
            << Acceleration         << ","
            << AccelerationError    << ","
            << Height               << ","
            << HeightError          << ","
            << QString("0x%1").arg(SensorFlags, 4, 16, QLatin1Char('0')) << ","
            << QString("0x%1").arg(DrInfoValidity, 4, 16, QLatin1Char('0'));

        m_pLogFilePosition->WriteString("ProxyDeadReckPosition", strLog);
    } // use gps log
#endif

#ifndef PRODUCT_JLR2019
    locationOut->init();
    locationOut->setGpsMode(LocationData::DR_GPS);
    locationOut->setDataStatus(LocationData::ACTIVE);
    locationOut->setDateTime(m_GNSStime);

    //    location->setAccuracy(m_hdop);
    double cep = PosErrCepLarge;
	//[Yeon-2018.11.17] cep 필터 값 변경
	// 두개의 값중에 좋은 값을 우선 사용하게 변경.
	// CEP는 두값이 최조 50미터 이내일때민 신뢰한다고 가정한다.
	float cepFilter = 50.f;
	if ( PosErrCepSmall < cepFilter && PosErrCepLarge < cepFilter) {
		if(PosErrCepSmall < PosErrCepLarge)
	        cep = PosErrCepSmall;
	} else {
		cep = 88.;
	}

    locationOut->setAltitude((double)Height);
    locationOut->setAccuracy(cep);
    locationOut->setLatitude(Latitude);
    locationOut->setLongitude(Longitude);
    // 1 cm/s = 0.036 km/h
    locationOut->setSpeed(0.036f * Speed / NMEA_KNOT_TO_KM_PER_HOUR); // convert to knots
    locationOut->setDirection(Heading);
    locationOut->setTimeStamp(Timestamp);

//    qDebug() << "PositionReceive DR[" << locationOut->timeStamp() << "] " << locationOut->accuracy();

    m_drHeading = Heading;
    m_drSpeed = Speed;

    if(SensorFlags & 0x8000)
    {
        if(m_rsmGnssConst.isCheckGood)
        {
            VSM_Int32 worldX, worldY;
            if(m_rsmGnssConst.good_x == 0 && m_rsmGnssConst.good_y == 0)
            {
                VSMNavi_WGS84ToWorld(Longitude, Latitude, worldX, worldY);
                m_rsmGnssConst.good_x = worldX;
                m_rsmGnssConst.good_y = worldY;
                m_rsmGnssConst.good_dist = 0;
//                printf("########################    Set : %d, %d\n", m_rsmGnssConst.good_x, m_rsmGnssConst.good_y); fflush(stdout);

            }else
            {
                int dist = 0;
                VSMNavi_WGS84ToWorld(Longitude, Latitude, worldX, worldY);
                VSMMap_GetDistanceWorld(worldX, worldY, m_rsmGnssConst.good_x, m_rsmGnssConst.good_y, dist);
                m_rsmGnssConst.good_dist = dist;

//                printf("########################    dist : (%d, %d) (%d, %d) => %d\n", m_rsmGnssConst.good_x, m_rsmGnssConst.good_y, worldX, worldY, m_rsmGnssConst.good_dist); fflush(stdout);
            }
        }

        locationOut->setIsDrActive(true);
    }
#endif
    return true;
}

bool PositionReceive::InpuRawDrData(uint32_t time_tag, uint8_t position_stat, uint8_t altitude_stat, uint8_t heading_stat, uint8_t speed_stat, uint8_t dir_switch_stat, uint8_t delta_distance_stat, uint8_t gyro_cal_stat, uint8_t tacho_cal_stat, uint8_t direction, uint8_t time_src, uint8_t dr_solution_status, uint32_t tow, float latitude, float longitude, int16_t altitude, float heading, uint16_t speed, int16_t delta_distance, uint16_t position_accy, uint16_t altitude_accy, uint16_t heading_accy, uint16_t speed_accy, uint8_t gyro_sample_num, uint16_t tacho_counts, LocationDataPtr &locationOut)
{
    m_GNSStime = DateTimeUtil::currentUtcDateTime();
//        if(m_nTimeBias != 0)
//            m_GNSStime = m_GNSStime.addSecs(m_nTimeBias);

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << time_tag                  << ","
            << position_stat             << ","
            << altitude_stat             << ","
            << heading_stat              << ","
            << speed_stat                << ","
            << dir_switch_stat           << ","
            << delta_distance_stat       << ","
            << gyro_cal_stat             << ","
            << tacho_cal_stat            << ","
            << direction                 << ","
            << time_src                  << ","
            << dr_solution_status        << ","
            << tow                       << ","
            << QString::number(latitude, 'f', 8) << ","
            << QString::number(longitude, 'f', 8)<< ","
            << altitude                  << ","
            << heading                   << ","
            << speed                     << ","
            << delta_distance            << ","
            << position_accy             << ","
            << altitude_accy             << ","
            << heading_accy              << ","
            << speed_accy                << ","
            << gyro_sample_num           << ","
            << tacho_counts;

        m_pLogFilePosition->WriteString("NotifyRawDrData", strLog);
    } // use gps log
#endif

    if(latitude <= 0.0 || longitude <= 0.0)
        return false;

    locationOut->init();
    locationOut->setGpsMode(LocationData::DR_GPS);
    locationOut->setDataStatus(LocationData::ACTIVE);
    locationOut->setDateTime(m_GNSStime);
    locationOut->setAltitude((double)altitude);
    locationOut->setAccuracy(position_accy);
    locationOut->setLatitude(latitude);
    locationOut->setLongitude(longitude);
    // 1 cm/s = 0.036 km/h
    locationOut->setSpeed(0.036f * speed / NMEA_KNOT_TO_KM_PER_HOUR); // convert to knots
    locationOut->setDirection(heading);
    locationOut->setTimeStamp(time_tag);
    locationOut->setDrSolutionStation(dr_solution_status);


//    qDebug() << "PositionReceive DR[" << locationOut->timeStamp() << "] " << locationOut->accuracy();

//    m_drHeading = heading;
//    m_drSpeed = Speed;

    if(m_jlrDrStatus >= 1)
    {
        if(m_jlrDrStatus == 1)
            m_jlrGpsGood = false;
        else
            m_jlrGpsGood = true;

        m_jlrDrStatus = 0;
	    locationOut->setIsDrActive(true);
    }
    return true;
}

bool PositionReceive::InputGNSSPosition(uint Timestamp, ushort Year, uchar Month, uchar Day, uchar Hour, uchar Minute, uchar Second, uchar FixStatus, double Longitude, double Latitude, short Altitude, double PDOP, double HDOP, LocationDataPtr &locationOut)
{
    m_GNSStime = DateTimeUtil::currentUtcDateTime();
//        if(m_nTimeBias != 0)
//            m_GNSStime = m_GNSStime.addSecs(m_nTimeBias);

#ifdef DRPOSITION_VERBOSE_LOG
    int month = static_cast<int>(Month);
    int day = static_cast<int>(Day);
    int hour = static_cast<int>(Hour);
    int minute = static_cast<int>(Minute);
    int second = static_cast<int>(Second);
    int fixStatus = static_cast<int>(FixStatus);

    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << Timestamp       << ","
               << Year            << ","
               << month           << ","
               << day             << ","
               << hour            << ","
               << minute          << ","
               << second          << ","
               << fixStatus       << ","
               << QString::number(Longitude, 'f', 8) << ","
               << QString::number(Latitude, 'f', 8) << ","
               << Altitude        << ","
               << QString::number(PDOP, 'f', 4) << ","
               << QString::number(HDOP, 'f', 4);

        m_pLogFilePosition->WriteString("ProxyPositionGNSS", strLog);
    }
#endif

//    m_GNSSTimestamp = Timestamp;
//    if(HDOP < 99 && Year > 0)
    {
#if 0
    NXDateTimeDetailInfo dateTimeDetailInfo = m_navigationExt->naviPropertyUpdateDateTimeDetailInfo();
    QString mMilSec = "001";
    QString mDate;
    QTextStream out(&mDate);
    out << dateTimeDetailInfo.m_year
        << "-"
        << dateTimeDetailInfo.m_month
        << "-"
        << dateTimeDetailInfo.m_day
        << " "
        << dateTimeDetailInfo.m_hours
        << ":"
        << dateTimeDetailInfo.m_minutes
        << ":"
        << dateTimeDetailInfo.m_seconds
        << "."
        << mMilSec;
    system_time = QDateTime::fromString(mDate, "yyyy-M-d H:m:s.zzz");
#endif

    if(m_rsmGnssConst.fixType != 3 || (HDOP >= 99.0 || HDOP <= -99.0))
        HDOP = -99;

    if(HDOP == -99)
    {
        m_rsmGnssConst.isCheckGood = false;
        m_rsmGnssConst.good_dist = 0;
        m_rsmGnssConst.good_x = 0;
        m_rsmGnssConst.good_y = 0;
        m_rsmGnssConst.bad_count++;
    }else
    {
        if(m_rsmGnssConst.HDOP == -99 && m_rsmGnssConst.bad_count > 10)
        {
            m_rsmGnssConst.isCheckGood = true;
            if(m_rsmGnssConst.good_dist > 40)
            {
                m_rsmGnssConst.isCheckGood = false;
                m_rsmGnssConst.good_dist = 0;
                m_rsmGnssConst.good_x = 0;
                m_rsmGnssConst.good_y = 0;
            }else
            {
                HDOP = -99;
            }
        }else
        {
            m_rsmGnssConst.bad_count = 0;
        }
//        printf("########################    =>     %f  (%d)    #####################\n", HDOP, m_rsmGnssConst.good_dist); fflush(stdout);
    }


        QString mMilSec = "000";
        QString mDate;
        QTextStream out(&mDate);
        out << Year << "-" << Month << "-" << Day << " " << Hour << ":" << Minute << ":" << Second << "." << mMilSec;
        m_Gpstime = QDateTime::fromString(mDate, "yyyy-M-d H:m:s.zzz");
        m_Gpstime.setTimeSpec(Qt::UTC);

        m_rsmGnssConst.latitude = Latitude;
        m_rsmGnssConst.longitude = Longitude;
        m_rsmGnssConst.HDOP = HDOP;
        m_rsmGnssConst.altitude = Altitude;

        locationOut->init();
        locationOut->setGpsMode(LocationData::PHONE_GPS);
        locationOut->setDataStatus(LocationData::ACTIVE);
        locationOut->setDateTime(m_GNSStime);

        //qDebug() << "PositionReceive::InputGNSSPosition #2 utc: " << locationOut->dateTime() << " local: " << locationOut->localDateTime();

        if(HDOP == 99)
            HDOP = -99;
        locationOut->setAccuracy(HDOP);
        locationOut->setLatitude(Latitude);
        locationOut->setLongitude(Longitude);
        // 1 cm/s = 0.036 km/h
        locationOut->setSpeed(0.036f * m_drSpeed / NMEA_KNOT_TO_KM_PER_HOUR); // convert to knots
        locationOut->setDirection(m_drHeading);
        locationOut->setTimeStamp(Timestamp);
        return true;
    }
    return false;
}

bool PositionReceive::InputGnssFixData(uint32_t tow, uint8_t fix_src, uint8_t altitude_hold, uint8_t dgps_stat, uint8_t position_stat, uint8_t altitude_stat, uint8_t heading_stat, uint8_t speed_stat, uint8_t time_src, float latitude, float longitude, int16_t altitude, float heading, uint16_t speed, uint16_t vertical_speed, uint16_t position_accy, uint16_t altitude_accy, uint16_t heading_accy, uint16_t speed_accy, uint16_t vertical_speed_accy, LocationDataPtr &locationOut)
{
    m_GNSStime = DateTimeUtil::currentUtcDateTime();
//        if(m_nTimeBias != 0)
//            m_GNSStime = m_GNSStime.addSecs(m_nTimeBias);

#ifdef DRPOSITION_VERBOSE_LOG
   if (m_useGpsLog && m_pLogFilePosition)
   {
       QString strLog;
       QTextStream out(&strLog);
       out << tow              << ","
           << fix_src          << ","
           << altitude_hold    << ","
           << dgps_stat        << ","
           << position_stat    << ","
           << altitude_stat    << ","
           << heading_stat     << ","
           << speed_stat       << ","
           << time_src         << ","
           << QString::number(latitude, 'f', 8) << ","
           << QString::number(longitude, 'f', 8)<< ","
           << altitude         << ","
           << heading          << ","
           << speed            << ","
           << vertical_speed   << ","
           << position_accy    << ","
           << altitude_accy    << ","
           << heading_accy     << ","
           << speed_accy       << ","
           << vertical_speed_accy;

       m_pLogFilePosition->WriteString("NotifyGnssFixData", strLog);
   }
#endif

    locationOut->init();
    locationOut->setGpsMode(LocationData::PHONE_GPS);
    locationOut->setDataStatus(LocationData::ACTIVE);
    locationOut->setDateTime(m_GNSStime);

    //qDebug() << "PositionReceive::InputGNSSPosition #2 utc: " << locationOut->dateTime() << " local: " << locationOut->localDateTime();

    double hdop = 1.0;
    if(!m_bFilterGnss || position_accy < 20)
        hdop = position_accy/10.0;
    else
        hdop = -99;
    locationOut->setAccuracy(hdop);
    locationOut->setLatitude(latitude);
    locationOut->setLongitude(longitude);
    // 1 cm/s = 0.036 km/h
    locationOut->setSpeed(0.036f * speed / NMEA_KNOT_TO_KM_PER_HOUR); // convert to knots
    locationOut->setDirection(heading);
    locationOut->setTimeStamp(tow); // JLR에서는 DR time_tag 값을 사용해야 한다. (임시 tow)

    m_jlrDrStatus = 2;
    return true;
}

bool PositionReceive::InputUtcConstData(uint64_t time_stamp, uint16_t utc_year, uint8_t utc_month, uint8_t utc_day, uint8_t utc_hour, uint8_t utc_minute, uint8_t utc_second, uint16_t pdop, uint16_t hdop, uint16_t vdop, uint8_t gnss_stat, uint8_t time_src, uint8_t num_vis_sv)
{
#ifdef DRPOSITION_VERBOSE_LOG
   if (m_useGpsLog && m_pLogFilePosition)
   {
       QString strLog;
       QTextStream out(&strLog);
       out  <<  time_stamp        << ","
            <<  utc_year          << ","
            <<  utc_month         << ","
            <<  utc_day           << ","
            <<  utc_hour          << ","
            <<  utc_minute        << ","
            <<  utc_second        << ","
            <<  pdop              << ","
            <<  hdop              << ","
            <<  vdop              << ","
            <<  gnss_stat         << ","
            <<  time_src          << ","
            <<  num_vis_sv;

       m_pLogFilePosition->WriteString("NotifyUtcConstData", strLog);
   }
#endif

    m_jlrUtcConst.time_stamp = time_stamp;
    m_jlrUtcConst.utc_year = utc_year;
    m_jlrUtcConst.utc_month = utc_month;
    m_jlrUtcConst.utc_day = utc_day;
    m_jlrUtcConst.utc_hour = utc_hour;
    m_jlrUtcConst.utc_minute = utc_minute;
    m_jlrUtcConst.utc_second = utc_second;
    m_jlrUtcConst.pdop = pdop;
    m_jlrUtcConst.hdop = hdop;
    m_jlrUtcConst.vdop = vdop;
    m_jlrUtcConst.gnss_stat = gnss_stat;
    m_jlrUtcConst.time_src = time_src;
    m_jlrUtcConst.num_vis_sv = num_vis_sv;
/*
    QString mDate;
    QTextStream out(&mDate);
    out << utc_year
        << "-"
        << utc_month
        << "-"
        << utc_day
        << " "
        << utc_hour
        << ":"
        << utc_minute
        << ":"
        << utc_second
        << "."
        << 0;
*/
    m_jlrDrStatus = 1;
}

bool PositionReceive::InputDetailsGNSS(uint Timestamp, ushort SatellitesUsed, ushort Speed, ushort Heading, RsmSvList SpaceVehicles, SatelliteDataPtr &satelliteOut)
{

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        QString strLog;
        QTextStream out(&strLog);
        out << Timestamp       << ","
               << SatellitesUsed  << ","
               << Speed           << ","
               << Heading;

        for (int i = 0; i < SpaceVehicles.size(); i++)
        {
            const RSM_GNSS_CHANNEL &sv = SpaceVehicles.at(i);

            out << ","
                   << sv.m_svID       << ","
                   << sv.m_status  << ","
                   << sv.m_azimuth  << ","
                   << sv.m_elevation  << ","
                   << sv.m_SNR;
        }

        m_pLogFilePosition->WriteString("ProxyDetailsGNSS", strLog);
    }
#endif

    m_activeSatellites.clear();
    m_satelliteInfo.clear();

    int nActive = 0;
    NMEA::NmeaSatellite satData;
    for (int i = 0; i < SpaceVehicles.size(); i++)
    {
        const RSM_GNSS_CHANNEL &sv = SpaceVehicles.at(i);

        memset(&satData, 0x00, sizeof(NMEA::NmeaSatellite));
        if(sv.m_svID < 100)
            satData.satelliteType = NMEA::GP;
        else if(sv.m_svID < 200)
            satData.satelliteType = NMEA::GN;
        else if(sv.m_svID < 300)
            satData.satelliteType = NMEA::GA;
        else if(sv.m_svID < 500)
            satData.satelliteType = NMEA::UnknownTalker;
        else if(sv.m_svID < 600)
            satData.satelliteType = NMEA::QZ;
        else
            satData.satelliteType = NMEA::UnknownTalker;

        satData.id = sv.m_svID;
        satData.elevation = sv.m_elevation;
        satData.azimuth = sv.m_azimuth;
        satData.snr = sv.m_SNR;

        if((sv.m_status & 0x0010) || (sv.m_status & 0x0020))
            satData.tracked = true;

        m_satelliteInfo.append(satData);
        if(sv.m_status & 0x0020)
        {
            m_activeSatellites.insert(sv.m_svID);
        }
        if(sv.m_SNR > 30)
            nActive++;
    }

    satelliteOut->init();

    satelliteOut->date = m_Gpstime.date();
    satelliteOut->time = m_Gpstime.time();
    satelliteOut->latitude = m_rsmGnssConst.latitude;
    satelliteOut->longitude = m_rsmGnssConst.longitude;
    satelliteOut->speed = Speed;
    satelliteOut->altitude = m_rsmGnssConst.altitude;
    satelliteOut->HDOP = m_rsmGnssConst.HDOP;

    satelliteOut->satellitesInUse = m_activeSatellites.count();

    if(nActive >= 3)
        satelliteOut->fixType = 3;  // 3D fix
    else
        satelliteOut->fixType = 1;
    m_rsmGnssConst.fixType = satelliteOut->fixType;
    printf("---------------------------- %d ----------------------------------------\n", nActive);fflush(stdout);


    satelliteOut->activeSatellites = m_activeSatellites;
    satelliteOut->satellites = m_satelliteInfo;

    return true;
}

bool PositionReceive::InputGpsPosition(uint timestamp, const char *buffer, size_t bufferLen, LocationDataPtr &locationOut)
{
    m_GNSStime = DateTimeUtil::currentUtcDateTime();
//        if(m_nTimeBias != 0)
//            m_GNSStime = m_GNSStime.addSecs(m_nTimeBias);

#ifdef DRPOSITION_VERBOSE_LOG
    if (m_useGpsLog && m_pLogFilePosition)
    {
        if (bufferLen)
        {
            QString strLog;
            QTextStream out(&strLog);
            out << timestamp << "\n";
            out << buffer;

            m_pLogFilePosition->WriteString("sock_gnss", strLog);
        }
    }
#endif


    bool bRmc = false;
    if (!buffer || bufferLen <= 0) {
        return false;
    }

//    QByteArray aaa = buffer->to
    QByteArray data = QByteArray::fromRawData(buffer, bufferLen);
    QBuffer in(&data);
    if (in.open(QIODevice::ReadOnly)) {
        forever {
            QByteArray line = in.readLine();
            if (line.isEmpty()) {
                break;
            }
            if(NMEA::RMC == m_nmea.parseSentence(line.constData(), line.count()))
            {
                bRmc = true;
            }
        }
        in.close();
    }

    if(bRmc)
    {
        LocationDataPtr location = GpsUtil::getLocationFromNmea(m_nmea);
        location->setGpsMode(LocationData::PHONE_GPS);
        locationOut = location;
        locationOut->setTimeStamp(timestamp);
        locationOut->setDateTime(m_GNSStime);

//        qDebug() << "PositionReceive GPS[" << locationOut->timeStamp() << "] " << locationOut->accuracy();

        return true;
    }

    return false;
}

bool PositionReceive::InpuDeadReckPosition(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut, bool bNewType)
{
    uint Timestamp, TimePosix;
    double Longitude, Latitude;
    int PosErrCepLarge, PosErrCepSmall, PosErrCepHeading, Heading, HeadingError;
    double Turnrate, TurnrateError;
    short Speed, SpeedError, Acceleration, AccelerationError;
    int Height, HeightError;
    ushort SensorFlags, DrInfoValidity;

    QString strLine = QString::fromUtf8(buffer);
    QStringList list = strLine.split(",");
    if(list.count() < 20)
        return false;

    bool bStatus = false;

    int idx = 1;
    if(bNewType) idx++;
    Timestamp = list.at(idx++).toUInt();
    TimePosix = list.at(idx++).toUInt();
    Longitude = list.at(idx++).toDouble();
    Latitude = list.at(idx++).toDouble();
    PosErrCepLarge = list.at(idx++).toInt();
    PosErrCepSmall = list.at(idx++).toInt();
    PosErrCepHeading = list.at(idx++).toInt();
    Heading = list.at(idx++).toInt();
    HeadingError = list.at(idx++).toInt();
    Turnrate = list.at(idx++).toDouble();
    TurnrateError = list.at(idx++).toDouble();
    Speed = list.at(idx++).toShort();
    SpeedError = list.at(idx++).toShort();
    Acceleration = list.at(idx++).toShort();
    AccelerationError = list.at(idx++).toShort();
    Height = list.at(idx++).toInt();
    HeightError = list.at(idx++).toInt();
    QString strSensorFlags = list.at(idx++);
    SensorFlags = strSensorFlags.toUInt(&bStatus,16);
    QString strDrInfoValidity = list.at(idx++);
    DrInfoValidity = strDrInfoValidity.toUInt(&bStatus,16);

    //onProxyDeadReckPosition,5561,2018-5-16 1:7:32.483,126.98772200000001,37.5652531,47,21,72,70,44,0,0.057295756414532661,0,921,0,266,92,8,0x8002,0x118f
    //ProxyDeadReckPosition,00098.473,673167,2018-6-19 12:11:34.680,126.9893539,37.566271499999999,18174,350,1,129,28,21.497581377625465,0.14698564074933529,240,3,31,4,76,427,0x0033,0x118f
    return InpuDeadReckPosition(Timestamp, TimePosix, Longitude, Latitude, PosErrCepLarge, PosErrCepSmall, PosErrCepHeading, Heading, HeadingError, Turnrate, TurnrateError, Speed, SpeedError, Acceleration, AccelerationError, Height, HeightError, SensorFlags, DrInfoValidity, locationOut);
}

bool PositionReceive::InputGNSSPosition(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut, bool bNewType)
{
    uint Timestamp;
    ushort Year;
    uchar Month, Day, Hour, Minute, Second, FixStatus;
    double Longitude, Latitude;
    short Altitude;
    double PDOP, HDOP;

    QString strLine = QString::fromUtf8(buffer);
    QStringList list = strLine.split(",");
//    if(list.count() != 14)
//        return;

    int idx = 1;
    if(bNewType) idx++;
    Timestamp = list.at(idx++).toUInt();
    Year = list.at(idx++).toUShort();
    Month = static_cast<uchar>(list.at(idx++).toUInt());
    Day = static_cast<uchar>(list.at(idx++).toUInt());
    Hour = static_cast<uchar>(list.at(idx++).toUInt());
    Minute = static_cast<uchar>(list.at(idx++).toUInt());
    Second = static_cast<uchar>(list.at(idx++).toUInt());
    FixStatus = static_cast<uchar>(list.at(idx++).toUInt());
    Longitude = list.at(idx++).toDouble();
    Latitude = list.at(idx++).toDouble();
    Altitude = list.at(idx++).toShort();
    PDOP = list.at(idx++).toDouble();
    HDOP = list.at(idx++).toDouble();

    //onProxyPositionGNSS,15721,0,0,0,0,0,0,0,126.98902883333332,37.565367666666667,97,99,99
    //ProxyPositionGNSS,00099.191,674032,0,0,0,0,0,0,0,126.98916633333334,37.565403500000002,135,99,99
    return InputGNSSPosition(Timestamp, Year, Month, Day, Hour, Minute, Second, FixStatus, Longitude, Latitude, Altitude, PDOP, HDOP, locationOut);
}

bool PositionReceive::InputDetailsGNSS(const char *buffer, size_t bufferLen, SatelliteDataPtr &satelliteOut, bool bNewType)
{
    uint Timestamp;
    ushort SatellitesUsed;
    ushort Speed;
    ushort Heading;

    QString strLine = QString::fromUtf8(buffer);
    QStringList list = strLine.split(",");
    int nSatellite = (list.count()-6)/5;

    int idx = 2;
    Timestamp = list.at(idx++).toUInt();
    SatellitesUsed = list.at(idx++).toUShort();
    Speed = list.at(idx++).toUShort();
    Heading = list.at(idx++).toUShort();

    RsmSvList svList;
    RSM_GNSS_CHANNEL gnssChannel;
    for (int i = 0; i < nSatellite; i++)
    {
        gnssChannel.m_svID = list.at(idx++).toUShort();
        gnssChannel.m_status = list.at(idx++).toUShort();
        gnssChannel.m_azimuth = list.at(idx++).toUShort();
        gnssChannel.m_elevation = list.at(idx++).toUShort();
        gnssChannel.m_SNR = list.at(idx++).toUShort();
        svList.append(gnssChannel);
    }
    return InputDetailsGNSS(Timestamp, SatellitesUsed, Speed, Heading, svList, satelliteOut);
}

bool PositionReceive::InpuRawDrData(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut)
{
    uint32_t time_tag, tow;
    uint8_t position_stat, altitude_stat, heading_stat, speed_stat, dir_switch_stat, delta_distance_stat, gyro_cal_stat, tacho_cal_stat, direction, time_src, dr_solution_status, gyro_sample_num;
    float latitude, longitude, heading;
    int16_t altitude, delta_distance;
    uint16_t speed, position_accy, altitude_accy, heading_accy, speed_accy, tacho_counts;

    QString strLine = QString::fromUtf8(buffer);
    QStringList list = strLine.split(",");
    if(list.count() != 27)
        return false;

    int idx = 2;
    time_tag = list.at(idx++).toUInt();
    position_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    altitude_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    heading_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    speed_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    dir_switch_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    delta_distance_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    gyro_cal_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    tacho_cal_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    direction = static_cast<uint8_t>(list.at(idx++).toUInt());
    time_src = static_cast<uint8_t>(list.at(idx++).toUInt());
    dr_solution_status = static_cast<uint8_t>(list.at(idx++).toUInt());
    tow = list.at(idx++).toUInt();
    latitude = list.at(idx++).toFloat();
    longitude = list.at(idx++).toFloat();
    altitude = list.at(idx++).toShort();
    heading = list.at(idx++).toFloat();
    speed = list.at(idx++).toUShort();
    delta_distance = list.at(idx++).toShort();
    position_accy = list.at(idx++).toUShort();
    altitude_accy = list.at(idx++).toUShort();
    heading_accy = list.at(idx++).toUShort();
    speed_accy = list.at(idx++).toUShort();
    gyro_sample_num = list.at(idx++).toUShort();
    tacho_counts = list.at(idx++).toUShort();

    return InpuRawDrData(time_tag, position_stat, altitude_stat, heading_stat, speed_stat, dir_switch_stat, delta_distance_stat, gyro_cal_stat, tacho_cal_stat, direction, time_src, dr_solution_status, tow, latitude, longitude, altitude, heading, speed, delta_distance, position_accy, altitude_accy, heading_accy, speed_accy, gyro_sample_num, tacho_counts, locationOut);
}

bool PositionReceive::InputGnssFixData(const char *buffer, size_t bufferLen, LocationDataPtr &locationOut)
{
    uint32_t tow;
    uint8_t fix_src, altitude_hold, dgps_stat, position_stat, altitude_stat, heading_stat, speed_stat, time_src;
    float latitude, longitude, heading;
    int16_t altitude;
    uint16_t speed, vertical_speed, position_accy, altitude_accy, heading_accy, speed_accy, vertical_speed_accy;

    QString strLine = QString::fromUtf8(buffer);
    QStringList list = strLine.split(",");
    if(list.count() != 22)
        return false;

    int idx = 2;
    tow = list.at(idx++).toUInt();
    fix_src = static_cast<uint8_t>(list.at(idx++).toUInt());
    altitude_hold = static_cast<uint8_t>(list.at(idx++).toUInt());
    dgps_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    position_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    altitude_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    heading_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    speed_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    time_src = static_cast<uint8_t>(list.at(idx++).toUInt());
    latitude = list.at(idx++).toFloat();
    longitude = list.at(idx++).toFloat();
    altitude = list.at(idx++).toShort();
    heading = list.at(idx++).toFloat();
    speed = list.at(idx++).toUShort();
    vertical_speed = list.at(idx++).toUShort();
    position_accy = list.at(idx++).toUShort();
    altitude_accy = list.at(idx++).toUShort();
    heading_accy = list.at(idx++).toUShort();
    speed_accy = list.at(idx++).toUShort();
    vertical_speed_accy = list.at(idx++).toUShort();

    return InputGnssFixData(tow, fix_src, altitude_hold, dgps_stat, position_stat, altitude_stat, heading_stat, speed_stat, time_src, latitude, longitude, altitude, heading, speed, vertical_speed, position_accy, altitude_accy, heading_accy, speed_accy, vertical_speed_accy, locationOut);
}

bool PositionReceive::InputUtcConstData(const char *buffer, size_t bufferLen)
{
    uint64_t time_stamp;
    uint16_t utc_year, pdop, hdop, vdop;
    uint8_t utc_month, utc_day, utc_hour, utc_minute, utc_second, gnss_stat, time_src, num_vis_sv;

    QString strLine = QString::fromUtf8(buffer);
    QStringList list = strLine.split(",");
    if(list.count() != 15)
        return false;

    int idx = 2;
    time_stamp = list.at(idx++).toULongLong();
    utc_year = list.at(idx++).toUShort();
    utc_month = static_cast<uint8_t>(list.at(idx++).toUInt());
    utc_day = static_cast<uint8_t>(list.at(idx++).toUInt());
    utc_hour = static_cast<uint8_t>(list.at(idx++).toUInt());
    utc_minute = static_cast<uint8_t>(list.at(idx++).toUInt());
    utc_second = static_cast<uint8_t>(list.at(idx++).toUInt());
    pdop = list.at(idx++).toUShort();
    hdop = list.at(idx++).toUShort();
    vdop = list.at(idx++).toUShort();
    gnss_stat = static_cast<uint8_t>(list.at(idx++).toUInt());
    time_src = static_cast<uint8_t>(list.at(idx++).toUInt());
    num_vis_sv = static_cast<uint8_t>(list.at(idx++).toUInt());

    return InputUtcConstData(time_stamp, utc_year, utc_month, utc_day, utc_hour, utc_minute, utc_second, pdop, hdop, vdop, gnss_stat, time_src, num_vis_sv);
}

SatelliteDataPtr PositionReceive::GetSatellite()
{
    return GpsUtil::getSatelliteFromNmea(m_nmea);
}


} //SKT
