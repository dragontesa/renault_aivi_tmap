#include "datetimeutil.h"
#include <QTimeZone>
#include <QMutex>

using namespace SKT;

class DateTimeUtilInternal {
public:
    static DateTimeUtilInternal* instance() {
        static DateTimeUtilInternal Instance;
        return &Instance;
    }

    QDateTime naviDateTime() {
        QMutexLocker lock(&m_mutex);
        QDateTime ret = mNaviDateTime;
        if (!ret.isValid()) {
            ret = QDateTime::currentDateTimeUtc();
            ret.setTimeZone(QTimeZone(9 * 60 * 60));
        }
        return ret;
    }

    void setNaviDateTime(NXDateTimeDetailInfo dateTimeDetailInfo) {
        QMutexLocker lock(&m_mutex);
        QDate date(dateTimeDetailInfo.m_year,dateTimeDetailInfo.m_month,dateTimeDetailInfo.m_day);
        QTime time(dateTimeDetailInfo.m_hours,dateTimeDetailInfo.m_minutes,dateTimeDetailInfo.m_seconds);
        mNaviDateTime = QDateTime(date, time, QTimeZone(9 * 60 * 60));

        qDebug() << "[Time][DateTimeUtil]naviDateTime: " << mNaviDateTime;
    }

private:
    QMutex m_mutex;
    QDateTime mNaviDateTime;

    DateTimeUtilInternal() {
    }
};

QDateTime DateTimeUtil::currentUtcDateTime()
{
#ifdef BUILD_TARGET
    return DateTimeUtilInternal::instance()->naviDateTime().toUTC();
#else
    return QDateTime::currentDateTimeUtc();
#endif
}

QDateTime DateTimeUtil::currentLocalDateTime()
{
    return toLocalDateTime(currentUtcDateTime());
}

QDateTime DateTimeUtil::toLocalDateTime(const QDateTime& dateTime)
{
    return dateTime.toTimeZone(QTimeZone(9 * 60 * 60));
}

void DateTimeUtil::setNaviDateTimeDetailInfo(NXDateTimeDetailInfo dateTimeDetailInfo)
{
    DateTimeUtilInternal::instance()->setNaviDateTime(dateTimeDetailInfo);
}
