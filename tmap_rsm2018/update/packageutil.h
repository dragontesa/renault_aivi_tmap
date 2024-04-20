#ifndef UPDATEUTIL_H
#define UPDATEUTIL_H

#include <QByteArray>
#include <QString>

#define PACKAGE_CONTAINER               "package.json"
#define PACKAGE_FILEPATH_USB            "tmap"
#define PACKAGE_MARK_FILENAME           ".done"

namespace SKT {

class RSAPublicKey
{
public:
    RSAPublicKey(const char* data, qint64 dataLen);
    ~RSAPublicKey();
    void* key() { return m_key; }

private:
    void* m_key;
};

class MessageDigestSHA256
{
public:
    MessageDigestSHA256();
    ~MessageDigestSHA256();
    bool update(const char* data, qint64 dataLen);
    QByteArray final(bool *ok = nullptr);

private:
    void* m_context;
};

namespace PackageUtil {

QString mapPath();
QString mapBackupPath();
QString mapUpdatePath();
QString mapUpdatePartition();
QString mediaPermitBindPath(const QString& mediaName);
QString mediaLxcBoundPath();
bool    hasMediaReadable(const QString& mediaPath);
bool    hasPermitMedia(const QString& mediaPath);
bool    tryMediaMountPermitted( const QString& mediaPath);


bool verifySignature(const QByteArray &messageDigest, const QByteArray &signature, RSAPublicKey* key);

}

}

#endif // UPDATEUTIL_H
