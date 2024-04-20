#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <QDateTime>
#include "navigationexttype.h"

namespace SKT {

class DateTimeUtil
{
public:    
    static QDateTime currentUtcDateTime();
    static QDateTime currentLocalDateTime();

    // UTC => Local Time(KST)
    static QDateTime toLocalDateTime(const QDateTime& dateTime);

    static void setNaviDateTimeDetailInfo(NXDateTimeDetailInfo dateTimeDetailInfo);
};

}

#endif
