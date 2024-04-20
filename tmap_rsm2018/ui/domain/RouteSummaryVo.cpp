#include "RouteSummaryVo.h"

#define MAX_LOCAL_DISTANCE  (700 * 1000) // 700km
#define MAX_SERVER_DISTANCE  (1000 * 1000) // 700km

namespace SKT {

RouteSummaryVo::RouteSummaryVo(QObject *parent)
    : QObject(parent)
    , mDistance(0)
    , mWeather(0)
    , mRemainTime(0)
    , mFee(0)
{
}

RouteSummaryVo::RouteSummaryVo(
    const QString &type,
    const QString &from, const QString &to,
    const QString &roadInfo,
    const QPoint &min, const QPoint &max,
    int distance,
    int weather,
    int remainTime,
    int fee,
    QObject *parent)
    : mType(type)
    , mFrom(from)
    , mTo(to)
    , mRoadInfo(roadInfo)
    , mMinPoint(min)
    , mMaxPoint(max)
    , mDistance(distance)
    , mWeather(weather)
    , mRemainTime(remainTime)
    , mFee(fee)
    , QObject(parent)
{
}

RouteSummaryVo::~RouteSummaryVo()
{
}

RouteSummaryVo::RouteSummaryVo(const RouteSummaryVo &other)
    : mType(other.mType)
    , mFrom(other.mFrom)
    , mTo(other.mTo)
    , mRoadInfo(other.mRoadInfo)
    , mMinPoint(other.mMinPoint)
    , mMaxPoint(other.mMaxPoint)
    , mDistance(other.mDistance)
    , mWeather(other.mWeather)
    , mRemainTime(other.mRemainTime)
    , mFee(other.mFee)
    , QObject(other.parent())
{
}

RouteSummaryVo &RouteSummaryVo::operator=(const RouteSummaryVo &other)
{
    if (this != &other) {
        mType = other.mType;
        mFrom = other.mFrom;
        mTo = other.mTo;
        mRoadInfo = other.mRoadInfo;
        mMinPoint = other.mMinPoint;
        mMaxPoint = other.mMaxPoint;
        mDistance = other.mDistance;
        mWeather = other.mWeather;
        mRemainTime = other.mRemainTime;
        mFee = other.mFee;
        setParent(other.parent());
    }
    return *this;
}

void RouteSummaryVo::setType(const QString &type)
{
    if (mType != type) {
        mType = type;
        emit typeChanged();
    }
}

void RouteSummaryVo::setFrom(const QString &from)
{
    if (mFrom != from) {
        mFrom = from;
        emit fromChanged();
    }
}

void RouteSummaryVo::setTo(const QString &to)
{
    if (mTo != to) {
        mTo = to;
        emit toChanged();
    }
}

void RouteSummaryVo::setRoadInfo(const QString &roadInfo)
{
    if (mRoadInfo != roadInfo) {
        mRoadInfo = roadInfo;
        emit roadInfoChanged();
    }
}

void RouteSummaryVo::setMinPoint(const QPoint &pt)
{
    if (mMinPoint != pt) {
        mMinPoint = pt;
        emit minPointChanged();
    }
}

void RouteSummaryVo::setMaxPoint(const QPoint &pt)
{
    if (mMaxPoint != pt) {
        mMaxPoint = pt;
        emit maxPointChanged();
    }
}

void RouteSummaryVo::setDistance(int distance)
{
    if (mDistance != distance) {
        mDistance = distance;
        emit distanceChanged();
    }
}

void RouteSummaryVo::setWeather(int weather)
{
    if (mWeather != weather) {
        mWeather = weather;
        emit weatherChanged();
    }
}

void RouteSummaryVo::setRemainTime(int time)
{
    if (mRemainTime != time) {
        mRemainTime = time;
        emit remainTimeChanged();
    }
}

void RouteSummaryVo::setFee(int fee)
{
    if (mFee != fee) {
        mFee = fee;
        emit feeChanged();
    }
}

bool RouteSummaryVo::isDistanceOver(bool isOnline) const
{
#if (1)
    return false;
#else
    if (isOnline) {
        return mDistance > MAX_SERVER_DISTANCE;
    } else {
        return mDistance > MAX_LOCAL_DISTANCE;
    }
#endif
}

bool RouteSummaryVo::hasData() const
{
    return !mMinPoint.isNull() || !mMaxPoint.isNull();
}

QString RouteSummaryVo::type() const
{
    return mType;
}

QString RouteSummaryVo::from() const
{
    return mFrom;
}

QString RouteSummaryVo::to() const
{
    return mTo;
}

QString RouteSummaryVo::roadInfo() const
{
    return mRoadInfo;
}

QPoint RouteSummaryVo::minPoint() const
{
    return mMinPoint;
}

QPoint RouteSummaryVo::maxPoint() const
{
    return mMaxPoint;
}

int RouteSummaryVo::distance() const
{
    return mDistance;
}

int RouteSummaryVo::weather() const
{
    return mWeather;
}

int RouteSummaryVo::remainTime() const
{
    return mRemainTime;
}

int RouteSummaryVo::fee() const
{
    return mFee;
}

}//SKT
