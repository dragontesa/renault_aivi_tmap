#include <QBuffer>
#include <QByteArray>
#include <QDebug>
#include <QTextStream>

#include "gpsservice.h"
#include "gpsprovider_gen3.h"
#include "gpsutil.h"
#include "socket_client.h"
#include "drpositionprovider_gen3.h"

namespace SKT {

PositionProvider* GpsService::newGpsProvider()
{
    bool drPosition = true; //qgetenv("TMAP_ENABLE_DRPOSITION").toInt();
    if (drPosition) {
        return new DrPositionProviderGen3;
    }

    return new GpsProviderGen3();
}

GpsProviderGen3::GpsProviderGen3() : m_timestamp(0), m_locationSent(false)
{
    gnss_socket_client_init();
}

GpsProviderGen3::~GpsProviderGen3()
{

}

void GpsProviderGen3::startUpdates()
{
    gnss_socket_client_start(gnssSocketClientCallback, this);
}

void GpsProviderGen3::stopUpdates()
{
    gnss_socket_client_stop();
    m_timestamp = 0;
    m_nmea.reset();
}

void GpsProviderGen3::sendExtraCommand(const QString &cmd)
{
    Q_UNUSED(cmd)
}

void GpsProviderGen3::changeGpsLogUsage(bool enabled)
{
}
void GpsProviderGen3::applyGpsTimeBias(int bias)
{
}
void GpsProviderGen3::onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) {
    //
}

void GpsProviderGen3::sendLocationData()
{
    LocationDataPtr location = GpsUtil::getLocationFromNmea(m_nmea);
    if (location) {
        location->setGpsMode(LocationData::PHONE_GPS);
        emit positionUpdated(location);
//        qDebug() << "LocationData" << location->toString();
    }
    m_locationSent = true;
}

// run on the gnss socket thread.
void GpsProviderGen3::parseGpsData(unsigned int timestamp, const char* buffer, size_t bufferLen)
{
    if (timestamp != m_timestamp) {
        if (m_timestamp != 0) {
            if (!m_locationSent) {
                sendLocationData();
            }

            SatelliteDataPtr satellite = GpsUtil::getSatelliteFromNmea(m_nmea);
            if (satellite) {
                emit satelliteUpdated(satellite);
                qDebug().noquote() << "SatelliteData" << satellite->toString();
            }

            m_locationSent = false;
            m_nmea.reset();
        }

        m_timestamp = timestamp;
    }

    if (!buffer || bufferLen <= 0) {
        return;
    }

    QByteArray data = QByteArray::fromRawData(buffer, bufferLen);
    QBuffer in(&data);
    if (in.open(QIODevice::ReadOnly)) {
        forever {
            QByteArray line = in.readLine();
            if (line.isEmpty()) {
                break;
            }

            m_nmea.parseSentence(line.constData(), line.count());

            if (!m_locationSent && m_nmea.hasGGA() && m_nmea.hasRMC()) {
                sendLocationData();
            }
        }
        in.close();
    }
}

void GpsProviderGen3::gnssSocketClientCallback(unsigned int timestamp, const char* buffer, size_t bufferLen, void *userdata)
{
    Q_UNUSED(timestamp)

    GpsProviderGen3 *provider = reinterpret_cast<GpsProviderGen3*>(userdata);
    if (provider) {
        provider->parseGpsData(timestamp, buffer, bufferLen);
    }
}

}
