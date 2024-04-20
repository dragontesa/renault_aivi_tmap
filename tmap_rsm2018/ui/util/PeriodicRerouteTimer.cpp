#include "PeriodicRerouteTimer.h"
#include "TRandomUtils.h"

#define FIVE_MIN    (5*60)
#define TEN_MIN     (10*60)

namespace SKT
{

PeroidicRerouteTimer::PeroidicRerouteTimer(QObject* parent)
    : mValue(100)
    , mTimerForDebugMode(FIVE_MIN)
    , QObject(parent)
{
}

int PeroidicRerouteTimer::getRerouteTime() const
{
    switch (mValue) {
    case 100:
        return FIVE_MIN;
    case 50:
        if (TRandomUtils::instance()->randInt(0, 100) < 50)
            return FIVE_MIN;
        else
            return TEN_MIN;
    case 0:
        return TEN_MIN;

    default:
        return mTimerForDebugMode;
    }
}

void PeroidicRerouteTimer::setValue(int value)
{
    mValue = value;
}

void PeroidicRerouteTimer::setTimerValueForDebugMode(int value)
{
    mValue = 1;
    mTimerForDebugMode = value;

    qDebug() << "==================================================================";
    qDebug() << "| periodic reroute timer for debugging : " << value;
    qDebug() << "==================================================================";
}

}
