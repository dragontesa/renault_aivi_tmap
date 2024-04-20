#include "cipherservice.h"
#include <QtConcurrent>
#include <QFutureWatcher>
#include "qaesencryption.h"

namespace {
    NSPrivateDataList s2p( QByteArray s ) {
        NSPrivateDataList ret;
        for ( auto c : s ) {
            NSPrivateData data;
            data.m_data = c;
            ret.append( data );
        }
        return ret;
    }

    QByteArray p2s( const NSPrivateDataList& pd ) {
        QByteArray ba;
        for ( auto c : pd ) {
            NSPrivateData data;
            data = c;
            ba.append( data.m_data );
        }
        return ba;
    }
}

namespace SKT {

CipherService::CipherService(ComBoschAutomotiveProxyPrivateDataEncryptionInterface* pde)
    : m_pde(pde)
{
    qDebug() << "[CipherService] max thread count: " << QThreadPool::globalInstance()->maxThreadCount();
}

void CipherService::encrypt(QByteArray s, std::function<void(QByteArray)> cb, bool waitForFinished)
{
    auto encryptData = [this](QByteArray s)->QByteArray{
        QByteArray ret;
        NSPrivateDataList ay = s2p(s);
        NSPrivateDataList ray;
        auto r = m_pde->EncryptData(0, ay, ray);
        if (!r.isValid()) {
            qDebug() << "[CipherService] PrivateDataEncryption Encrypt Error: " << r.error();
        }
        else {
            ret = p2s( ray );
        }
        return ret;
    };

    QFutureWatcher<QByteArray>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QByteArray>;
        connect(watcher, &QFutureWatcher<QByteArray>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(encryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void CipherService::decrypt(QByteArray s, std::function<void(QByteArray)> cb, bool waitForFinished)
{
    auto decryptData = [this](QByteArray s)->QByteArray{
        QByteArray ret;
        NSPrivateDataList dray;
        NSPrivateDataList ray = s2p(s);
        auto r = m_pde->DecryptData(0, ray, dray);
        if (!r.isValid()) {
            qDebug() << "[CipherService] PrivateDataEncryption Decrypt Error: " << r.error();
        }
        else {
            ret = p2s(ray);
        }
        return ret;
    };

    QFutureWatcher<QByteArray>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QByteArray>;
        connect(watcher, &QFutureWatcher<QByteArray>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(decryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void CipherService::encrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    auto encryptData = [this](QList<QByteArray> s)->QList<QByteArray>{
        QList<QByteArray> ret;
        for (auto i : s) {
            QByteArray ba;            
            NSPrivateDataList ay = s2p(i);
            NSPrivateDataList ray;
            auto r = m_pde->EncryptData(0, ay, ray);
            if (!r.isValid()) {
                qDebug() << "[CipherService] PrivateDataEncryption Decrypt Error: " << r.error();
            }
            else {
                ba = p2s( ray );
            }
            ret.append( ba );
        }
        return ret;
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;
    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(encryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void CipherService::decrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    auto decryptData = [this](QList<QByteArray> s)->QList<QByteArray>{
        QList<QByteArray> ret;
        for ( auto i : s ) {
            QByteArray ba;
            NSPrivateDataList ay = s2p(i);
            NSPrivateDataList ray;
            auto r = m_pde->DecryptData(0, ay, ray);
            if (!r.isValid()) {
                qDebug() << "[CipherService] PrivateDataEncryption Decrypt Error: " << r.error();
            }
            else {
                ba = p2s(ray);
            }
            ret.append( ba );
        }
        return ret;
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(decryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void CipherService::encrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    struct Internal {
        struct Mapped {
            typedef QByteArray result_type;
            Mapped(ComBoschAutomotiveProxyPrivateDataEncryptionInterface* pde) : pde( pde ){}
            QByteArray operator()(QByteArray s) {
                QByteArray ret;
                NSPrivateDataList ay = s2p(s);
                NSPrivateDataList ray;
                auto r = pde->EncryptData(0, ay, ray);
                if (!r.isValid()) {
                    qDebug() << "[CipherService] PrivateDataEncryption Decrypt Error: " << r.error();
                }
                else {
                    ret = p2s( ray );
                }
                return ret;
            }
            ComBoschAutomotiveProxyPrivateDataEncryptionInterface* pde;
        };

        static void Reduced(QList<QByteArray>& res, QByteArray s) {
            res.append(s);
        }
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::mappedReduced(s, Internal::Mapped(m_pde), Internal::Reduced,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce );

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void CipherService::decrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    struct Internal {
        struct Mapped {
            typedef QByteArray result_type;
            Mapped(ComBoschAutomotiveProxyPrivateDataEncryptionInterface* pde) : pde( pde ){}
            QByteArray operator()(QByteArray s) {
                QByteArray ret;
                NSPrivateDataList ay = s2p(s);
                NSPrivateDataList ray;
                auto r = pde->DecryptData(0, ay, ray);
                if (!r.isValid()) {
                    qDebug() << "[CipherService] PrivateDataEncryption Decrypt Error: " << r.error();
                }
                else {
                    ret = p2s(ray);
                }
                return ret;
            }
            ComBoschAutomotiveProxyPrivateDataEncryptionInterface* pde;
        };

        static void Reduced(QList<QByteArray>& res, QByteArray s) {
            res.append(s);
        }
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::mappedReduced(s, Internal::Mapped(m_pde), Internal::Reduced,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce );

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

AesCipherService::AesCipherService()
{
    QString key("m_hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256)");
    m_hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);

    qDebug() << "[AesCipherService] max thread count: " << QThreadPool::globalInstance()->maxThreadCount();
}

void AesCipherService::encrypt(QByteArray s, std::function<void(QByteArray)> cb, bool waitForFinished)
{
    auto encryptData = [this](const QString& s)->QByteArray{
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);
        return encryption.encode(s.toLocal8Bit(), m_hashKey);
    };

    QFutureWatcher<QByteArray>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QByteArray>;
        connect(watcher, &QFutureWatcher<QByteArray>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(encryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void AesCipherService::decrypt(QByteArray s, std::function<void(QByteArray)> cb, bool waitForFinished)
{
    auto decryptData = [this](QByteArray s)->QByteArray{
        QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);
        return encryption.removePadding(encryption.decode(s, m_hashKey));
    };

    QFutureWatcher<QByteArray>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QByteArray>;
        connect(watcher, &QFutureWatcher<QByteArray>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(decryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void AesCipherService::encrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    auto encryptData = [this](QList<QByteArray> s)->QList<QByteArray>{
        QList<QByteArray> ret;
        for ( auto i : s ) {
            QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);
            auto ba = encryption.encode(i, m_hashKey);
            ret.append(ba);
        }
        return ret;
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(encryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void AesCipherService::decrypt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    auto decryptData = [this](QList<QByteArray> s)->QList<QByteArray>{
        QList<QByteArray> ret;
        for (auto i : s) {
            QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);            
            auto ba = encryption.removePadding(encryption.decode(i, m_hashKey));
            ret.append(ba);
        }
        return ret;
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::run(decryptData, s);

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void AesCipherService::encrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{    
    struct Internal {
        struct Mapped {
            typedef QByteArray result_type;
            Mapped(QByteArray hashKey) : hashKey( hashKey ){}
            QByteArray operator()(QByteArray s) {
                QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);
                return encryption.encode(s, hashKey);
            }
            QByteArray hashKey;
        };

        static void Reduced(QList<QByteArray>& res, QByteArray s) {
            res.append(s);
        }
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::mappedReduced(s, Internal::Mapped(m_hashKey), Internal::Reduced,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce );

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

void AesCipherService::decrypt_mt(QList<QByteArray> s, std::function<void(QList<QByteArray>)> cb, bool waitForFinished)
{
    struct Internal {
        struct Mapped {
            typedef QByteArray result_type;
            Mapped(QByteArray hashKey) : hashKey( hashKey ){}
            QByteArray operator()(QByteArray s) {
                QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB, QAESEncryption::Padding::ISO);
                return encryption.removePadding(encryption.decode(s, hashKey));
            }
            QByteArray hashKey;
        };

        static void Reduced(QList<QByteArray>& res, QByteArray s) {
            res.append(s);
        }
    };

    QFutureWatcher<QList<QByteArray>>* watcher = nullptr;

    if (!waitForFinished) {
        watcher = new QFutureWatcher<QList<QByteArray>>;
        connect(watcher, &QFutureWatcher<QList<QByteArray>>::finished,
                [this, watcher, cb]() {
            auto future = watcher->future();
            watcher->deleteLater();
            auto res = future.result();
            cb(res);
        });
    }

    auto future = QtConcurrent::mappedReduced(s, Internal::Mapped(m_hashKey), Internal::Reduced,
        QtConcurrent::OrderedReduce | QtConcurrent::SequentialReduce );

    if (!waitForFinished) {
        watcher->setFuture(future);
    }
    else {
        cb( future.result() );
    }
}

}
