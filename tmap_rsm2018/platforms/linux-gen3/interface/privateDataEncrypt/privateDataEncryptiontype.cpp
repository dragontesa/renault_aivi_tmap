#include <QMetaType>
#include <QDBusMetaType>
#include "privateDataEncryptiontype.h"

void DataEncrypttype::registerMetaTypes()
{
    qDBusRegisterMetaType<NSPrivateData>();
    qDBusRegisterMetaType<NSPrivateDataList>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const NSPrivateData& data)
{
    argument.beginStructure();
    argument << data.m_data;
    argument.endStructure();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NSPrivateData &data)
{
    argument.beginStructure();
    argument >> data.m_data;
    argument.endStructure();

    return argument;
}
