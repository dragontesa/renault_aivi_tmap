#ifdef TMAP_CONNMAN_PROXY_API // Linux Container Stuff
#include "connmanager_lxc.h"
#include <QtDBus>
#include <QDebug>
#include <QMap>
#include <QList>

#define CONNMAN_SERVICE_NAME        "net.connman"
#define CONNMAN_OBJECT_PATH         "/"
#define CONNMAN_IF_MANAGER           "net.connman.Manager"
#define CONNMAN_IF_NOTIFICATION      "net.connman.Notification"
#define CONNMAN_IF_SESSION           "net.connman.Session"
#define CONNMAN_IF_NOTIFICATION_OBJ_PATH  "/connman/tmap/notification"


namespace SKT {
typedef QMap<QString, QDBusVariant> ConnmanDict;
struct ConnmanServiceInfo {
    QDBusObjectPath objectPath;
    ConnmanDict info;
};
typedef QList<ConnmanServiceInfo> ConnmanServiceInfoList;
}

Q_DECLARE_METATYPE(SKT::ConnmanDict)
Q_DECLARE_METATYPE(SKT::ConnmanServiceInfo)
Q_DECLARE_METATYPE(SKT::ConnmanServiceInfoList)


QDBusArgument &operator<<(QDBusArgument& arg, const SKT::ConnmanDict& dic)
{
    arg.beginMap( QVariant::Int, qMetaTypeId<SKT::ConnmanDict>() );
    QMapIterator<QString,QDBusVariant> it(dic);
    while (it.hasNext()) {
        it.next();
      arg.beginMapEntry();
      arg << it.key() << it.value();
      arg.endMapEntry();
    }
    arg.endMap();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument& arg, SKT::ConnmanDict& dic)
{
    arg.beginMap();
    dic.clear();
    while ( !arg.atEnd() ) {
      QString key;
      QDBusVariant value;
      arg.beginMapEntry();
      arg >> key >> value;
      arg.endMapEntry();
      dic.insert( key, value );
  }
  arg.endMap();
  return arg;
}

QDBusArgument &operator<<(QDBusArgument& arg, const SKT::ConnmanServiceInfo& si)
{
    arg.beginStructure();
    arg << si.objectPath << si.info;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument& arg, SKT::ConnmanServiceInfo& si)
{
    arg.beginStructure();
    arg >> si.objectPath >> si.info;
    arg.endStructure();
    return arg;
}


QDBusArgument &operator<<(QDBusArgument &arg, const SKT::ConnmanServiceInfoList &siList)
{
    arg.beginArray( qMetaTypeId<SKT::ConnmanServiceInfoList>() );
    for ( int i = 0; i < siList.size(); ++i )
        arg << siList.at(i);
    arg.endArray();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, SKT::ConnmanServiceInfoList &siList)
{
  arg.beginArray();
  siList.clear();

  while ( !arg.atEnd() ) {
      SKT::ConnmanServiceInfo si;
      arg >> si;
      siList.append( si );
  }

  arg.endArray();
  return arg;
}

namespace SKT {
ConnManager::ConnManager(QObject *parent)
: QObject(parent)
, m_state(State::Offline)
{
    qDBusRegisterMetaType<SKT::ConnmanDict>();
    qDBusRegisterMetaType<SKT::ConnmanServiceInfo>();
    qDBusRegisterMetaType<SKT::ConnmanServiceInfoList>();
}

void ConnManager::init()
{
	qWarning() << CONNMAN_SERVICE_NAME << ": ConnManager init";
	QDBusConnection conn = QDBusConnection::sessionBus();
	if (!conn.isConnected()) {
		qDebug() << CONNMAN_SERVICE_NAME << ": failed to connect session bus ";
		return;
	}

	if (conn.interface()->isServiceRegistered(CONNMAN_SERVICE_NAME)) {
		onConnmanServiceRegistered();
	} else {
		qDebug() << "Waiting for service:" << CONNMAN_SERVICE_NAME;
		QDBusServiceWatcher *serviceWatcher = new QDBusServiceWatcher(CONNMAN_SERVICE_NAME,
			conn, QDBusServiceWatcher::WatchForRegistration, this);
		connect(serviceWatcher, SIGNAL(serviceRegistered(QString)), this, SLOT(onConnmanServiceRegistered()));
	}
}


void ConnManager::onConnmanServiceRegistered()
{
  qDebug() << CONNMAN_SERVICE_NAME << ": service is registered";
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.isConnected()) {
        qWarning() << CONNMAN_SERVICE_NAME << ": service is not connected";
        return;
    }

    QDBusInterface *manager = new QDBusInterface(CONNMAN_SERVICE_NAME,
                                               CONNMAN_OBJECT_PATH,
                                               CONNMAN_IF_MANAGER,
                                               conn,
                                               this);
    if (!manager->isValid()) {
        qWarning() << CONNMAN_SERVICE_NAME << "is not valid.";
        return;
    } else {
        qWarning() << CONNMAN_SERVICE_NAME << "is valid.";
    }

     // setup notification interface
    QDBusInterface* notifier = new QDBusInterface(CONNMAN_SERVICE_NAME,
    											  CONNMAN_IF_NOTIFICATION_OBJ_PATH,
    											  CONNMAN_IF_NOTIFICATION,
    											  conn,
    											  this);

    // connect to notificiation's update 
    bool ok = conn.connect(CONNMAN_SERVICE_NAME,
    						CONNMAN_OBJECT_PATH,
    						CONNMAN_IF_NOTIFICATION,
    						"Update",
    						QLatin1String("a{sv}"),
    						this,
    						SLOT(onUpdate(const QDBusVariant&))
    						);
    if (!ok)
        qDebug() << CONNMAN_SERVICE_NAME << ": failed to connect notification";


     // setup session interface
    QVariantMap settings;
    settings[QLatin1String("Service")] = "";
    settings[QLatin1String("Conn")] = conn.name();
    QDBusPendingCall call = manager->asyncCall(QLatin1String("CreateSession"),
                                            QVariant::fromValue(settings),
                                            QVariant(CONNMAN_IF_NOTIFICATION_OBJ_PATH)
                                            );
    // QVariantList args;
    // QDBusPendingCall call = manager->asyncCallWithArgumentList(
    //     QLatin1String("CreateSession"),
    //     args
    //     );
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call,this);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
        this, SLOT(onCreateSession(QDBusPendingCallWatcher*)));


    // monitor state change
    ok = conn.connect(CONNMAN_SERVICE_NAME,
                 CONNMAN_OBJECT_PATH,
                 CONNMAN_IF_MANAGER,
                 "ServiceChanged",
                 QLatin1String("a(oa{sv})"),
                 this,
                 SLOT(onServiceChanged(const QDBusVariant&)));
    if (!ok)
        qWarning() << CONNMAN_SERVICE_NAME << ": failed to connect .Manager service";
}


void ConnManager::onCreateSession(QDBusPendingCallWatcher* call)
{
    qDebug() << CONNMAN_SERVICE_NAME << ": session created #1";
    #if 0
    QDBusPendingReply<ConnmanDict, QString> reply = *call;
    if (!reply.isError()) {
        QString sessionPath = reply.argumentAt<1>();
        // QString sessionPath  = reply.value().path();
        qDebug() << CONNMAN_SERVICE_NAME << ": success to reply !";
        qWarning() << QStringLiteral("Connman session created at: ") << sessionPath;
    }
    else {
        qDebug() << CONNMAN_SERVICE_NAME << ": failed to get reply";
        qDebug() << CONNMAN_SERVICE_NAME << ": " << reply.error().message();
        qDebug() << CONNMAN_SERVICE_NAME << ": " << reply.argumentAt(0).typeName();
        qDebug() << CONNMAN_SERVICE_NAME << ": " << reply.argumentAt(1).typeName();
    }
    #endif
    call->deleteLater();
}

void ConnManager::onServiceChanged(const QDBusVariant& service)
{
    QDBusArgument arg;
    arg << service;
    ConnmanServiceInfoList si =  qdbus_cast<ConnmanServiceInfoList>(arg);
    qDebug() << CONNMAN_SERVICE_NAME << ": sevice changed to " << si.at(0).objectPath.path();
    // todo something 
}

void ConnManager::onUpdate(const QDBusVariant& state)
{
    qDebug() << CONNMAN_SERVICE_NAME << ": noti updated !";
    QVariant v = state.variant();
    QVariantList list = v.toList();
    if (list.size() > 0) {
        QVariantMap m = list.at(0).toMap();
        if (m.contains("State")) {
            QString s = m["State"].toString();
            updateState(s);
        }
        else {
            qDebug() << CONNMAN_SERVICE_NAME << ": invalid state info";
        }
    } else {
        qWarning() << CONNMAN_SERVICE_NAME << ": unkown notification message !";
    }
}


void ConnManager::updateState(const QString &state)
{
    qDebug() << CONNMAN_SERVICE_NAME << ": update state: " << state;
   State s = State::Offline;

    if (state == QLatin1String("offline"))
        s = State::Offline;
    else if (state == QLatin1String("idle"))
        s = State::Idle;
    else if (state == QLatin1String("ready"))
        s = State::Ready;
    else if (state == QLatin1String("online") ||
        state == QLatin1String("connected"))
        s = State::Online;

    setState(s);

}

void ConnManager::setState(State state)
{
    qDebug() << CONNMAN_SERVICE_NAME << ": set state:" << state;

    if (m_state == state)
        return;

    qDebug() << "Connman state changed!";
    m_state = state;
    emit stateChanged(state);
}

}

#endif
