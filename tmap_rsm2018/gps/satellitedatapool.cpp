#include <QDebug>
#include "satellitedatapool.h"

#define SATELLITEDATA_POOL_SIZE      5

namespace SKT {

FixedPool<SatelliteData> SatelliteDataPool::s_fixedPool(SATELLITEDATA_POOL_SIZE);

QString SatelliteData::toString()
{
    return QString("date=%1 time=%2 lat=%3 lon=%4 fixType=%5 speed=%6 altitude=%7 HDOP=%8 satInUse=%9 satInView=%10")
            .arg(date.toString("yyyyMMdd"))
            .arg(time.toString("hhmmss.zzz"))
            .arg(latitude)
            .arg(longitude)
            .arg(fixType)
            .arg(speed)
            .arg(altitude)
            .arg(HDOP)
            .arg(satellitesInUse)
            .arg(satellites.count());
}

}
