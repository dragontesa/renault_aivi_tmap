#ifndef TDBUSABSTRACTINTERFACE_H
#define TDBUSABSTRACTINTERFACE_H

#include <QDBusAbstractInterface>

namespace SKT {

class TDBusAbstractInterface : public QDBusAbstractInterface
{
    Q_OBJECT
public:
    virtual ~TDBusAbstractInterface();

protected:
    TDBusAbstractInterface(const QString &service, const QString &path, const char *interface,
                           const QDBusConnection &connection, QObject *parent);


    void connectNotify(const QMetaMethod &signal) override;
    void disconnectNotify(const QMetaMethod &signal) override;

signals:
    void propertyChanged(const QString &propertyName, const QVariant &value);
    void propertyInvalidated(const QString &propertyName);


private slots:
    void onPropertiesChanged(const QString &interfaceName,
                             const QVariantMap &changedProperties,
                             const QStringList &invalidatedProperties);
private:
    static QVariant demarshall(const QMetaProperty &metaProperty,
                               const QVariant &value,
                               bool &demarshalled);

private:
    bool m_connected;

};

}

#endif // TDBUSABSTRACTINTERFACE_H
