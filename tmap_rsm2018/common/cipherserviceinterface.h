#ifndef CIPHERSERVICEINTERFACE_H
#define CIPHERSERVICEINTERFACE_H

#include <functional>
#include <QObject>
#include <QByteArray>
#include <QString>
#include <QVariant>

namespace SKT {

class CipherServiceInterface
{
public:
    virtual ~CipherServiceInterface() {}

    virtual void encrypt(QByteArray s, std::function<void(QByteArray)> cb, bool waitForFinished = false) = 0;
    virtual void decrypt(QByteArray s, std::function<void(QByteArray)> cb, bool waitForFinished = false) = 0;

    virtual void encrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished = false) = 0;
    virtual void decrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished = false) = 0;

    virtual void encrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished = false) = 0;
    virtual void decrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished = false) = 0;
};

}

Q_DECLARE_INTERFACE(SKT::CipherServiceInterface, "com.skt.tmap.util.CipherServiceInterface/1.0")

#endif // CIPHERSERVICEINTERFACE_H
