#ifndef CIPHERSERVICE_H
#define CIPHERSERVICE_H

#include <functional>
#include <QObject>
#include <QByteArray>
#include <QString>
#include "cipherserviceinterface.h"
#include "privateDatainterface.h"

namespace SKT {

class CipherService : public QObject, public CipherServiceInterface
{
public:
    CipherService(ComBoschAutomotiveProxyPrivateDataEncryptionInterface* pde);
    virtual ~CipherService() {}

    void encrypt(QByteArray s, std::function<void(QByteArray)>cb, bool waitForFinished = false);
    void decrypt(QByteArray s, std::function<void(QByteArray)>cb, bool waitForFinished = false);

    void encrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);
    void decrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);

    void encrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);
    void decrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);

private:
    ComBoschAutomotiveProxyPrivateDataEncryptionInterface* m_pde;
};

class AesCipherService : public QObject, public CipherServiceInterface
{
public:
    AesCipherService();
    virtual ~AesCipherService() {}

    void encrypt(QByteArray s, std::function<void(QByteArray)>cb, bool waitForFinished = false);
    void decrypt(QByteArray s, std::function<void(QByteArray)>cb, bool waitForFinished = false);

    void encrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);
    void decrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);

    void encrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);
    void decrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)>cb, bool waitForFinished = false);

private:
    QByteArray m_hashKey;
};


}

#endif // CIPHERSERVICE_H
