#ifndef POSITIONCACHE_H
#define POSITIONCACHE_H

#include <QMutex>

class QSettings;

namespace SKT {

class PositionCache
{
public:
    PositionCache(const QString &cacheFileName,
                  double defaultLongitude,
                  double defaultLatitude);
    ~PositionCache();

    void setPosition(double longitude, double latitude);
    void position(double &longitude, double &latitude) const;

private:
    Q_DISABLE_COPY(PositionCache)

    static const QString kPosition;
    static const QString kLongitude;
    static const QString kLatitude;

    mutable QMutex m_mutex;
    double m_longitude;
    double m_latitude;
    QSettings* m_settings;
};

}

#endif // POSITIONCACHE_H
