#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>

#include "packageutil.h"
#include "environment.h"

namespace SKT {
namespace PackageUtil {

QString mapPath()
{
    return Environment::getMapDataPath();
}

QString mapBackupPath()
{
    return Environment::getMapDataPath() + QLatin1String(".bak");
}

QString mapUpdatePath()
{
    return Environment::getMapUpdatePath();
}

QString mapUpdatePartition()
{
    return Environment::getMapUpdatePartition();
}

QString mediaPermitBindPath(const QString& mediaPath)
{
    #if 0
    QString mediaName = mediaPath.mid(mediaPath.lastIndexOf(QChar('/'))+1);
    QString boundPath = QStringLiteral("/var/lib/lxc/extnav/rootfs/media/%1").arg(mediaName);
    #else  // the path in lxc fs should be offered by system
    QString boundPath = QStringLiteral("/var/lib/lxc/extnav/rootfs/media/usb_dev");
    #endif
    return boundPath;
}

QString mediaLxcBoundPath()
{
    return QStringLiteral("/media/usb_dev");
}

bool hasMediaReadable(const QString& mediaPath)
{
    bool readable = false;
    // QString packageFilePath = mediaPath + QDir::separator() + QLatin1String(PACKAGE_CONTAINER);
    QFileInfo fi(mediaPath);
    return fi.isReadable();
}

bool hasPermitMedia(const QString& mediaPath)
{
    qDebug() << "pacman permit media path: " << mediaPath;
    if (!hasMediaReadable(mediaPath)) {
        qDebug() << "pacman the bound path is not readable";
        return false;
    }
    return true;
}


bool verifySignature(const QByteArray &messageDigest, const QByteArray &signature, RSAPublicKey *key)
{
    if (!key) {
        return false;
    }

    RSA* rsaKey = static_cast<RSA*>(key->key());
    if (!rsaKey) {
        return false;
    }

    if (RSA_verify(NID_sha256,
                   (unsigned char*)messageDigest.data(),
                   messageDigest.count(),
                   (unsigned char*)signature.data(),
                   signature.count(),
                   rsaKey)) {
        return true;
    }

    return false;
}

}

RSAPublicKey::RSAPublicKey(const char* data, qint64 dataLen)
{
    m_key = 0;

    BIO *bp = BIO_new_mem_buf((void*)data, dataLen);
    if (bp) {
        m_key = PEM_read_bio_RSA_PUBKEY(bp, NULL, 0, NULL);
        BIO_free(bp);
    } else {
        qWarning() << "failed to read RSA public key!";
    }
}

RSAPublicKey::~RSAPublicKey()
{
    RSA* key = static_cast<RSA*>(m_key);
    if (key) {
        RSA_free(key);
    }
}

MessageDigestSHA256::MessageDigestSHA256() : m_context(0)
{
    EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
    if (mdctx) {
        if (EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL) == 1) {
            m_context = mdctx;
        } else {
            EVP_MD_CTX_destroy(mdctx);
        }
    }
}

MessageDigestSHA256::~MessageDigestSHA256()
{
    EVP_MD_CTX* mdctx = static_cast<EVP_MD_CTX*>(m_context);
    if (mdctx) {
        EVP_MD_CTX_destroy(mdctx);
    }
}

bool MessageDigestSHA256::update(const char *data, qint64 dataLen)
{
    EVP_MD_CTX* mdctx = static_cast<EVP_MD_CTX*>(m_context);
    if (mdctx) {
        return EVP_DigestUpdate(mdctx, data, dataLen) == 1;
    }
    return false;
}

QByteArray MessageDigestSHA256::final(bool *ok)
{
    EVP_MD_CTX* mdctx = static_cast<EVP_MD_CTX*>(m_context);
    if (mdctx) {
        unsigned char md_value[EVP_MAX_MD_SIZE];
        unsigned int md_len;

        if (EVP_DigestFinal_ex(static_cast<EVP_MD_CTX*>(m_context), md_value, &md_len) == 1) {
            if (ok) *ok = true;
            return QByteArray((char*)md_value, md_len);
        }
    }
    if (ok) *ok = false;
    return QByteArray();
}


}
