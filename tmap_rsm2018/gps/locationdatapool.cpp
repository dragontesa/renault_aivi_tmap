#include <QDebug>
#include "locationdatapool.h"

#define LOCATIONDATA_POOL_SIZE      5

namespace SKT {

FixedPool<LocationData> LocationDataPool::s_fixedPool(LOCATIONDATA_POOL_SIZE);

QString LocationData::toString()
{
    return QString::fromLatin1("datetime=%1 status=%2 lat=%3 lon=%4 speed=%5 direction=%6 accuracy=%7 altitude=%8 gpsMode=%9")
            .arg(m_dateTime.toString("yyyyMMdd hhmmss.zzz"))
            .arg(m_dataStatus == ACTIVE ? "A" : "V")
            .arg(m_latitude)
            .arg(m_longitude)
            .arg(m_speed)
            .arg(m_direction)
            .arg(m_accuracy)
            .arg(m_altitude)
            .arg(m_gpsMode);
}

}
