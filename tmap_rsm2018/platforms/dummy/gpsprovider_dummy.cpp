#include "positionprovider.h"
#include "gpsservice.h"

namespace SKT {

class GpsProviderDummy : public PositionProvider
{
public:
    GpsProviderDummy() {

    }

    virtual ~GpsProviderDummy() {

    }

    void startUpdates() override {

    }

    void stopUpdates() override {

    }

    void sendExtraCommand(const QString &cmd) override {
        Q_UNUSED(cmd)
    }

    void changeGpsLogUsage(bool enabled) {}

    void applyGpsTimeBias(int bias) {}
    void onRouteGuidanceUpdated(SKT::RGDataPtr rgdata){}

};

PositionProvider* GpsService::newGpsProvider()
{
    return new GpsProviderDummy();
}

}

