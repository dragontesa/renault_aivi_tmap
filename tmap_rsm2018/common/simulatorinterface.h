#ifndef SIMULATORINTERFACE_H
#define SIMULATORINTERFACE_H

#include <QObject>
#include "rgdataptr.h"

namespace SKT {

class SimulatorInterface
{
public:
    virtual ~SimulatorInterface() {}

    /**
     * @brief qobject
     * @return
     */
    virtual QObject* qobject() = 0;

    virtual int speed() const = 0;
    virtual bool repeat() const = 0;
    virtual bool pause() const = 0;

public slots:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setSpeed(int speed) = 0;
    virtual void goNextGP() = 0;
    virtual void goPreviousGP() = 0;
    virtual void goTo(int dist) = 0;
    virtual void setPause(bool pause) = 0;
    virtual void setRepeat(bool repeat) = 0;

signals:
    // just hint
    void started();
    void stopped();
    void speedChanged(int speed);
    void repeatChanged(bool repeat);
    void pauseChanged(bool pause);
    void routeGuidanceUpdated(SKT::RGDataPtr rgData);
    void remainDistanceChanged(int distance);
};

}

Q_DECLARE_INTERFACE(SKT::SimulatorInterface, "com.skt.tmap.simulator.SimulatorInterface/1.0")

#endif // SIMULATORINTERFACE_H
