#include "tdbusabstractinterface.h"

#include <QMetaMethod>
#include <QDBusMetaType>
#include <QDebug>

namespace SKT {

Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, dBusPropertiesInterface, ("org.freedesktop.DBus.Properties"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, dBusPropertiesChangedSignal, ("PropertiesChanged"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, propertyChangedSignature, ("propertyChanged(QString,QVariant)"))
Q_GLOBAL_STATIC_WITH_ARGS(QByteArray, propertyInvalidatedSignature, ("propertyInvalidated(QString)"))

TDBusAbstractInterface::TDBusAbstractInterface(const QString &service,
                                               const QString &path,
                                               const char *interface,
                                               const QDBusConnection &connection,
                                               QObject *parent)
    : QDBusAbstractInterface(service, path, interface, connection, parent)
    , m_connected(false)
{

}

TDBusAbstractInterface::~TDBusAbstractInterface()
{

}

void TDBusAbstractInterface::connectNotify(const QMetaMethod &signal)
{
    if (signal.methodType() == QMetaMethod::Signal &&
            (signal.methodSignature() == *propertyChangedSignature() ||
             signal.methodSignature() == *propertyInvalidatedSignature())) {
        if (!m_connected) {
            QStringList argumentMatch;
            argumentMatch << interface();

            QDBusConnection conn = connection();
            conn.connect(service(),
                         path(),
                         *dBusPropertiesInterface(),
                         *dBusPropertiesChangedSignal(),
                         argumentMatch,
                         QString(),
                         this,
                         SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

            m_connected = true;
        }
        return;
    }

    QDBusAbstractInterface::connectNotify(signal);
}

void TDBusAbstractInterface::disconnectNotify(const QMetaMethod &signal)
{
    if (signal.methodType() == QMetaMethod::Signal &&
            (signal.methodSignature() == *propertyChangedSignature() ||
             signal.methodSignature() == *propertyInvalidatedSignature())) {
        if (m_connected) {
            if (receivers(propertyChangedSignature()->constData()) == 0) {
                QStringList argumentMatch;
                argumentMatch << interface();

                QDBusConnection conn = connection();
                conn.disconnect(service(),
                                path(),
                                *dBusPropertiesInterface(),
                                *dBusPropertiesChangedSignal(),
                                argumentMatch,
                                QString(),
                                this,
                                SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

                m_connected = false;
            }
        }
        return;
    }

    QDBusAbstractInterface::disconnectNotify(signal);
}

// TODO: Frequently call?
void TDBusAbstractInterface::onPropertiesChanged(const QString &interfaceName,
                                                 const QVariantMap &changedProperties,
                                                 const QStringList &invalidatedProperties)
{
    if (interfaceName == interface()) {
        QVariantMap::const_iterator i = changedProperties.constBegin();
        while (i != changedProperties.constEnd()) {
            int propertyIndex = metaObject()->indexOfProperty(i.key().toLatin1().constData());

            if (-1 == propertyIndex) {
                qDebug() << "Got unknown changed property" <<  i.key();
            } else {
                bool demarshlled = false;
                QVariant value = demarshall(metaObject()->property(propertyIndex), i.value(), demarshlled);

                if (demarshlled) {
                    emit propertyChanged(i.key(), value);
                } else {
                    emit propertyInvalidated(i.key());
                }
            }

            ++i;
        }

        QStringList::const_iterator j = invalidatedProperties.constBegin();
        while (j != invalidatedProperties.constEnd()) {
            if (-1 == metaObject()->indexOfProperty(j->toLatin1().constData())) {
                qDebug() << "Got unknown invalidated property" <<  *j;
            } else {
                emit propertyInvalidated(*j);
            }

            ++j;
        }
    }
}

QVariant TDBusAbstractInterface::demarshall(const QMetaProperty &metaProperty,
                                            const QVariant &value,
                                            bool &demarshalled)
{
    demarshalled = false;

    if (value.userType() == metaProperty.userType()) {
        demarshalled = true;
        return value;
    }

    if (value.userType() == qMetaTypeId<QDBusArgument>()) {
        if (metaProperty.isValid()) {
            QVariant result = QVariant(metaProperty.userType(), (void*)0);
            const char *expectedSignature = QDBusMetaType::typeToSignature(metaProperty.userType());
            QDBusArgument dbusArg = value.value<QDBusArgument>();

            if (expectedSignature == dbusArg.currentSignature().toLatin1()) {
                QDBusMetaType::demarshall(dbusArg, metaProperty.userType(), result.data());
                if (result.isValid()) {
                    demarshalled = true;
                }
                return result;
            }
        }
    }

#if 0
    const char * propSig = QDBusMetaType::typeToSignature( metaProperty.userType() );
    const char * valueSig = QDBusMetaType::typeToSignature( value.userType() );

    qDebug() << Q_FUNC_INFO;
    qDebug() << "value.userType():" << value.userType() << valueSig;
    qDebug() << "metaProperty.userType():" << metaProperty.userType() << propSig;
    qDebug() << "qMetaTypeId<QDBusArgument>():" << qMetaTypeId<QDBusArgument>();
#endif

    return QVariant();
}

}
