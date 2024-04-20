#include <QMetaType>
#include <QDBusMetaType>
#include "lifecyclemanagertype.h"

void LifeCycleManagerType::registerMetaTypes()
{
//    qDBusRegisterMetaType<LCMCredential>();
    qDBusRegisterMetaType<LCMCredentials>();
}

//QDBusArgument &operator<<(QDBusArgument &argument, const LCMCredential& data)
//{
//    argument.beginStructure();
//    argument << data.label;
//    argument << data.nameOfContainer;
//    argument.endStructure();

//    return argument;
//}

//const QDBusArgument &operator>>(const QDBusArgument &argument, LCMCredential& data)
//{
//    argument.beginStructure();
//    argument >> data.label;
//    argument >> data.nameOfContainer;
//    argument.endStructure();

//    return argument;
//}

