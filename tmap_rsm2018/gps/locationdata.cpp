#include "locationdataptr.h"
#include "datetimeutil.h"

using namespace SKT;

QDateTime LocationData::localDateTime() const
{
    return DateTimeUtil::toLocalDateTime(m_dateTime);
}
