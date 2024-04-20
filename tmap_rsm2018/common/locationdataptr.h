#ifndef LOCATIONDATAPTR_H
#define LOCATIONDATAPTR_H

#include <QDateTime>
#include <QSharedPointer>
#include <QString>

namespace SKT {

class LocationData
{
public:
    enum GpsMode {
        PHONE_GPS = 1,
        DR_GPS
    };

    enum DataStatus {
        ACTIVE = 0,
        INACTIVE        // VOID
    };

    LocationData();
    LocationData(const QDateTime &datetime,
                 DataStatus status,
                 double latitude,
                 double longitude,
                 double speed,
                 double direction,
                 double accuracy,
                 double altitude,
                 GpsMode gpsMode,
                 uint timeStamp,
                 bool isDrActive,
                 int drSolutionStation);

    void init();

    QDateTime dateTime() const;
    QDateTime localDateTime() const;
    DataStatus dataStatus() const;
    double latitude() const;
    double longitude() const;
    double speed() const;
    double direction() const;
    double accuracy() const;
    double altitude() const;
    GpsMode gpsMode() const;
    uint timeStamp() const;
    bool isDrActive() const;
    int drSolutionStation() const;

    void setDateTime(const QDateTime &dateTime);
    void setDataStatus(DataStatus status);
    void setLatitude(double lat);
    void setLongitude(double lon);
    void setSpeed(double speed);
    void setDirection(double direction);
    void setAccuracy(double accuracy);
    void setAltitude(double altitude);
    void setGpsMode(GpsMode gpsMode);
    void setTimeStamp(uint timeStamp);
    void setIsDrActive(bool isDrActivep);
    void setDrSolutionStation(int drSolutionStation);

    QString toString();

private:
    QDateTime m_dateTime;
    DataStatus m_dataStatus;
    double m_latitude;
    double m_longitude;
    double m_speed;
    double m_direction;

    double m_accuracy;
    double m_altitude;

    GpsMode m_gpsMode;
    uint m_timeStamp;
    bool m_isDrActive;
    int m_drSolutionStation;
};

typedef QSharedPointer<LocationData> LocationDataPtr;

// inline functions
inline LocationData::LocationData()
{
    init();
}

inline LocationData::LocationData(const QDateTime &datetime,
                                  DataStatus status,
                                  double latitude,
                                  double longitude,
                                  double speed,
                                  double direction,
                                  double accuracy,
                                  double altitude,
                                  GpsMode gpsMode,
                                  uint timeStamp,
                                  bool isDrActive,
                                  int drSolutionStation)
    : m_dateTime(datetime),
      m_dataStatus(status),
      m_latitude(latitude),
      m_longitude(longitude),
      m_speed(speed),
      m_direction(direction),
      m_accuracy(accuracy),
      m_altitude(altitude),
      m_gpsMode(gpsMode),
      m_timeStamp(timeStamp),
      m_isDrActive(isDrActive),
      m_drSolutionStation(drSolutionStation)
{

}

inline void LocationData::init()
{
    m_dateTime = QDateTime();
    m_dataStatus = DataStatus::INACTIVE;
    m_latitude = 0;
    m_longitude = 0;
    m_speed = 0;
    m_direction = 0;
    m_accuracy = 0;
    m_altitude = 0;
    m_gpsMode = GpsMode::PHONE_GPS;
    m_timeStamp = 0;
    m_isDrActive = false;
    m_drSolutionStation = 0;
}

inline QDateTime LocationData::dateTime() const
{
    return m_dateTime;
}

inline LocationData::DataStatus LocationData::dataStatus() const
{
    return m_dataStatus;
}

inline double LocationData::latitude() const
{
    return m_latitude;
}

inline double LocationData::longitude() const
{
    return m_longitude;
}

inline double LocationData::speed() const
{
    return m_speed;
}

inline double LocationData::direction() const
{
    return m_direction;
}

inline double LocationData::accuracy() const
{
    return m_accuracy;
}

inline double LocationData::altitude() const
{
    return m_altitude;
}

inline LocationData::GpsMode LocationData::gpsMode() const
{
    return m_gpsMode;
}

inline uint LocationData::timeStamp() const
{
    return m_timeStamp;
}

inline bool LocationData::isDrActive() const
{
    return m_isDrActive;
}

inline int LocationData::drSolutionStation() const
{
    return m_drSolutionStation;
}

inline void LocationData::setDateTime(const QDateTime &dateTime)
{
    m_dateTime = dateTime;
}

inline void LocationData::setDataStatus(LocationData::DataStatus status)
{
    m_dataStatus = status;
}

inline void LocationData::setLatitude(double lat)
{
    m_latitude = lat;
}

inline void LocationData::setLongitude(double lon)
{
    m_longitude = lon;
}

inline void LocationData::setSpeed(double speed)
{
    m_speed = speed;
}

inline void LocationData::setDirection(double direction)
{
    m_direction = direction;
}

inline void LocationData::setAccuracy(double accuracy)
{
    m_accuracy = accuracy;
}

inline void LocationData::setAltitude(double altitude)
{
    m_altitude = altitude;
}

inline void LocationData::setGpsMode(LocationData::GpsMode gpsMode)
{
    m_gpsMode = gpsMode;
}

inline void LocationData::setTimeStamp(uint timeStamp)
{
    m_timeStamp = timeStamp;
}

inline void LocationData::setIsDrActive(bool isDrActive)
{
    m_isDrActive = isDrActive;
}

inline void LocationData::setDrSolutionStation(int drSolutionStation)
{
    m_drSolutionStation = drSolutionStation;
}

} // SKT

Q_DECLARE_METATYPE(SKT::LocationDataPtr)

#endif // LOCATIONDATAPTR_H
