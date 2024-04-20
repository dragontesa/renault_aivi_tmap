#include <QDir>
#include <QDebug>
#include <QElapsedTimer>
#include <QThread>
#include <unistd.h>

#include "updateworker.h"
#include "packageinfo.h"
#include "platformcontroller.h"
#include "packageutil.h"

#define REPORT_INTERVAL_MS              500
#define BLOCK_SIZE                      (16*1024)   // 16KB


namespace SKT {

enum UpdateState {
    UpdateStateStart,
    UpdateStateRemountRW,
    UpdateStateWaitRemountRW,
    UpdateStateCopy,
    UpdateStateRemountRO,
    UpdateStateWaitRemountRO,
    UpdateStateDone
};

UpdateWorker::UpdateWorker(int id,
                           PlatformController* platformController,
                           PackageContainerPtr pkgContainer,
                           const QString &packagePath,
                           QSharedPointer<RSAPublicKey> publicKey,
                           QObject* parent)
    : QObject(parent)
    , m_id(id)
    , m_platformController(platformController)
    , m_pkgContainer(pkgContainer)
    , m_packagePath(packagePath)
    , m_publicKey(publicKey)
    , m_abort(false)
    , m_error(UpdateError_OK)
    , m_state(UpdateStateStart)
{
#if 0 // it doesn't need longer to remount
    connect(m_platformController, &PlatformController::platformRemountStatus,
            this, &UpdateWorker::onRemountStatus);
#endif
}

void UpdateWorker::prepareProgress()
{
    m_progress = ProgressData();

    m_progress.totalSize = m_pkgContainer->fileSize;
    m_progress.totalCount = 1;

    for (const std::unique_ptr<PackageItem> &item : m_pkgContainer->packageInfo->items) {
        m_progress.totalCount += item->files.size();
        for (const std::unique_ptr<PackageFile> &file : item->files) {
            m_progress.totalSize += file->fileSize;
        }
    }
}

void UpdateWorker::reportProgress(bool force)
{
    if (force || m_progressTimer.hasExpired(REPORT_INTERVAL_MS)) {
        emit updateProgress(m_id,
                            m_progress.storedSize,
                            m_progress.totalSize,
                            m_progress.count,
                            m_progress.totalCount);
        m_progressTimer.restart();
    }
}

UpdateWorker::UpdateError UpdateWorker::verify(const QString &filePath, const QByteArray &signature)
{
    UpdateError error = UpdateError_OK;
    char block[BLOCK_SIZE];

    QFile in(filePath);
    if (!in.open(QIODevice::ReadOnly)) {
        error = UpdateError_IntegrityError;
    } else {
        MessageDigestSHA256 sha256;
        qint64 totalRead = 0;

        while (!in.atEnd()) {
            // check abort
            if (m_abort) {
                error = UpdateError_Aborted;
                break;
            }

            qint64 len = in.read(block, sizeof(block));
            if (len <= 0) {
                error = UpdateError_IntegrityError;
                break;
            }

            totalRead += len;

            // message digest
            if (!sha256.update(block, len)) {
                error = UpdateError_IntegrityError;
                break;
            }
        }

        if (error == UpdateError_OK) {
            // verify signature
            bool ok = false;
            QByteArray digest = sha256.final(&ok);
            if (!ok || !PackageUtil::verifySignature(digest, signature, m_publicKey.data())) {
                error = UpdateError_IntegrityError;
            }
        }
    }

    return error;
}

UpdateWorker::UpdateError UpdateWorker::safeCopy(const QString &src, const QString &dest, int fileSize)
{
    UpdateError error = UpdateError_OK;
    char block[BLOCK_SIZE];

    QFile in(src);
    if (!in.open(QIODevice::ReadOnly)) {
        error = UpdateError_ReadError;
    } else {
        QString tmp = dest + QLatin1String(".part");
        QFile out(tmp);

        if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            error = UpdateError_WriteError;
            out.close();
        } else {
            qint64 totalRead = 0;

            while (!in.atEnd()) {
                // check abort
                if (m_abort) {
                    error = UpdateError_Aborted;
                    break;
                }

                qint64 len = in.read(block, sizeof(block));
                if (len <= 0) {
                    error = UpdateError_ReadError;
                    break;
                }

                totalRead += len;

                if (len != out.write(block, len)) {
                    error = UpdateError_WriteError;
                    break;
                }

                m_progress.storedSize += len;
                reportProgress();
            }

            // check file size
            if (in.size() != totalRead || fileSize != totalRead) {
                error = UpdateError_ReadError;
            }

            // rename
            if (error == UpdateError_OK) {
                out.flush();
                if (fsync(out.handle())) {
                    qDebug() <<"safeCopy(E)..."<<strerror(errno)<<",errno:"<<errno;
                }
                if (!out.rename(dest)) {
                    error = UpdateError_WriteError;
                }
            }
        }

        if (error != UpdateError_OK) {
            out.remove();
        }
    }

    return error;
}

void UpdateWorker::replaceDestinationTag(QString &str)
{
    str.replace(QLatin1String(PACKAGE_TAG_MAP_PREFIX),
                PackageUtil::mapUpdatePath());
//    str.replace(QLatin1String(PACKAGE_TAG_VR_PREFIX),
//                PackageUtil::mapUpdatePath();
}

UpdateWorker::UpdateError UpdateWorker::cleanUp()
{
    // clean up backup path
    const QString oldPath = PackageUtil::mapBackupPath();
    QDir dir(oldPath);
    if (dir.exists()) {
        dir.removeRecursively();
    }

    // clean up update path
    const QString dest = PackageUtil::mapUpdatePath() + QDir::separator() + QLatin1String(PACKAGE_CONTAINER);
    bool needCleanUp = true;

    QFile f(dest);
    if (f.exists()) {
        // keep the packages stored in the update directory if version is same.
        PackageContainer* container = PackageContainer::fromFile(dest, m_publicKey.data());
        if (container) {
            if (container->packageInfo->version == m_pkgContainer->packageInfo->version) {
                needCleanUp = false;
            } else {
                qDebug() << dest << "exists but version mismatch.";
            }
            delete container;
        } else {
            qDebug() << dest << "exists but invalid.";
        }
    }

    if (needCleanUp) {
        // clean up and copy package descriptor
        qDebug() << "clean up" << PackageUtil::mapUpdatePath();

        QDir dir(PackageUtil::mapUpdatePath());
        dir.removeRecursively();
        dir.mkpath(PackageUtil::mapUpdatePath());
    }

    return UpdateError_OK;
}

UpdateWorker::UpdateError UpdateWorker::copyPackageDescriptor()
{
    qDebug() << "copy package descriptor";

    const QString dest = PackageUtil::mapUpdatePath() + QDir::separator() + QLatin1String(PACKAGE_CONTAINER);
    QFile f(dest);
    if (f.exists()) {
        f.remove();
    }

    // copy
    UpdateError error = safeCopy(m_pkgContainer->filePath, dest, m_pkgContainer->fileSize);

    // verify
    if (error == UpdateError_OK) {
        PackageContainer* container = PackageContainer::fromFile(dest, m_publicKey.data());
        if (container) {
            m_progress.count++;
            reportProgress(true);

            delete container;
        } else {
            error = UpdateError_IntegrityError;
        }
    }

    return error;
}

UpdateWorker::UpdateError UpdateWorker::copyAndVerifyPackage()
{
    UpdateError error = UpdateError_OK;

    QDir dir;

    for (const std::unique_ptr<PackageItem> &item : m_pkgContainer->packageInfo->items) {
        if (error != UpdateError_OK) {
            break;
        }

        QString sourcePath = m_packagePath + QDir::separator() + item->sourcePath + QDir::separator();
        QString destinationPath = item->destinationPath + QDir::separator();

        replaceDestinationTag(destinationPath);

        for (const std::unique_ptr<PackageFile> &file : item->files) {
            QString src = sourcePath + file->path + QDir::separator() + file->fileName;
            QString destDir = destinationPath + file->path;
            QString dest = destinationPath + file->path + QDir::separator() + file->fileName;
            QByteArray signature = file->signature;

            dir.mkpath(destDir);

            QFile d(dest);
            if (d.exists()) {
                qDebug() << "file exists. verifying..." << file->fileName;
                error = verify(dest, signature);
                if (error == UpdateError_OK) {
                    m_progress.count++;
                    m_progress.storedSize += file->fileSize;
                    reportProgress(true);
                    continue;
                } else if (error == UpdateError_Aborted) {
                    break;
                }
                qDebug() << "delete..." << file->fileName;
                d.remove();
            }

            qDebug() << "copying..." << file->fileName;

            error = safeCopy(src, dest, file->fileSize);
            if (error != UpdateError_OK) {
                break;
            }

            qDebug() << "verifying..." << file->fileName;

            error = verify(dest, signature);
            if (error != UpdateError_OK) {
                if (error != UpdateError_Aborted) {
                    // remove erroneous file
                    QFile f(dest);
                    if (f.exists()) {
                        f.remove();
                    }
                }
                break;
            }

            // report progress
            m_progress.count++;
            reportProgress(true);
        }
    }

    return error;
}

UpdateWorker::UpdateError UpdateWorker::markDone()
{
    QString dest = PackageUtil::mapUpdatePath() + QDir::separator() + QLatin1String(PACKAGE_MARK_FILENAME);
    QString tmp = dest + QLatin1String(".part");

    QFile out(tmp);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return UpdateError_WriteError;
    }

    QByteArray data;
    data.append('1');

    if (out.write(data) != data.size()) {
        return UpdateError_WriteError;
    }

    if (QFile::exists(dest))  {
        QFile::remove(dest);
    }

    if (!out.rename(dest)) {
        return UpdateError_WriteError;
    }

    return UpdateError_OK;
}

void UpdateWorker::abort()
{
    m_abort = true;
}

void UpdateWorker::onRemountStatus(bool success)
{
    qDebug() << Q_FUNC_INFO << "success:" << success;

    int nextState = m_state;

    if (m_state == UpdateStateWaitRemountRW) {
        if (success) {
            nextState = UpdateStateCopy;
        } else {
            nextState = UpdateStateDone;
            m_error = UpdateError_MountError;
        }
    } else if (m_state == UpdateStateWaitRemountRO) {
        if (success) {
            nextState = UpdateStateDone;
        } else {
            nextState = UpdateStateDone;
            m_error = UpdateError_MountError;
        }
    } else {
        qWarning() << Q_FUNC_INFO << "invalid state:" << m_state;
    }

    if (nextState != m_state) {
        m_state = nextState;
        QMetaObject::invokeMethod(this, "doNext", Qt::QueuedConnection);
    }
}

void UpdateWorker::doNext()
{
    forever {
        int nextState = m_state;

        switch (m_state) {
        case UpdateStateStart:
            prepareProgress();
            reportProgress(true);
            #if 0 // it doesn't need longer to remount as RW
            nextState = UpdateStateRemountRW;
            #else
            nextState = UpdateStateCopy;
            #endif
            break;
#if 0 // it doesn't need longer to remount
        case UpdateStateRemountRW:
        #ifdef BUILD_APP
            if (m_platformController->remountCommand(E_CMD_CODE_REMOUNT_DIR_RW, PackageUtil::mapUpdatePartition(), QString())) {
                nextState = UpdateStateWaitRemountRW;
                break;
            }
        #else
            if (m_platformController->remount(PackageUtil::mapUpdatePartition(), false)) {
                nextState = UpdateStateWaitRemountRW;
                break;
            }
        #endif
            m_error = UpdateError_MountError;
            nextState = UpdateStateDone;
            break;

        case UpdateStateWaitRemountRW:
            break;
#endif
        case UpdateStateCopy:
            m_error = cleanUp();
            if (m_error != UpdateError_OK) {
                nextState = UpdateStateDone;
                break;
            }

            m_error = copyPackageDescriptor();
            if (m_error != UpdateError_OK) {
                nextState = UpdateStateDone;
                break;
            }

            m_error = copyAndVerifyPackage();
            if (m_error != UpdateError_OK) {
                nextState = UpdateStateDone;
                break;
            }

            m_error = markDone();
            if (m_error != UpdateError_OK) {
                nextState = UpdateStateDone;
                break;
            }
            #if 0 // it doesn't need longer to remount
            nextState = UpdateStateRemountRO;
            #else
            nextState = UpdateStateDone;
            #endif
            break;
#if 0 // it doesn't need longer to remount
        case UpdateStateRemountRO:
        #ifdef BUILD_APP
            if (m_platformController->remountCommand(E_CMD_CODE_REMOUNT_DIR_RO, PackageUtil::mapUpdatePartition(), QString())) {
                nextState = UpdateStateWaitRemountRO;
                break;
            }
        #else
            if (m_platformController->remount(PackageUtil::mapUpdatePartition(), true)) {
                nextState = UpdateStateWaitRemountRO;
                break;
            }
        #endif
            m_error = UpdateError_MountError;
            nextState = UpdateStateDone;
            break;

        case UpdateStateWaitRemountRO:
            break;
#endif
        case UpdateStateDone:
            if (m_error == UpdateError_OK) {
                emit updateSuccess(m_id);
            } else {
                emit updateError(m_id, m_error);
            }
            break;

        default:
            qWarning() << "Invalid State=" << m_state;
            break;
        }

        if (nextState == m_state) {
            break;
        }

        m_state = nextState;
    }
}

}
