#include <QDebug>
#include <QDir>
#include <QSqlQuery>
#include <QDateTime>
#include <QThread>
#include "sessionmanager.h"
#include "rpnsearch.h"
#include "environment.h"
#include "NDDS_Define.h"
#include "NDDS_Interface.h"

#define SESSIONMANAGER_DB_CONNECTION    "com.skt.tmap.sessionmanager"
#define SESSIONMANAGER_DB_NAME          "session.db"
#define SESSIONMANAGER_DB_VERSION       1

#define NDDS_VALIDATE_RESULT_CODE       2000

namespace SKT {

SessionManager::SessionManager(QObject* parent)
    : QObject(parent),
      m_sessionReq(nullptr)
{
    openDatabase();
}

SessionManager::~SessionManager()
{
    closeDatabase();
}

void SessionManager::init()
{
    RPnSearch* rp = RPnSearch::sharedInstance();
    connect(rp, &RPnSearch::nddsMessageReceived, this, &SessionManager::handleNddsMessage,
            Qt::DirectConnection);
}

void SessionManager::createSessionByOTP(const QString &otpCode)
{
    qDebug() << "[OTP] createSessionByOTP(" << otpCode << ")";

    if(otpCode.length() > 6) {
        QMetaObject::invokeMethod(this, "sessionCreated", Qt::QueuedConnection,
                                  Q_ARG(bool, false), Q_ARG(int, 0), Q_ARG(int, DE_ERROR_AUTH_OTPCODE_MAX));
        return;
    }

    QMutexLocker locker(&m_lock);
    bool success = false;

    DeErrorCode eErrorCode = DE_ERROR_UNKNOWN;
    if (!m_sessionReq) {
        m_sessionReq.reset(new SessionReq(m_session.profileId));

        eErrorCode = DE_ERROR_FAIL;
        if ((eErrorCode = NDDS_AuthTmapForSmartCar(otpCode.toLatin1().data(), false)) == DE_ERROR_SUCCESS) {
            success = true;
        } else {
            qWarning() << "NDDS_AuthTmapForSmartCar failed";
        }
    }

    if (!success) {
        QMetaObject::invokeMethod(this, "sessionCreated", Qt::QueuedConnection,
                                  Q_ARG(bool, success), Q_ARG(int, 0), Q_ARG(int, (int)eErrorCode));
    }

    qDebug() << "[OTP] createSessionByOTP(" << otpCode << ") -> " << success;
}

QByteArray SessionManager::sessionKey() const
{
//    QMutexLocker locker(&m_lock); // session manager alive in main thread with UI and this function is only used to be for UI

    return m_session.sessionKey;
}

QByteArray SessionManager::sessionKey(int id) const
{
    QMutexLocker locker(&m_lock);
    return querySessionKeyLocked(id);
}

void SessionManager::clearSession()
{
    qDebug() << "[OTP] clearSession";

    QMutexLocker locker(&m_lock);

    deleteSessionKeyLocked(m_session.profileId);
    setSessionKeyLocked(QByteArray());
}

void SessionManager::clearAllSession()
{
    qDebug() << "[OTP] clearAllSession";

    QMutexLocker locker(&m_lock);

    QSqlQuery query(m_sessionDB);
    query.exec(QLatin1String("delete from session"));

    setSessionKeyLocked(QByteArray());
}

void SessionManager::clearSessionByProfileId(int id)
{
    QMutexLocker locker(&m_lock);

    deleteSessionKeyLocked(id);
    if (m_session.profileId == id) {
        setSessionKeyLocked(QByteArray());
    }
}

void SessionManager::openDatabase()
{
    m_sessionDB = QSqlDatabase::addDatabase("QSQLITE", QLatin1String(SESSIONMANAGER_DB_CONNECTION));
    m_sessionDB.setDatabaseName(Environment::getDatabasePath() + QDir::separator() + QLatin1String(SESSIONMANAGER_DB_NAME));
    if (!m_sessionDB.open()) {
        return;
    }

    QSqlQuery query(m_sessionDB);
    query.exec(QLatin1String("PRAGMA user_version"));
    if (query.next()) {
        int version = query.value(0).toInt();
        if (version != SESSIONMANAGER_DB_VERSION) {
            // create or migration
            if (version != 0) {
                query.exec(QLatin1String("drop table session"));
            }

            if (!query.exec(QLatin1String("create table session(id integer primary key, session_key varchar, created_at datetime)"))) {
                qDebug() << "AuthManager: create table error";
            }

            QSqlQuery(QString("PRAGMA user_version=%1").arg(SESSIONMANAGER_DB_VERSION), m_sessionDB);
        }
    }
}

void SessionManager::closeDatabase()
{
    if (m_sessionDB.isOpen()) {
        m_sessionDB.close();
    }
    m_sessionDB = QSqlDatabase();
    QSqlDatabase::removeDatabase(QLatin1String(SESSIONMANAGER_DB_CONNECTION));
}

void SessionManager::setSessionKeyLocked(const QByteArray &sessionKey)
{
    if (sessionKey != m_session.sessionKey) {
        m_session.sessionKey = sessionKey;

        // 2019-4-26 donot authorize for guest,  Spec Frozen by Kimkangyoung
        if (m_session.profileId > 0) {
            if (NDDS_SetAuthAccessKey(const_cast<char*>(sessionKey.constData())) != DE_ERROR_SUCCESS) {
                qWarning() << "NDDS_SetAuthAccessKey failed";
            }
        }

        emit sessionChanged(m_session.profileId);
    }
}

void SessionManager::storeSessionKeyLocked(int id, const QByteArray &sessionKey)
{
    QSqlQuery query(m_sessionDB);
    query.prepare(QLatin1String("insert or replace into session(id, session_key, created_at) values(?, ?, datetime(?, 'unixepoch'))"));
    query.bindValue(0, id);
    query.bindValue(1, sessionKey);
    query.bindValue(2, QDateTime::currentMSecsSinceEpoch()/1000);
    query.exec();

    if (id == m_session.profileId) {
        setSessionKeyLocked(sessionKey);
    }
}

QByteArray SessionManager::querySessionKeyLocked(int id) const
{
    QSqlQuery query(m_sessionDB);
    query.prepare(QLatin1String("select session_key from session where id=?"));
    query.bindValue(0, id);
    query.exec();
    if (query.next()) {
        return query.value(0).toByteArray();
    }

    return QByteArray();
}

void SessionManager::deleteSessionKeyLocked(int id) const
{
    QSqlQuery query(m_sessionDB);
    query.prepare(QLatin1String("delete from session where id=?"));
    query.bindValue(0, id);
    query.exec();
}

void SessionManager::changeSessionByProfileId(int id)
{
    QMutexLocker locker(&m_lock);

    // don't stop the session change role 
    m_session.profileId = id;

    setSessionKeyLocked(querySessionKeyLocked(id));
}

// called in the ndds thread
void SessionManager::handleNddsMessage(int msg, long lParam1, long lParam2, void *lpParam)
{
    //Q_UNUSED(lParam1)
    //Q_UNUSED(lParam2)

    if (msg == DE_MSGTYPE_AUTH_END) {
        bool success = false;
        int validateCode = 0;
        const char* sessionKey = nullptr;

        DS_AUTHRESULT result;
        memset(&result, 0, sizeof(DS_AUTHRESULT));

        if (NDDS_GetAuthResult(&result) == DE_ERROR_SUCCESS) {
            success = true;
            validateCode = result.sAuthInfo.nValidateCode;
            sessionKey = result.szAccessKey;

#if 0  // --> ServerPolicyFetch 에서 처리  19.09.6
            // 채널 정책 처리
            // 19.08.02 ymhong
            for (auto i=0; i<result.sAppControlInfo.nControlInfoCount; i++) {
                auto info = result.sAppControlInfo.sControlInfoDetails[i];
                auto serviceName = QString::fromLatin1(info.szServiceName);
                auto use = QString::fromLatin1(info.szServiceYn) == "Y";
                auto serviceItem = QString::fromLatin1(info.szServiceItems);
                qDebug() << serviceName << ":" << use << "," << serviceItem;

                if (use) {
                    emit channelPolicyReceived(serviceItem, serviceName);
                }
            }
#endif
        }

//        QMutexLocker locker(&m_lock);

        success = success && m_sessionReq && validateCode == NDDS_VALIDATE_RESULT_CODE && !QString(sessionKey).isEmpty();
        if (success) {
            storeSessionKeyLocked(m_sessionReq->profileId, QByteArray(sessionKey));
        }

        // 2019-12-02 lckxx, 인증키 무효처리 된 경우, 초기화
        QString errcode((const char*)lpParam);
        if (qstrlen(sessionKey) == 0 || errcode == QStringLiteral("210501")) {
            qDebug() << "sm, ndds auth 210501 #1";
            // clearSession();  2020.3.2 fix TMCRSM-4612 
        }

        m_sessionReq.reset();

        int nErrCode = (int) lParam1;
        qDebug() << "[OTP] sessionCreated(success: " << success << ", validCode: " << validateCode << ", err: " << nErrCode << ")";
        emit sessionCreated(success, validateCode, nErrCode);
    } 
    else if (msg == DE_MSGTYPE_AUTH_CHANGE) {
       const char* szAccessKey = static_cast<const char*>(lpParam);
       if (szAccessKey) {
           if (m_sessionReq)
            storeSessionKeyLocked(m_sessionReq->profileId, QByteArray(szAccessKey));
       }
    }
    else if (msg == DE_MSGTYPE_AUTH_PROGRESS) {
        QString errcode((const char*)lpParam);
        if (errcode == QStringLiteral("210501")) { // expired access key
        qDebug() << "sm, ndds auth 210501 #2";
            clearSession();
        }
    }
}

}
