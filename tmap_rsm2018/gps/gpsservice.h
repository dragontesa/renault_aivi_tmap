#ifndef GPSSERVICE_H
#define GPSSERVICE_H

#include <QObject>
#include "gpsmanagerinterface.h"
#include "qctestingfeatures.h"
#ifdef PLATFORM_QNX
#include "qnx_interface_def.h"
#include "platformcontroller.h"
#endif
#include "locationdatapool.h"
#include "logfile.h"

namespace SKT {

class PositionProvider;
class GpsServiceWorker;

class GpsService : public QObject, public GpsManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::GpsManagerInterface)
public:
    GpsService(QObject *parent = nullptr);
    virtual ~GpsService();

    void InitNaviEngine();

    QObject* qobject() override { return this; }
    void lastPosition(double &lon, double &lat) const override;
    void subscribeRouteGuidance(bool subscribe) override;

    static void MountCallback(bool enable);
    void OnMountMapFolder(bool enable);

#ifdef PLATFORM_QNX
    qnx_location_callback m_locationcb;
    PlatformController *m_platformController;
    void SetPlatformController(PlatformController *platformController);
#endif

public slots:
    void startUpdates() override;
    void stopUpdates() override;
    void sendExtraCommand(const QString &cmd) override;
    void changeGpsLogUsage(bool enabled) override;
    void applyGpsTimeBias(int bias);

    TMAP_TMC_QC__MMFB_OVERRIDE

signals:
    void locationUpdated(SKT::LocationDataPtr location);
    void satelliteUpdated(SKT::SatelliteDataPtr satellite);
    void routeGuidanceUpdated(SKT::RGDataPtr rgdata);
    void mountMapFolder(bool enable);

private:
    PositionProvider* newGpsProvider();

private:
    GpsServiceWorker *m_worker;
    PositionProvider *m_provider;
public:
    bool m_useSaveMMFBLog; //  맵매칭 피드백 로그 저장
    bool m_useSaveOpenLRLog;
    QString m_logPath;
    LogFile *m_pLogFileMM;
};

}

#endif // GPSSERVICE_H
