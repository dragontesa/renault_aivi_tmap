#ifndef SATELLITEDATAPTR_H
#define SATELLITEDATAPTR_H

#include <QDate>
#include <QTime>
#include <QSharedPointer>
#include <QSet>
#include <QString>
#include <QVector>
#include "nmeatype.h"

namespace SKT {

struct SatelliteData {
    SatelliteData() {
        init();
    }

    void init() {
        date = QDate();
        time = QTime();
        latitude = 0;
        longitude = 0;
        fixType = 1;
        speed = 0;
        altitude = 0;
        HDOP = 0;
        satellitesInUse = 0;

        satellites.clear();
        activeSatellites.clear();
    }

    QDateTime localDateTime();
    QString toString();

    QDate date;         // utc date
    QTime time;         // utc time    
    double latitude;    // degree
    double longitude;   // degree
    int fixType;        // 1: No fix, 2: 2D fix, 3: 3D fix
    int speed;          // knot
    int altitude;       // meter
    double HDOP;

    int satellitesInUse;
    QVector<NMEA::NmeaSatellite> satellites;    // in view
    QSet<int> activeSatellites;
};

typedef QSharedPointer<SatelliteData> SatelliteDataPtr;

}
Q_DECLARE_METATYPE(SKT::SatelliteData)
Q_DECLARE_METATYPE(SKT::SatelliteDataPtr)

#endif // SATELLITEDATAPTR_H
