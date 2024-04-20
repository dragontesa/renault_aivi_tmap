#include "satellitedataptr.h"
#include "datetimeutil.h"
#include <QDateTime>

using namespace SKT;

QDateTime SatelliteData::localDateTime()
{
    return DateTimeUtil::toLocalDateTime(QDateTime( date, time, Qt::UTC ));
}
