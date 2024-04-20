#include <QMetaType>
#include <QDBusMetaType>
#include "navigationdataupdateservicetype.h"

void NavigationDataUpdateServiceType::registerMetaTypes()
{
    qDBusRegisterMetaType<NavigationDataUpdateStatusInfo>();
    qDBusRegisterMetaType<NavigationDataDeleteStatusInfo>();
}


QDBusArgument &operator<<(QDBusArgument &argument, const NavigationDataUpdateStatusInfo& info)
{
    argument.beginStructure();
    argument << info.m_status;
    argument << info.m_progressValue;
    argument << info.m_remainingTimeInSecond;
    argument << info.m_productName;
    argument << info.m_regionName;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NavigationDataUpdateStatusInfo& info)
{
    argument.beginStructure();
    argument >> info.m_status;
    argument >> info.m_progressValue;
    argument >> info.m_remainingTimeInSecond;
    argument >> info.m_productName;
    argument >> info.m_regionName;
    argument.endStructure();
    return argument;
}


QDBusArgument &operator<<(QDBusArgument &argument, const NavigationDataDeleteStatusInfo& info)
{
    argument.beginStructure();
    argument << info.m_status;
    argument << info.m_progressValue;
    argument << info.m_remainingTimeInSecond;
    argument << info.m_productName;
    argument << info.m_regionName;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NavigationDataDeleteStatusInfo& info)
{
    argument.beginStructure();
    argument >> info.m_status;
    argument >> info.m_progressValue;
    argument >> info.m_remainingTimeInSecond;
    argument >> info.m_productName;
    argument >> info.m_regionName;
    argument.endStructure();
    return argument;
}
