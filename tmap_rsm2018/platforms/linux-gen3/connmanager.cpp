#ifndef BUILD_APP

#include <QtDBus>
#include <QDebug>
#include "connmanager.h"

#define CONNMAN_SERVICE_NAME        "net.connman"
#define CONNMAN_OBJECT_PATH         "/"
#define CONNMAN_INTERFACE_MANAGER           "net.connman.Manager"
#define CONNMAN_INTERFACE_NOTIFICATION      "net.connman.Notification"
#define CONNMAN_INTERFACE_Session           "net.connman.Session"

namespace SKT {

typedef	QMap<QString, QDBusVariant> ConnmanProperties;

}

Q_DECLARE_METATYPE(SKT::ConnmanProperties)


namespace SKT {

ConnManager::ConnManager(QObject *parent)
    : QObject(parent)
    , m_state(State::Offline)
{
    qDBusRegisterMetaType<SKT::ConnmanProperties>();
}

void ConnManager::init()
{
    qWarning() << "ConnManager::init";
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.isConnected()) {
        qWarning() << "ConnManager::init_sessionBus bus is not connected";
        return;
    }

    if (conn.interface()->isServiceRegistered(CONNMAN_SERVICE_NAME)) {
        onServiceRegistered();
    } else {
        qDebug() << "Waiting for service:" << CONNMAN_SERVICE_NAME;
        QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(CONNMAN_SERVICE_NAME,
                conn, QDBusServiceWatcher::WatchForRegistration, this);
        connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onServiceRegistered()));
    }
}

void ConnManager::onGetProperties(QDBusPendingCallWatcher* call)
{
    QDBusPendingReply<ConnmanProperties> reply = *call;
    if (!reply.isError()) {
        ConnmanProperties propertyMap = reply.value();
        QString state = propertyMap["State"].variant().toString();
        setState(state);
    }
    call->deleteLater();
}

void ConnManager::onPropertyChanged(const QString &name, const QDBusVariant &value)
{
    if (name == QLatin1String("State")) {
        setState(value.variant().toString());
    }
}

void ConnManager::onServiceRegistered()
{
    qDebug() << CONNMAN_SERVICE_NAME << "is registered";
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.isConnected()) {
        qWarning() << "onServiceRegistered_sessionBus is not connected";
        return;
    }

    QDBusInterface *iface = new QDBusInterface(CONNMAN_SERVICE_NAME,
                                               CONNMAN_OBJECT_PATH,
                                               CONNMAN_INTERFACE_MANAGER,
                                               conn,
                                               this);
    if (!iface->isValid()) {
        qWarning() << CONNMAN_SERVICE_NAME << "is not valid.";
        return;
    } else {
        qWarning() << CONNMAN_SERVICE_NAME << "is valid.";
    }

    // get properties to obtain current state
    QList<QVariant> argumentList;
    QDBusPendingCall async = iface->asyncCallWithArgumentList(QLatin1String("GetProperties"),
                                                              argumentList);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(onGetProperties(QDBusPendingCallWatcher*)));

    // monitor state change
    bool ok = conn.connect(CONNMAN_SERVICE_NAME,
                 CONNMAN_OBJECT_PATH,
                 CONNMAN_INTERFACE_MANAGER,
                 "PropertyChanged",
                 "sv",
                 this,
                 SLOT(onPropertyChanged(QString,QDBusVariant)));
    if (!ok)
        qDebug() << "connman failed to connect .Manager service";
}

void ConnManager::setState(const QString &state)
{
    State s = State::Offline;

    if (state == QLatin1String("offline"))
        s = State::Offline;
    else if (state == QLatin1String("idle"))
        s = State::Idle;
    else if (state == QLatin1String("ready"))
        s = State::Ready;
    else if (state == QLatin1String("online"))
        s = State::Online;

    setState(s);
}

void ConnManager::setState(State state)
{
    qDebug() << "ConnManager: setState" << state;

    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(state);
}

}


#endif