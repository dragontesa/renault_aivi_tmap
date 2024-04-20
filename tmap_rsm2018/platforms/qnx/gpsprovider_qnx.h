#ifndef GPSPROVIDER_QNX_H
#define GPSPROVIDER_QNX_H

#include "positionprovider.h"
#include "nmeadata.h"
#include "positionreceive.h"
#include "logfile.h"
#include "rgdatapool.h"
#include "qnx_interface_def.h"
#include <QDebug>

//#define SAVE_GPSLOG
#ifdef SAVE_GPSLOG
#include <fstream>
#include <iostream>
#endif

namespace SKT {

class LogFile;
class GpsService;
class GpsProviderQnx : public PositionProvider
{
public:
    GpsProviderQnx();
    virtual ~GpsProviderQnx();

public slots:
    void startUpdates() override;
    void stopUpdates() override;
    void sendExtraCommand(const QString &cmd) override;
    void changeGpsLogUsage(bool enabled) override;
    void applyGpsTimeBias(int bias) override;
    void onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) override;

    TMAP_TMC_QC__MMFB_OVERRIDE

    void UpdateLocationData(int nMsg, void* lpParam) override;

public:
    void onNotifyRawDrData(LOCSRV_RAW_DR* rawDr);
    void onNotifyGnssFixData(LOCSRV_GNSS_FIX* gnssFix);
    void onNotifyUtcConstData(LOCSRV_UTC_CONST* utcConst);
    void onNotifyAllSvsInfoData(LOCSRV_ALL_SVS_INFO* svsInfo);
    void onNotifyGyroMeasData(LOCSRV_GYRO_MEAS* gyroMeas);
    void onNotifyAltFilterData(LOCSRV_ALT_FILTER* altFilter);
    void onNotifyAccelMeasData(LOCSRV_ACCEL_MEAS* accelMeas);
    void onResponseFwInfoData(int& requestID, const LOCSRV_FW_INFO* fwInfo);
    void onNotifyDrModuleError(E_LOCSRV_ERR* eError);

private:
//    QnxGpsInfo m_gpsinfo;
//    NmeaData m_nmea;

    PositionReceive* m_pPositionReceive;
    LogFile *m_pLogFileMMFB;

    QMutex m_logMutex;

    bool m_start;
    bool m_useGpsLog; // gps log usage , default is true, by lckxx 2018.1.3;
    TMAP_TMC_QC_MMFB_DECLARE  // mmfb usage, by lckxx 2018.4.18
    RGDataPtr   m_RGData;
    double m_mmfbTime;
    int m_mmfbX, m_mmfbY;
};

}

#endif GPSPROVIDER_QNX_H
