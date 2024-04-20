#include <QDate>
#include <QTime>
#include "daynightdetector.h"
#include "sunrisesunsetcalculator.h"
#include <QDebug>
#include <QTimeZone>
#include "datetimeutil.h"

namespace SKT {

DayNightDetector::TimeType DayNightDetector::detect(
    const QDateTime &utc,
     double longitude,
      double latitude,
      QDateTime(&outTimeSet)[4])
{
    // we can just pass this day as KST since sun_rise_set function requires calendar date.
    // this also prevents the date from being dealt with as the previous day in the morning.
    QDate date = DateTimeUtil::toLocalDateTime(utc).date();

    int year = date.year();
    int month = date.month();
    int day = date.day();

    double sunrise, sunset;
    sun_rise_set(year, month, day, longitude, latitude, &sunrise, &sunset);

    double civ_start, civ_end;
    civil_twilight(year, month, day, longitude, latitude, &civ_start, &civ_end);

    QDateTime sunriseTime;
    QDateTime sunsetTime;
    QDateTime civStart;
    QDateTime civEnd;

    const int KST = 9 * 60 * 60;
    QDateTime today(date);
    today.setTimeZone(QTimeZone(KST));

    // the results from sun_rise_set function are stored in UT(C), so we have to add 9 hours only for the time values returned.
    sunriseTime = today.addSecs(sunrise * 3600 + KST).toUTC();
    sunsetTime = today.addSecs(sunset * 3600 + KST).toUTC();
    civStart = today.addSecs(civ_start * 3600 + KST).toUTC();
    civEnd = today.addSecs(civ_end * 3600 + KST).toUTC();

    outTimeSet[0] = sunriseTime;
    outTimeSet[1] = sunsetTime;
    outTimeSet[2] = civStart;
    outTimeSet[3] = civEnd;

    qDebug() << "[Time][DayNightDetector]currentTime: " << DateTimeUtil::toLocalDateTime(utc);
    qDebug() << "[Time][DayNightDetector]sunriseTime: " << DateTimeUtil::toLocalDateTime(sunriseTime);
    qDebug() << "[Time][DayNightDetector]sunsetTime: " << DateTimeUtil::toLocalDateTime(sunsetTime);
    qDebug() << "[Time][DayNightDetector]civStart: " << DateTimeUtil::toLocalDateTime(civStart);
    qDebug() << "[Time][DayNightDetector]civEnd: " << DateTimeUtil::toLocalDateTime(civEnd);

    if (utc >= sunriseTime && utc < sunsetTime) {
        return DAY;
    } else if (utc >= sunsetTime && utc < civEnd) {
        return TWILIGHT;
    } else if (utc >= civStart && utc < sunriseTime) {
        return DAWN;
    }

    return NIGHT;
}

}
