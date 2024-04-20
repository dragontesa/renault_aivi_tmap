#ifndef CONNMANAGER_H
#define CONNMANAGER_H

#ifndef BUILD_APP

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
    void onGetProperties(QDBusPendingCallWatcher*);
    void onPropertyChanged(const QString &name, const QDBusVariant &value);
    void onServiceRegistered();

signals:
    void stateChanged(ConnManager::State state);

private:
    void setState(const QString &state);
    void setState(State state);

private:
    State m_state;
};

}

#endif
#endif // CONNMANAGER_H
