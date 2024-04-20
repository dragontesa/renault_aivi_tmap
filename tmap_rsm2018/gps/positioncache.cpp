#include <QSettings>
#include "positioncache.h"
#include "VSM_Navi.h"

namespace SKT {

const QString PositionCache::kPosition = "position";
const QString PositionCache::kLongitude = "lon";
const QString PositionCache::kLatitude = "lat";

PositionCache::PositionCache(const QString &cacheFileName,
                             double defaultLongitude,
                             double defaultLatitude)
{
    m_settings = new QSettings(cacheFileName, QSettings::IniFormat);

    m_settings->beginGroup(kPosition);
    double longitude, latitude;
    longitude = m_settings->value(kLongitude, defaultLongitude).toDouble();
    latitude = m_settings->value(kLatitude, defaultLatitude).toDouble();

    // VSM 초기화가 안되었으므로 VSMNavi_XXX  함수를 사용할수 없음 (한반도 영역을 임으로 정하여 체크)
//    if(VSMNavi_IsKoreaArea(longitude, latitude))
    if(latitude > 32.0 && latitude < 40.0 && longitude > 124.0 && longitude < 132.0)
    {
        m_longitude = longitude;
        m_latitude = latitude;
    }else
    {
        m_longitude = defaultLongitude;
        m_latitude = defaultLatitude;
    }

    m_settings->endGroup();
}

PositionCache::~PositionCache()
{
    delete m_settings;
}

void PositionCache::setPosition(double longitude, double latitude)
{
    if (longitude == 0 || latitude == 0)
        return;

    QMutexLocker locker(&m_mutex);

    m_longitude = longitude;
    m_latitude = latitude;

    m_settings->beginGroup(kPosition);
    m_settings->setValue(kLongitude, longitude);
    m_settings->setValue(kLatitude, latitude);
    m_settings->endGroup();
}

void PositionCache::position(double &longitude, double &latitude) const
{
    QMutexLocker locker(&m_mutex);

    longitude = m_longitude;
    latitude = m_latitude;
}

}
