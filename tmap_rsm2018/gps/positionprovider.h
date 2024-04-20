#ifndef POSITIONPROVIDER_H
#define POSITIONPROVIDER_H

#include <QObject>
#include <QString>
#include "locationdataptr.h"
#include "satellitedataptr.h"
#include "rgdatapool.h"
#ifdef PLATFORM_QNX
#include "qnx_interface_def.h"
#include "platformcontroller.h"
#endif
#include "qctestingfeatures.h"

namespace SKT {

class PositionProvider : public QObject
{
    Q_OBJECT
public slots:
    virtual void startUpdates() = 0;
    virtual void stopUpdates() = 0;
    virtual void sendExtraCommand(const QString &cmd) = 0;
#ifdef PLATFORM_QNX
    virtual void UpdateLocationData(int nMsg, void* lpParam) = 0;
#endif
    virtual void changeGpsLogUsage(bool enabled) = 0; // gps log usage state [hidden menu spec]
    virtual void applyGpsTimeBias(int bias) = 0; // adjust gps time [hidden menu spec]
    virtual void onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) = 0;
    
    TMAP_TMC_QC__MMFB
    
signals:
    void positionUpdated(SKT::LocationDataPtr location);
    void satelliteUpdated(SKT::SatelliteDataPtr satellite);

public:
#ifdef PLATFORM_QNX
    void SetPlatformController(PlatformController *platformController) {m_platformController = platformController;};
    qnx_location_callback m_locationcb;
    PlatformController *m_platformController;
#endif
};

}

#endif // POSITIONPROVIDER_H
