#ifndef GPSPROVIDER_H
#define GPSPROVIDER_H

#include "positionprovider.h"
#include "nmeadata.h"

namespace SKT {

class GpsProviderGen3 : public PositionProvider
{
    Q_OBJECT
public:
    GpsProviderGen3();
    virtual ~GpsProviderGen3();

public slots:
    void startUpdates() override;
    void stopUpdates() override;
    void sendExtraCommand(const QString &cmd) override;
    void changeGpsLogUsage(bool enabled) override;
    void applyGpsTimeBias(int bias) override;
    void onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) override;

protected:
    void sendLocationData();
    void parseGpsData(unsigned int timestamp, const char* buffer, size_t bufferLen);
    static void gnssSocketClientCallback(unsigned int timestamp, const char* buffer, size_t bufferLen, void *userdata);

private:
    NmeaData m_nmea;
    unsigned int m_timestamp;
    bool m_locationSent;
};

}

#endif // GPSPROVIDER_H
