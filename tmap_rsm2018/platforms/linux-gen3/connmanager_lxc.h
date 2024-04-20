#ifndef TMAP_RSM2018_PLATFORMS_LINUX_GEN3_CONNMANAGER_LXC_H_
#define TMAP_RSM2018_PLATFORMS_LINUX_GEN3_CONNMANAGER_LXC_H_

// notice!
// bosch's ConnManProxy API for the ID DBus inside containers could not support longer,
// instead of that, bosch tell us that we can use CSManager to interaction with the ConnMan on native system.
// this class is implementation for ConnManProxy API
//
#ifdef TMAP_CONNMAN_PROXY_API
#include <QObject>

class QDBusPendingCallWatcher;
class QDBusVariant;

namespace SKT {

class ConnManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state NOTIFY stateChanged)

public:
    enum State {
        Offline = 0,
        Idle,
        Ready,
        Online
    };

    ConnManager(QObject *parent = 0);

    void init();
    State state() const { return m_state; }

private slots:
    void onConnmanServiceRegistered();
    void onServiceChanged(const QDBusVariant& service);
    void onCreateSession(QDBusPendingCallWatcher* call);
    void onUpdate(const QDBusVariant &value);

signals:
    void stateChanged(ConnManager::State state);

private:
    void updateState(const QString &state);
    void setState(State state);

private:
    State m_state;
};

}

#endif

#endif // TMAP_RSM2018_PLATFORMS_LINUX_GEN3_CONNMANAGER_LXC_H_
