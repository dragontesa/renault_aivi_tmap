#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include "sessionmanagerinterface.h"

namespace SKT {

class SessionManager : public QObject, public SessionManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SessionManagerInterface)
public:
    SessionManager(QObject* parent = nullptr);
    virtual ~SessionManager();

    void init();

    QObject* qobject() override { return this; }

    // return current session key
    QByteArray sessionKey() const override;

    QByteArray sessionKey(int id) const override;


public slots:
    // request new session key
    void createSessionByOTP(const QString &otpCode) override;


    // clear current session
    void clearSession() override;

    // clear all the session
    void clearAllSession() override;

    // clear session by profile
    void clearSessionByProfileId(int id) override;

    // query session for profile and set as active
    void changeSessionByProfileId(int id);


signals:
    void sessionCreated(bool success, int validateCode, int errCode);
    void sessionChanged(int ownerId);
    void channelPolicyReceived(QString serviceItems, QString serviceName);

private:
    void openDatabase();
    void closeDatabase();
    void setSessionKeyLocked(const QByteArray &sessionKey);
    void storeSessionKeyLocked(int id, const QByteArray &sessionKey);
    QByteArray querySessionKeyLocked(int id) const;
    void deleteSessionKeyLocked(int id) const;

private slots:
    void handleNddsMessage(int msg, long lParam1, long lParam2, void *lpParam);

private:
    struct Session {
        Session() : profileId(0) {}
        int profileId;
        QByteArray sessionKey;
    };

    struct SessionReq {
        SessionReq(int id) : profileId(id) {}
        int profileId;
    };

    QSqlDatabase m_sessionDB;
    Session m_session;
    QScopedPointer<SessionReq> m_sessionReq;
    mutable QMutex m_lock;
};

}

#endif // SESSIONMANAGER_H
