#ifndef GPSMANAGERINTERFACE_H
#define GPSMANAGERINTERFACE_H

#include <QObject>
#include "locationdataptr.h"
#include "satellitedataptr.h"
#include "rgdataptr.h"
#include "../test/qctestingfeatures.h"

namespace SKT {

class GpsManagerInterface
{
public:
    virtual ~GpsManagerInterface() {}

    /**
     * @brief qobject
     * @return
     */
    virtual QObject* qobject() = 0;

    /**
     * @brief lastPosition
     * @param lon
     * @param lat
     */
    virtual void lastPosition(double &lon, double &lat) const = 0;

    /**
     * @brief subscribeRouteGuidance
     * @param subscribe
     */
    virtual void subscribeRouteGuidance(bool subscribe) = 0;

    /**
     * @brief startUpdates
     */
    virtual void startUpdates() = 0;

    /**
     * @brief stopUpdates
     */
    virtual void stopUpdates() = 0;

    /**
     * @brief sendExtraCommand
     * @param cmd
     */
    virtual void sendExtraCommand(const QString &cmd) = 0;

    /**
     * @brief gps log usage [hidden menu spec]
     */
    virtual void changeGpsLogUsage(bool enabled) = 0;

    /**
     * @brief gps mapmatching feedback from vsm [hidden menu spec]
     */
     TMAP_TMC_QC__MMFB
     

signals:
    // just hint....
    void locationUpdated(SKT::LocationDataPtr location);
    void satelliteUpdated(SKT::SatelliteDataPtr satellite);
    void routeGuidanceUpdated(SKT::RGDataPtr rgdata);
};

}

Q_DECLARE_INTERFACE(SKT::GpsManagerInterface, "com.skt.tmap.gps.GpsManagerInterface/1.0")


#endif // GPSMANAGERINTERFACE_H
