#ifndef DRDATA_H
#define DRDATA_H

#include <QVector>
#include <QSet>
#include <QDateTime>

namespace SKT {
struct drdata {
    uint mTimestamp;
    QDateTime mTimePosix;
    double mLongitude;
    double mLatitude;
    int mPosErrCepLarge;
    int mPosErrCepSmall;
    int mPosErrCepHeading;
    int mHeading;
    int mHeadingError;
    double mTurnrate;
    double mTurnrateError;
    short mSpeed;
    short mSpeedError;
    short mAcceleration;
    short mAccelerationError;
    int mHeight;
    int mHeightError;
    ushort mSensorFlags;
    ushort mDrInfoValidity;
    double mHdop;

    bool parse (const char* data, int size);

};
}


#endif // DRDATA_H
