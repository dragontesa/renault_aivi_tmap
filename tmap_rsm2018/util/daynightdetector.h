#ifndef DAYNIGHTDETECTOR_H
#define DAYNIGHTDETECTOR_H

#include <QDateTime>
namespace SKT {

/**
 * @brief The DayNightDetector class
 */
class DayNightDetector
{
public:
    /**
     * @brief The TimeType enum
     */
    enum TimeType {
        DAWN = 0,
        DAY,
        TWILIGHT,
        NIGHT
    };

    /**
     * @brief detect
     *
     *      civStart sunrise                         sunset  civEnd
     *         |        |                               |       |
     *  -------*--------*-------------------------------*-------*--------
     *  NIGHT  | DAWN   | DAY                           | DUSK  | NIGHT
     *
     * @param utc
     * @param longitude
     * @param latitude
     * @return NIGHT, DAWN, DAY, DUSK or NIGHT
     */
    static TimeType detect(const QDateTime &utc, double longitude, double latitude, QDateTime(&outTimeSet)[4]);
};

}

#endif // DAYNIGHTDETECTOR_H
