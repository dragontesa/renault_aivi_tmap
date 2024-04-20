#ifndef NMEADATA_H
#define NMEADATA_H

#include "nmeatype.h"
#include <QSet>
#include <QVector>

namespace SKT {

struct NmeaData
{
    NmeaData();
    void reset();

    bool hasGGA() const;
    bool hasRMC() const;
    bool hasGSA() const;

    int parseSentence(const char* data, int size, int parseTypes = NMEA::ALL);

    static NMEA::NmeaType getNmeaType(const char *data, int size, NMEA::Talker* talker = nullptr);
    static bool parseGGA(const char* data, int size, NMEA::NmeaGGA* gga);
    static bool parseGSA(const char* data, int size, NMEA::NmeaGSA* gsa);
    static bool parseGSV(const char* data, int size, NMEA::NmeaGSV* gsv, NMEA::Talker talker = NMEA::GP);
    static bool parseRMC(const char* data, int size, NMEA::NmeaRMC* rmc);

    NMEA::NmeaGGA m_gga;
    NMEA::NmeaRMC m_rmc;
    NMEA::NmeaGSA m_gsa;
    QVector<NMEA::NmeaSatellite> m_satelliteInfo;
    QSet<int> m_activeSatellites;

    // parsed nmea
    int m_mask;
};

}

#endif // NMEADATA_H
