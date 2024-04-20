#ifndef SESSIONMANAGERINTERFACE_H
#define SESSIONMANAGERINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QString>

namespace SKT {

class SessionManagerInterface
{
public:
    virtual ~SessionManagerInterface() {}

    /**
     * @brief qobject
     * @return
     */
    virtual QObject* qobject() = 0;

    // return current session key
    virtual QByteArray sessionKey() const = 0;

    virtual QByteArray sessionKey(int id) const = 0;

    // request new session key
    virtual void createSessionByOTP(const QString &otpCode) = 0;

    // clear current session
    virtual void clearSession() = 0;

    // clear all the session
    virtual void clearAllSession() = 0;


    virtual void clearSessionByProfileId(int id) = 0;


signals:
    // just hint....
    void sessionCreated(bool success, int validateCode, int errCode);
    void sessionChanged(int ownerId);
};

}

Q_DECLARE_INTERFACE(SKT::SessionManagerInterface, "com.skt.tmap.util.SessionManagerInterface/1.0")

#endif // SESSIONMANAGERINTERFACE_H
