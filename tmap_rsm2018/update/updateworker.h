#ifndef UPDATEWORKER_H
#define UPDATEWORKER_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QElapsedTimer>
#include <QSharedPointer>

#include <atomic>

#include "packageinfo.h"

namespace SKT {

struct PackageContainer;
class RSAPublicKey;
class PlatformController;

class UpdateWorker : public QObject
{
    Q_OBJECT
public:
    enum UpdateError {
        UpdateError_OK = 0,
        UpdateError_ReadError = -1,
        UpdateError_WriteError = -2,
        UpdateError_IntegrityError = -3,
        UpdateError_MountError = -4,
        UpdateError_Aborted = -10
    };

    UpdateWorker(int id,
                 PlatformController* platformController,
                 PackageContainerPtr pkgContainer,
                 const QString &packagePath,
                 QSharedPointer<RSAPublicKey> publicKey,
                 QObject* parent = 0);

public slots:
    void doNext();
    void abort();

signals:
    void updateProgress(int id, quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount);
    void updateError(int id, SKT::UpdateWorker::UpdateError errorType);
    void updateSuccess(int id);

private slots:
    void onRemountStatus(bool success);

private:
    void replaceDestinationTag(QString &str);

    void prepareProgress();
    void reportProgress(bool force=false);

    UpdateError cleanUp();
    UpdateError copyPackageDescriptor();
    UpdateError copyAndVerifyPackage();
    UpdateError markDone();

    UpdateError verify(const QString &filePath, const QByteArray &signature);
    UpdateError safeCopy(const QString &src, const QString &dest, int fileSize);

private:
    const int m_id;
    PlatformController* m_platformController;
    PackageContainerPtr m_pkgContainer;
    QString m_packagePath;
    QSharedPointer<RSAPublicKey> m_publicKey;
    std::atomic_bool m_abort;
    UpdateError m_error;

    QElapsedTimer m_progressTimer;
    struct ProgressData {
        ProgressData() : count(0), totalCount(0), storedSize(0), totalSize(0) {}
        quint32 count;
        quint32 totalCount;
        quint64 storedSize;
        quint64 totalSize;
    } m_progress;

    int m_state;
};

}

Q_DECLARE_METATYPE(SKT::UpdateWorker::UpdateError)

#endif // UPDATEWORKER_H
