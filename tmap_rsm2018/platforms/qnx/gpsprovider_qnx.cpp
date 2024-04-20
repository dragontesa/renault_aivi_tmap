#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <stdint.h>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <sstream>


#include "gpsprovider_qnx.h"
#include "gpsservice.h"
#include "qnx_interface_def.h"
#include "gpsutil.h"
#include "locationdatapool.h"
#include "locationservice/LocationMsgDef.h"
#include <QDebug>
#include "VSM_Map.h"


using namespace std;
namespace SKT {

PositionProvider *g_pGpsProviderQnx = NULL;
void LocationCallback(int nMsg, void* lpParam)
{
    g_pGpsProviderQnx->UpdateLocationData(nMsg, lpParam);
}


GpsProviderQnx::GpsProviderQnx()
    : m_start(false)
    , m_useGpsLog(false)
    , m_pPositionReceive(NULL)
    , m_RGData(0)
    , m_mmfbTime(0)
    , m_mmfbX(0)
    , m_mmfbY(0)
{
    m_platformController = NULL;

    m_locationcb = NULL;
    m_locationcb = LocationCallback;

	m_pPositionReceive = new PositionReceive;
	m_pLogFileMMFB = NULL;
}

GpsProviderQnx::~GpsProviderQnx()
{

    if(m_pPositionReceive)
    {
        delete m_pPositionReceive;
        m_pPositionReceive = NULL;
    }
    if(m_pLogFileMMFB)
    {
        delete m_pLogFileMMFB;
        m_pLogFileMMFB = NULL;
    }
}

void GpsProviderQnx::startUpdates()
{
    if (m_start) {
        return;
    }
    m_start = true;
}

void GpsProviderQnx::stopUpdates()
{
    if (!m_start) {
        return;
    }

    m_start = false;
}

void GpsProviderQnx::UpdateLocationData(int nMsg, void* lpParam)
{
    //qWarning() << "void GpsProviderQnx::UpdateLocationData() called.";
    if(false == m_start)
        return;

    //qWarning() << "----------------------------------- UpdateLocationData called. (" << nMsg << ")";

    if(0 == nMsg)
    {
        // onNotifyUtcConstData
        onNotifyUtcConstData((LOCSRV_UTC_CONST*)lpParam);
    }
    else if(1 == nMsg)
    {
        // onNotifyGnssFixData
        onNotifyGnssFixData((LOCSRV_GNSS_FIX*)lpParam);
    }
    else if(2 == nMsg)
    {
        // onNotifyAllSvsInfoData
        onNotifyAllSvsInfoData((LOCSRV_ALL_SVS_INFO*)lpParam);
    }
    else if(3 == nMsg)
    {
        // onNotifyRawDrData
        onNotifyRawDrData((LOCSRV_RAW_DR*)lpParam);
    }
    else if(4 == nMsg)
    {
        // onResponseFwInfoData
    }
    else if(5 == nMsg)
    {

    }
}

void GpsProviderQnx::sendExtraCommand(const QString &cmd)
{
    Q_UNUSED(cmd)
}

void GpsProviderQnx::changeGpsLogUsage(bool enabled)
{
    m_useGpsLog = enabled;
    m_pPositionReceive->SetGpsLogUsage(enabled);
}
void GpsProviderQnx::applyGpsTimeBias(int bias) {
    qDebug() << "DrPositionProviderGen3::applyGpsTimeBias bias: " << bias;
    m_pPositionReceive->SetGpsTimeBias(bias);
}

#ifdef TMAP_TMC_QC_TESTING
void GpsProviderQnx::changeMMFeedback(bool enabled)
{
   m_useMMFB = enabled;
}

void GpsProviderQnx::changeOnlyTunnelMMFB(bool enabled)
{
    m_useOnlyTunnelMMFB = enabled;
}

void GpsProviderQnx::changeMMFeedbackLog(bool enabled)
{
    m_useSaveMMFBLog = enabled;
}

void GpsProviderQnx::changeMMFeedbackGpsMethod(int method)
{
    method = 2;
    m_MmfbGpsMethod = method;
}

void GpsProviderQnx::changeMMFeedbackAngleOnTunnel(int method)
{
    m_MmfbAngleOnTunnel = method;
}

void GpsProviderQnx::changeMMFeedbackDRCep(bool enabled)
{
    m_MmfbDrCEP = enabled;
}

void GpsProviderQnx::changeMMFeedbackDRCepRange(int method)
{
    m_MmfbDrCEPrange = method;
}

// [TMCJLR-1137] CEP 맵매칭 거리 추가
void GpsProviderQnx::changeMMFeedbackDRCepMMRange(int method)
{
//    m_MmfbDrCEPrange = method;
}

void GpsProviderQnx::convertGpsLogToKML()
{
    // TODO something
}

int GpsProviderQnx::getGpsMethod()
{
    return m_MmfbGpsMethod;
}

void GpsProviderQnx::setLogPath(QString path)
{
    m_pPositionReceive->SetLogPath(path);

    if(m_pLogFileMMFB != NULL)
        delete m_pLogFileMMFB;
    m_pLogFileMMFB = new LogFile(eLogFileType_MMFB, path);
}

void GpsProviderQnx::changeOpenLRLog(bool enabled)
{
    qDebug() << "[MENU] ***************** changeOpenLRLog = " << enabled;
    m_useSaveOpenLRLog = enabled;
}


#endif


void GpsProviderQnx::onNotifyRawDrData(LOCSRV_RAW_DR* rawDr)
{
    if (!m_start) {
        return;
    }

    LocationDataPtr location_dr = LocationDataPool::newDataPtr();
    bool bRet = m_pPositionReceive->InpuRawDrData(rawDr->time_tag, rawDr->position_stat, rawDr->altitude_stat, rawDr->heading_stat, rawDr->speed_stat, rawDr->dir_switch_stat, rawDr->delta_distance_stat, rawDr->gyro_cal_stat, rawDr->tacho_cal_stat, rawDr->direction, rawDr->time_src, rawDr->dr_solution_status, rawDr->tow, rawDr->latitude, rawDr->longitude, rawDr->altitude, rawDr->heading, rawDr->speed, rawDr->delta_distance, rawDr->position_accy, rawDr->altitude_accy, rawDr->heading_accy, rawDr->speed_accy, rawDr->gyro_sample_num, rawDr->tacho_counts, location_dr);
    if(bRet)
    {
        if(!m_pPositionReceive->IsJrlGpsGood())
        {
            m_pPositionReceive->SetJrlGpsGood();
            LocationDataPtr location_gnss = LocationDataPool::newDataPtr();
            location_gnss->init();
            location_gnss->setGpsMode(LocationData::PHONE_GPS);
            location_gnss->setDataStatus(LocationData::ACTIVE);
            location_gnss->setDateTime(location_dr->dateTime());
            location_gnss->setAccuracy(-99);
            location_gnss->setLatitude(0);
            location_gnss->setLongitude(0);
            location_gnss->setSpeed(0);
            location_gnss->setDirection(0);
            location_gnss->setTimeStamp(location_dr->timeStamp());

            emit positionUpdated(location_gnss);
        }
        emit positionUpdated(location_dr);
    }
}

void GpsProviderQnx::onNotifyGnssFixData(LOCSRV_GNSS_FIX* gnssFix)
{
   LocationDataPtr location = LocationDataPool::newDataPtr();
   bool bRet = m_pPositionReceive->InputGnssFixData(gnssFix->tow, gnssFix->fix_src, gnssFix->altitude_hold, gnssFix->dgps_stat, gnssFix->position_stat, gnssFix->altitude_stat, gnssFix->heading_stat, gnssFix->speed_stat, gnssFix->time_src, gnssFix->latitude, gnssFix->longitude, gnssFix->altitude, gnssFix->heading, gnssFix->speed, gnssFix->vertical_speed, gnssFix->position_accy, gnssFix->altitude_accy, gnssFix->heading_accy, gnssFix->speed_accy, gnssFix->vertical_speed_accy, location);
   int nGpsMethod = getGpsMethod();
   if(bRet && (nGpsMethod == 1 || nGpsMethod == 2))
   {
       emit positionUpdated(location);
   }

}

void GpsProviderQnx::onNotifyUtcConstData(LOCSRV_UTC_CONST* utcConst)
{
    m_pPositionReceive->InputUtcConstData(utcConst->time_stamp, utcConst->utc_year, utcConst->utc_month, utcConst->utc_day, utcConst->utc_hour, utcConst->utc_minute, utcConst->utc_second, utcConst->pdop, utcConst->hdop, utcConst->vdop, utcConst->gnss_stat, utcConst->time_src, utcConst->num_vis_sv);
}

void GpsProviderQnx::onNotifyAllSvsInfoData(LOCSRV_ALL_SVS_INFO* svsInfo)
{
    //qWarning() << "void GpsProviderQnx::onNotifyAllSvsInfoData() called.";
#if 0
    m_nmea.m_activeSatellites.clear();
    m_nmea.m_satelliteInfo.clear();

    m_nmea.m_mask = NMEA::RMC | NMEA::GGA;
    for (int i = 0; i < svsInfo->sv_num; i++) {
        NMEA::NmeaSatellite satData;
        m_nmea.m_activeSatellites.insert(svsInfo->sv[i].sv_id);
        satData.satelliteType = NMEA::Talker::GN;
        satData.id = svsInfo->sv[i].sv_id;
        satData.elevation = svsInfo->sv[i].elevation;
        satData.azimuth = svsInfo->sv[i].azimuth;
        satData.snr = svsInfo->sv[i].snr;        // 00-99 dB
        satData.tracked = (bool)svsInfo->sv[i].tracking;
        m_nmea.m_satelliteInfo.append(satData);
        if(svsInfo->sv[i].used_in_fix)
            m_nmea.m_gga.satInUse++;
    }

    SatelliteDataPtr satellite = GpsUtil::getSatelliteFromNmea(m_nmea);
    if (satellite) {
        emit satelliteUpdated(satellite);
    }
#endif
}

void GpsProviderQnx::onNotifyGyroMeasData(LOCSRV_GYRO_MEAS* gyroMeas)
{
    //qWarning() << "[LocationService] onNotifyGyroMeasData() called";
}

void GpsProviderQnx::onNotifyAltFilterData(LOCSRV_ALT_FILTER* altFilter)
{
   //qWarning("delta_alt_stat=%d, cum_delta_alt=%f",altFilter.delta_alt_stat, altFilter.cum_delta_alt);
}

void GpsProviderQnx::onNotifyAccelMeasData(LOCSRV_ACCEL_MEAS* accelMeas)
{
    //qWarning() << "[LocationService] onNotifyAccelMeasData() called";
}

void GpsProviderQnx::onResponseFwInfoData(int& requestID, const LOCSRV_FW_INFO* fwInfo)
{
#if 0
    qWarning("[LocationService] onResponseFwInfoData() called. FwVer=%u.%u.%u (%u-%u-%u)",
       fwInfo.major, fwInfo.minor, fwInfo.build_number,
       fwInfo.release_year, fwInfo.release_month, fwInfo.release_day);
#endif
}

void GpsProviderQnx::onNotifyDrModuleError(E_LOCSRV_ERR* eError)
{

}

void GpsProviderQnx::onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) {
    if (rgdata == NULL) {
        return;
    }
    m_RGData = rgdata;

#ifdef TMAP_TMC_QC_TESTING
    static int g_nLastMmfbCnt = 10;
    if(rgdata->m_bTunnelMapMatchingFeedback || rgdata->m_bUnderpassMapMatchingFeedback)
        g_nLastMmfbCnt = 0;

    g_nLastMmfbCnt++;
    if(g_nLastMmfbCnt > 10)
    {
//        printf("m_pPositionReceive->SetFilterGnss(true) *****************************************************\n"); fflush(stdout);
        m_pPositionReceive->SetFilterGnss(true);
    }else{
//        printf("m_pPositionReceive->SetFilterGnss(false) ####################################################\n"); fflush(stdout);
        m_pPositionReceive->SetFilterGnss(false);
    }

    if (!(bool)rgdata->m_bMapMatchingFeedback) return;

    if (m_useMMFB) {
        if (m_useOnlyTunnelMMFB && !(bool)rgdata->m_bTunnelMapMatchingFeedback && !(bool)rgdata->m_bUnderpassMapMatchingFeedback)
        {
            uint32_t timestamp = rgdata->m_MapMatchingFeedbackInfo.m_Timestamp;
            int32_t method = rgdata->m_MapMatchingFeedbackInfo.m_nGpsMethod;
            memset(&rgdata->m_MapMatchingFeedbackInfo, 0x00, sizeof(VSM_MapMatchingFeedbackInfo));
            rgdata->m_MapMatchingFeedbackInfo.m_Timestamp = timestamp;
            rgdata->m_MapMatchingFeedbackInfo.m_nGpsMethod = method;
            return;
        }

        struct timespec rtime;
        clock_gettime(CLOCK_MONOTONIC, &rtime);
        // JLR일때 3초 단위로 MMFB 전송한다. (정확히 1초 단위로 MMFB가 발생되지 않으므로 2.8초 기준으로 체크한다.
        double current_time = rtime.tv_sec + (double)rtime.tv_nsec/1000000000.0;
        if(current_time < (m_mmfbTime+2.8))
            return;
        if(rgdata->stCurrent.nPosSpeed < 18) // 5m/s = 18km/h
        {
            int dist = 0;
            VSMMap_GetDistanceWorld(m_mmfbX, m_mmfbY, rgdata->m_MapMatchingFeedbackInfo.m_wc.x, rgdata->m_MapMatchingFeedbackInfo.m_wc.y, dist);
            if(dist < 20) // 20m
            {
                //qDebug() << "---------------------------- dist = " << dist;
                return;
            }
        }

        //qDebug() << "---------------------------- send mmfb (" << rgdata->stCurrent.nPosSpeed << ")";

        m_mmfbX = rgdata->m_MapMatchingFeedbackInfo.m_wc.x;
        m_mmfbY = rgdata->m_MapMatchingFeedbackInfo.m_wc.y;
        m_mmfbTime = current_time;

        LOCSRV_MMFB mmfb = {0,};
        mmfb.time_tag = m_RGData->m_MapMatchingFeedbackInfo.m_Timestamp;     // System time tag
        mmfb.latitude = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_mmLatitude;  // Map-matching latitude (unit : WGS84 in degrees)
        mmfb.longitude = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_mmLongitude; // Map-matching longitude (unit : WGS84 in degrees)
        mmfb.heading = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentHeading;    // Heading of current link in the direction vehicle is driving (unit : degrees)
        mmfb.in_tunnel = (m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentFlag == 1) ? 1 : 0;   // 0 : Not in tunnel, 1 : In tunnel
        mmfb.is_bifurcation_ahead = 0; //분기지점 0 : No bifurcation ahead (Set as 0 if this data is unavailable), 1 : Yes
        mmfb.is_righthand_traffic = 1; // 0 : No (Japan, UK, etc), 1 : Yes (America, EU, Korea, China, etc)
        mmfb.is_oneway = (m_RGData->stCurrent.linkcate==1) ? 0 : 1; // 0 : No, 1 : Yes
        mmfb.is_offroad = 0; // 0 : No (not matched to street (for information only)), 1 : Yes
        mmfb.mm_confidence = 255; // Map match confidence level: 0-6, 255 = no confidence
        mmfb.mm_probability = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_probability; // Map match probability
        mmfb.num_of_lanes = m_RGData->m_MapMatchingFeedbackInfo.m_Candidates[0].m_lane; // Number of lanes in the direction vehicle is driving, 0 if unsure
        mmfb.navi_suppliers = 1; // 0 : Nuesoft, 1 : SKP, 2 : Aotunavi, 3 : HERE

        if(m_platformController)
        {
            m_platformController->updateMmfb(mmfb);

            if (m_useSaveMMFBLog && m_pLogFileMMFB)
            {
                QString strLog;
                QTextStream out(&strLog);
                out << mmfb.time_tag << ","
                       << QString::number(mmfb.latitude, 'f', 8) << ","
                       << QString::number(mmfb.longitude, 'f', 8) << ","
                       << (int)mmfb.heading << ","
                       << (uint) mmfb.in_tunnel << ","
                       << mmfb.is_bifurcation_ahead << ","
                       << mmfb.is_righthand_traffic << ","
                       << mmfb.is_oneway << ","
                       << mmfb.is_offroad << ","
                       << mmfb.mm_confidence << ","
                       << mmfb.mm_probability << ","
                       << mmfb.num_of_lanes << ","
                       << mmfb.navi_suppliers;

                m_pLogFileMMFB->WriteString("MMFB", strLog);
            }
        }
    }
#endif
}

PositionProvider* GpsService::newGpsProvider()
{
    g_pGpsProviderQnx = new GpsProviderQnx();
    return g_pGpsProviderQnx;
}

}

