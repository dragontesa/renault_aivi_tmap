#ifndef PRIVATEDATAENCRYPTION_H
#define PRIVATEDATAENCRYPTION_H

#include <QDBusArgument>

/**
 * @brief The NSPrivateData struct
 */
struct NSPrivateData {
    uchar m_data;
};

QDBusArgument &operator<<(QDBusArgument &argument, const NSPrivateData& data);
const QDBusArgument &operator>>(const QDBusArgument &argument, NSPrivateData &data);

typedef QList<NSPrivateData> NSPrivateDataList;

Q_DECLARE_METATYPE(NSPrivateData)
Q_DECLARE_METATYPE(NSPrivateDataList)

/**
 * @brief The NavigationServiceType class
*/
class DataEncrypttype
{
public:
    static void registerMetaTypes();
};

#endif // NAVIGATIONSERVICETYPE_H
