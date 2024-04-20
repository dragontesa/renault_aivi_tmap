#ifndef LIFECYCLEMANAGERTYPE_H
#define LIFECYCLEMANAGERTYPE_H

#include <QDBusArgument>

#define LIFECYCLEMANAGER_PATH   "/com/bosch/AutomotiveProxy/LifeCycleManager"

/**
 * @brief The LCMShutdownType
 */
enum LCMShutdownType {
    LCMShutdownType_RUNUP = 0x80000000U,
    LCMShutdownType_NORMAL = 0x00000001U
};

/*
/**
 * @brief The LCMCredential struct
 *
struct LCMCredential {
    QString         label;
    QVariant    nameOfContainer;
};

QDBusArgument &operator<<(QDBusArgument &argument, const LCMCredential& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, LCMCredential& data);

typedef QList<LCMCredential> LCMCredentials;

Q_DECLARE_METATYPE(LCMCredential)
Q_DECLARE_METATYPE(LCMCredentials)
*/

typedef QMap<QString, QDBusVariant> LCMCredentials;
Q_DECLARE_METATYPE(LCMCredentials)

/**
 * @brief The NavigationServiceType class
*/
class LifeCycleManagerType
{
public:
    static void registerMetaTypes();
};

#endif // LIFECYCLEMANAGERTYPE_H
