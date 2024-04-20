#include "drdata.h"
#include <QDebug>
namespace SKT {

bool drdata::parse(const char *data, int size) {
    const QList<QByteArray> parts = QByteArray::fromRawData(data, size-1).split(',');  //remove whitespace characters size -1
    if (parts.count() != 21) {
        return false;
    }
    bool ok;
    QString sTmp = QString::fromUtf8(parts[2].toStdString().c_str());
    mTimestamp          = parts[1].toUInt();
    mTimePosix          = QDateTime::fromString(sTmp, "yyyy-M-d H:m:s.zzz");
    mLongitude          = parts[3].toDouble();
    mLatitude           = parts[4].toDouble();
    mPosErrCepLarge     = parts[5].toInt();
    mPosErrCepSmall     = parts[6].toInt();
    mPosErrCepHeading   = parts[7].toInt();
    mHeading            = parts[8].toInt();
    mHeadingError       = parts[9].toInt();
    mTurnrate           = parts[10].toDouble();
    mTurnrateError      = parts[11].toDouble();
    mSpeed              = parts[12].toShort();
    mSpeedError         = parts[13].toShort();
    mAcceleration       = parts[14].toShort();
    mAccelerationError  = parts[15].toShort();
    mHeight             = parts[16].toInt();
    mHeightError        = parts[17].toInt();
    mSensorFlags        = parts[18].toUShort(&ok, 16);
    mDrInfoValidity     = parts[19].toUShort(&ok,16);
    mHdop               = parts[20].toDouble();
    return true;
}

}

