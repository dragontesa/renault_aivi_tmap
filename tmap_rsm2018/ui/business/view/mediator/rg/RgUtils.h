#ifndef RGUTILS_H
#define RGUTILS_H

#include "RgConstants.h"

namespace SKT {

class RgUtils
{
public:
    static QString formatDistance(int dist);

    static QString tbtFirstImage( int code );
    static QString tbtSecondImage( int code );
    static QString tbtFirstDistanceNumberImage( QChar num );
    static QString tbtSecondDistanceNumberImage( QChar num );

    static QString sdiSignNumberImage(QChar num);
    static QString sdiSignNumberSmallImage(QChar num);

    static QString sdiSectionSpeedAverageNumberImage(RgConstants::SdiSectionSpeedNumberType numberType, QChar num);
    static QString sdiSectionSpeedTimeImage(RgConstants::SdiSectionSpeedNumberType numberType, QChar num);
    static QString sdiSpeedSectionAverageNumberKmhImage(RgConstants::SdiSectionSpeedNumberType numberType);
    static QString sdiDistanceNumberImage(QChar num);
    static QString sdiCurrentSpeedNumberImage(QChar num, bool alert);

    static QString laneImage( int laneIndex, const QString& code );
    static QString laneDistanceNumberImage( int laneIndex, QChar num );

    static QString highwayDistanceNumberImage( QChar num );
    static QString highwayCongestionImage( int congestion );
    static QString highwayStationImage( int station );

    static QString summaryTbtSecondImage( int code );
    static QString summaryTbtSecondDistanceNumberImage(QChar num);
    static QString summaryTbtListDistanceNumberImage( QChar num );
    static QString summaryTbtCongestionImage( int congestion );
};

}

#endif // RGUTILS_H
