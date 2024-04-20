#include <QDebug>
#include <QDir>
#include <QThread>
#include <QResource>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <unistd.h>

#include "packagemanager.h"
#include "platformcontroller.h"
#include "updateworker.h"
#include "packageutil.h"

namespace SKT {

PackageManager::PackageManager(PlatformController* platformController, QObject *parent)
    : QObject(parent)
    , m_platformController(platformController)
    , m_packageStatus(PackageManager::PackageStatusNone)
    , m_worker(0)
    , m_workerId(0)
    , m_updateCommitted(false)
    , m_centralLocked(false)
    , m_ByDiagDataUpdate(false)
{
    qRegisterMetaType<SKT::UpdateWorker::UpdateError>();
    qRegisterMetaType<SKT::UpdateManagerInterface::PackageStatus>();
    qRegisterMetaType<SKT::UpdateManagerInterface::ErrorType>();

    connect(m_platformController, &PlatformController::platformMediaMounted,
            this, &PackageManager::onMediaMounted);
    connect(m_platformController, &PlatformController::platformMediaUnmounted,
            this, &PackageManager::onMediaUnmounted);
    connect(m_platformController, &PlatformController::platformSpmCentralLockStateUpdated,
            this, &PackageManager::onSpmCentralLockStateUpdated);
    connect(this, &PackageManager::updatePackageRequestCentralLock,
            m_platformController, &PlatformController::onPlatformSpmCentralLocStateRequested );

    connect(m_platformController, &PlatformController::platformDiagDataUpdateRequested,
            this, &PackageManager::onDiagDataUpdateRequested);
    connect(m_platformController, &PlatformController::platformDiagDataDeleteRequested,
            this, &PackageManager::onDiagDataDeleteRequested);

    // Diagnosis Function
    connect(this, SIGNAL(updateDiagUpdateStatusInfo(int,int)),
            m_platformController, SLOT(onNavigationDiagDataUpdateStatusInfo(int,int)));
    connect(this, SIGNAL(updateDiagDeleteStatusInfo(int,int)),
            m_platformController, SLOT(onNavigationDiagDataDeleteStatusInfo(int,int)));

    // load public key
    QResource r(":/assets/local_public.pem");
    if (!r.isValid()) {
        qWarning("public key is not available!");
    }

    m_publicKey.reset(new RSAPublicKey(reinterpret_cast<const char*>(r.data()), r.size()));
}

PackageManager::~PackageManager()
{
    stopUpdate();
}

bool PackageManager::checkUpdatePackageInstalled()
{
    QString markPath = PackageUtil::mapUpdatePath() + QDir::separator() + QLatin1String(PACKAGE_MARK_FILENAME);

    return QFile(markPath).exists();
}

void PackageManager::commitUpdatePackage()
{
    #if 0 // it doesn't need longer to remount
    connect(m_platformController, &PlatformController::platformRemountStatus,
            this, &PackageManager::handleRemount);
#ifdef BUILD_APP //  LXC
    if (!m_platformController->remountCommand(E_CMD_CODE_REMOUNT_DIR_RW, PackageUtil::mapUpdatePartition(), QString())) {
#else
    if (!m_platformController->remount(PackageUtil::mapUpdatePartition(), false)) {
#endif
        qWarning() << "failed to remount!";
        disconnect(m_platformController, &PlatformController::platformRemountStatus, this, 0);
        emit updateCommitted(false);
    }
    else {
        qDebug() << "success remount to navdata";
    }
    #endif
    
    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as RW
    tryNavdataMountPermitted(2);
    bool success = commitUpdate();
    tryNavdataMountPermitted(1);
    emit updateCommitted(success);
}

bool PackageManager::loadInstalledPackage()
{
    qDebug() << Q_FUNC_INFO;

    QString filePath = PackageUtil::mapPath() + QDir::separator() + QLatin1String(PACKAGE_CONTAINER);

    m_current = readPackageContainer(filePath);

    if (!m_current) {
        qDebug() << "failed to load installed package!";
        // offers diagnosis map data status info
        if (PackageContainer::error == PackageContainer::E_ERROR_FILEOPEN) {
            bool existMapdata = false;
            QDir mapdata(PackageUtil::mapPath());
            existMapdata = mapdata.exists();
            if (existMapdata == false) {
                emit updateDiagDeleteStatusInfo(100,0); // NavDataDeleteCannotStatus_NoMapdata
            } else {
                emit updateDiagDeleteStatusInfo(101,0); // NavDataDeleteCannotStatus_FilePermissionDenied
                // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) try to remount mapdata directory to read
                tryNavdataMountPermitted(1);
                m_current = readPackageContainer(filePath);
            }
        }
        // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D)
        if (!m_current) return false;
    }

#ifdef QT_DEBUG
    qDebug().noquote() << "pacman package.json" << "\n" << m_current->toString();
#endif

    qDebug() << "Installed package=" << m_current->packageInfo->version;

    return true;
}

void PackageManager::cancelUpdate()
{
    m_centralLocked = PM_CL_Request_Use;
    emit updatePackageRequestCentralLock(PM_CL_Request_Use); // fix TMCRSM-2534, request use state to platform
    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as ro
    tryNavdataMountPermitted(1);
}

void PackageManager::startUpdate()
{
    qDebug() << Q_FUNC_INFO;
    // locked 후 한 번 더 lock 요청 수정
    int lockstat = m_centralLocked>>16&0xff;
    if (lockstat != PM_CL_Locked) {
        m_centralLocked = PM_CL_Request_Lock;
        emit updatePackageRequestCentralLock(PM_CL_Request_Lock); // fix TMCRSM-2534, request lock state to platform
    }

    if (!m_usbPackage) {
        qWarning() << "USB package not loaded.";
        emit updateDiagUpdateStatusInfo(100,0); // FAILED__NO_UPDATES_FOUND
        return;
    }

    if (m_packageStatus != UpdateManagerInterface::PackageStatusSame &&
            m_packageStatus != UpdateManagerInterface::PackageStatusNewer) {
        qWarning() << "invalid package status=" << m_packageStatus;
        emit updateDiagUpdateStatusInfo(101,0); // FAILED__INVALID_USB_DATA
        return;
    }

#ifdef BUILD_TARGET
//    if (!m_ByDiagDataUpdate) {
//        int lockstat = m_centralLocked>>16&0xff;
//         if (lockstat != PM_CL_Locked) {
//             qWarning() << "it needs to be locked with spm";
//             return;
//         }
//    }
#endif

    if (m_worker) {
        qWarning() << "already updating...";
        emit updateDiagUpdateStatusInfo(2,0); // AVAILABLE_UPDATES_IN_PROGRESS
        return;
    }

    if (m_usbPackagePath.isEmpty()) {
        qWarning() << "USB is not ready !!";
        emit updateDiagUpdateStatusInfo(19,0); // USB_REMOVED
        return;
    }

    QThread* thread = new QThread;
    m_worker = new UpdateWorker(++m_workerId, m_platformController, m_usbPackage, m_usbPackagePath, m_publicKey);
    m_worker->moveToThread(thread);

    connect(thread, &QThread::started, m_worker, &UpdateWorker::doNext);
    connect(thread, &QThread::finished, m_worker, &UpdateWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    connect(m_worker, &UpdateWorker::updateProgress, this, &PackageManager::handleUpdateProgress);
    connect(m_worker, &UpdateWorker::updateError, this, &PackageManager::handleUpdateError);
    connect(m_worker, &UpdateWorker::updateSuccess, this, &PackageManager::handleUpdateSuccess);

    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as RW
    tryNavdataMountPermitted(2);

    thread->start();
    emit updateDiagUpdateStatusInfo(12,0); // INSTALLATION_STARTED
}

void PackageManager::stopUpdate()
{
    // fix TMCRSM-2534, request lock state to platform
    int lockstat = m_centralLocked>>16&0xff;
    if (lockstat != PM_CL_Used ) {// need to update if lock is not 'used'
        m_centralLocked = PM_CL_Request_Use;
        emit updatePackageRequestCentralLock(PM_CL_Request_Use);
    }

    if (m_worker) {
        m_workerId++; // to ignore the queued signals.
        m_worker->abort();
    }
    finishWorker();
    
    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as ro
    tryNavdataMountPermitted(1);
}

void PackageManager::applyUpdate()
{
    if (m_platformController->restart()) {
        QMetaObject::invokeMethod(this, "updateApplied", Qt::QueuedConnection);
    }
}

QString PackageManager::packageVersion() const
{
    if (m_current) {
        return m_current->packageInfo->version;
    }
    return QString();
}

QString PackageManager::mapVersion() const
{
    if (m_current) {
        for (const std::unique_ptr<PackageItem> &item : m_current->packageInfo->items) {
            if (item->name == QLatin1String(PACKAGE_NAME_VSM)) {
                return item->version;
            }
        }
    }
    return QString();
}

QString PackageManager::localNetworkVersion() const
{
    if (m_current) {
        for (const std::unique_ptr<PackageItem> &item : m_current->packageInfo->items) {
            if (item->name == QLatin1String(PACKAGE_NAME_LOCAL)) {
                return item->version;
            }
        }
    }
    return QString();
}

QString PackageManager::poiVersion() const
{
    return localNetworkVersion();
}

void PackageManager::replaceDestinationTag(QString &str) const
{
    str.replace(QLatin1String(PACKAGE_TAG_MAP_PREFIX),
                PackageUtil::mapPath());
//    str.replace(QLatin1String(PACKAGE_TAG_VR_PREFIX),
    //                PackageUtil::mapPath();
}

QString PackageManager::mapPath() const
{
    if (m_current) {
        for (const std::unique_ptr<PackageItem> &item : m_current->packageInfo->items) {
            if (item->name == QLatin1String(PACKAGE_NAME_VSM)) {
                QString path = item->destinationPath;
                replaceDestinationTag(path);
                return path;
            }
        }
    }
    return QString();
}

QString PackageManager::localNetworkPath() const
{
    if (m_current) {
        for (const std::unique_ptr<PackageItem> &item : m_current->packageInfo->items) {
            if (item->name == QLatin1String(PACKAGE_NAME_LOCAL)) {
                QString path = item->destinationPath;
                replaceDestinationTag(path);
                return path;
            }
        }
    }
    return QString();
}

QString PackageManager::crossImagePath() const
{
    if (m_current) {
        for (const std::unique_ptr<PackageItem> &item : m_current->packageInfo->items) {
            if (item->name == QLatin1String(PACKAGE_NAME_CROSSIMAGE)) {
                QString path = item->destinationPath;
                replaceDestinationTag(path);
                return path;
            }
        }
    }
    return QString();
}

/**
 * @param mediaPath the path of usb mounted
 * @return lxc bound path for the usb
 */
bool PackageManager::tryMediaMountPermitted( const QString& mediaPath)
{
    if (!m_platformController->remountCommand(E_CMD_CODE_REMOUNT_USB_RW, mediaPath, QString())) {
        qInfo() << "pacman failed to remount the usb media as rw";
        return false;
    }

#if 0 // it has a problem, why this change owner from lxc_extnav to root ?
    // change  owner to lxc_extnav
    if (!m_platformController->remountCommand(E_CMD_CODE_LXC_CHOWN, mediaPath, QString())) {
        qInfo() << "pacman failed to chown usb lxc bind path" << mediaPath;
        return false;
    }
#endif
    // bind the media to lxc directory
    QString bindPath = PackageUtil::mediaPermitBindPath(mediaPath);
    if (!m_platformController->remountCommand(E_CMD_CODE_MOUNT_DIR, mediaPath, bindPath)) {
        qInfo() << "pacman failed to bind to the directory " << bindPath;
        return false;
    }

    m_mountMediaPath = bindPath;

    return true;
}

/**
 * Remount navdata partition or mapdata directory
 * @param rw  1: read only 2: read & write
 **/
bool PackageManager::tryNavdataMountPermitted(int rw)
{
    QString datapth = PackageUtil::mapPath();
    QString mappth = datapth.mid(0, datapth.lastIndexOf("/"));
    QString navpth = mappth.mid(0, mappth.lastIndexOf("/"));

    // 맵데이터 디렉토리 RO 리마운트, 맵데이터 업데이트 이후 쓰기방지, 
    if (rw == 1)
    {
        #if defined(PLATFORM_GEN3) && defined(BUILD_TARGET)
        if (!m_platformController->remountCommand(E_CMD_CODE_LXC_CHOWN, navpth, QString())) {
            qInfo() << "pacman failed to remount navdata as owner for ro";
            return false;
        }

        if (!m_platformController->remountCommand(E_CMD_CODE_REMOUNT_DIR_RO, mappth, QString())) {
            qInfo() << "pacman failed remount mapdata as ro";
            return false;
        }
        #endif

        QFileInfo mapfi(mappth);
        bool ok = mapfi.isReadable() && !mapfi.isWritable();
        qDebug() << "pacman remount mappath as ro =" << ok;
        return ok;
    }

    // navdata 파티션 owner 변경과 map 디렉토리 RW 리마운트  SW3037 (SKT SW1949.3D) 이슈 대안
    // Neil: The navdata partition user name is pj_nav when device startup, it cause the diagnosis map update failed.
    // The wrapper help mount the navdata partition when startup, but some unknow reason, the navdata partition still is pj_nav.
    // I need your help in SKT navigation side, when start map update, if you can not access navdata partition ,
    //  please try remount the navdata partition as read and write, set the user name lxc_extnav and group is pj_nav_share.
    if (rw == 2)
    {
        #if defined(PLATFORM_GEN3) && defined(BUILD_TARGET)
        if (!m_platformController->remountCommand(E_CMD_CODE_LXC_CHOWN, navpth, QString())) {
            qInfo() << "pacman failed to remount navdata as owner for rw";
            return false;
        }

        if (!m_platformController->remountCommand(E_CMD_CODE_REMOUNT_DIR_RW, mappth, QString())) {
            qInfo() << "pacman failed to remount mapdata as rw";
            return false;
        }
        #endif

        QFileInfo fi(mappth);
        bool ok = fi.isReadable() && fi.isWritable();
        qDebug() << "pacman remount navdata as rw= " << ok;
        return ok;
    }
    

    return false;
}

void PackageManager::onMediaMounted(const QString &mountPath)
{
    qDebug() << "pacman, path=" << mountPath;
    emit updateDiagUpdateStatusInfo(1,0); // AVAILABLE_UPDATES_STARTED
    m_usbPackagePath = mountPath;
    if (!m_usbPackagePath.endsWith(QDir::separator())) {
        m_usbPackagePath += QDir::separator();
    }
    m_usbPackagePath += QLatin1String(PACKAGE_FILEPATH_USB);
    qDebug() << "pacman pkg path:" << m_usbPackagePath;

    // first, check readable (for LXC environment) , then should try to remount a usb as RW and bind to the lxc directory
    if (!PackageUtil::hasMediaReadable(mountPath)) {
        tryMediaMountPermitted(mountPath);
        if (!PackageUtil::hasPermitMedia(PackageUtil::mediaLxcBoundPath())) {
            qInfo() << "pacman not found permit bound media ";
            emit updateDiagUpdateStatusInfo(4,0); // AVAILABLE_UPDATES_ABORTED_BY_ERROR
            return;
        }

        m_usbPackagePath = PackageUtil::mediaLxcBoundPath()+QDir::separator()+QLatin1String(PACKAGE_FILEPATH_USB);
    }

    QString usbPackagePath = m_usbPackagePath;
    QString packagePath = usbPackagePath + QDir::separator() + QLatin1String(PACKAGE_CONTAINER);

    if (!QFile::exists(packagePath)) {
        qInfo() << "the media doesn't have the right map data ";
        emit updateDiagUpdateStatusInfo(4,0); // AVAILABLE_UPDATES_ABORTED_BY_ERROR
        return;
    }

    QFuture<PackageContainerPtr> future =
            QtConcurrent::run(this, &PackageManager::readPackageContainer, packagePath);

    QFutureWatcher<PackageContainerPtr>* watcher = new QFutureWatcher<PackageContainerPtr>;

    connect(watcher, &QFutureWatcher<PackageContainerPtr>::finished,
            [this, watcher, usbPackagePath]() {
        QFuture<PackageContainerPtr> future = watcher->future();
        watcher->deleteLater();

        // ignore if the package path is changed(or unmounted)
        if (usbPackagePath != m_usbPackagePath) {
            qDebug() << "pacman unmatch usb path";
            emit updateDiagUpdateStatusInfo(4,0); // AVAILABLE_UPDATES_ABORTED_BY_ERROR
            return;
        }

        PackageStatus status = PackageStatusNone;

        m_usbPackage = future.result();

        if (m_usbPackage) {
            qDebug() << "usb package=" << m_usbPackage->packageInfo->version;

            if (m_current) {
                qDebug() << "installed package=" << m_current->packageInfo->version;

                int comp = m_usbPackage->packageInfo->version.compare(m_current->packageInfo->version);
                qDebug() << "pacman ver comp:" << comp;
                status = (comp > 0) ? PackageStatusNewer :
                         (comp < 0) ? PackageStatusOlder :
                         PackageStatusSame;
            } else {
                status = PackageStatusNewer;
            }
        } else {
            qDebug() << "pacman, This media doesn't have the map data.";
            status = PackageStatusError;
        }

        setPackageStatus(status);

        if (status != PackageStatusError) {
        m_centralLocked = PM_CL_Request_Reserve;
            emit updatePackageRequestCentralLock(PM_CL_Request_Reserve); // fix TMCRSM-2534, request use state to platform
            if (!m_ByDiagDataUpdate) {// UI is unnecessary on diagnosis mode
                #ifdef BUILD_TARGET
                    //emit updatePackageMounted(m_centralLocked == PM_CL_Locked);
                #else
                    emit updatePackageMounted(true);
                #endif
            }

            emit updateDiagUpdateStatusInfo(3,0); // AVAILABLE_UPDATES_FINISHED
            if (m_ByDiagDataUpdate) // Auto start on diagnosis
               startUpdate();
        }
        else {
            emit updateDiagUpdateStatusInfo(4,0); // AVAILABLE_UPDATES_ABORTED_BY_ERROR
        }
    });

    watcher->setFuture(future);
}

void PackageManager::onMediaUnmounted()
{
    qDebug() << Q_FUNC_INFO;

    bool updating = m_worker ? true : false;

    // 20200324 fix https://tde.sktelecom.com/pms/browse/TMCRSM-4621 Navi should unmount USB media remounted
    if (m_mountMediaPath.indexOf(QStringLiteral("media")) >= 0) {
        if (!m_platformController->remountCommand(E_CMD_CODE_UMOUNT_DIR, m_mountMediaPath, QString())) {
            qInfo() << "failed to unmount "<< m_mountMediaPath;
        }
    }
    stopUpdate();
    m_usbPackagePath.clear();
    m_usbPackage.clear();
    m_packageStatus = PackageStatusNone;

    if (updating) {
        emit updatePackageError(ErrorTypeMediaUnmounted);
    }

    // chunjae.lee 2018.03.05
    emit updatePackageUnmounted();

    m_ByDiagDataUpdate = false;
    // update diagnosis data update status info
    emit updateDiagUpdateStatusInfo(19,0); // USB_REMOVED
    emit updateDiagUpdateStatusInfo(0,0); // IDLE
}


void PackageManager::onSpmCentralLockStateUpdated(int lockstate)
{
    int requestLockType = m_centralLocked&0xff;
    m_centralLocked |= (lockstate&0xff)<<16;
    if (requestLockType == PM_CL_Request_Reserve) {
        if (lockstate == PM_CL_Reserved) {
           emit updatePackageMounted(true);
         }
         else {
           updatePackageMounted(false);
         }
    }
    else if (requestLockType == PM_CL_Request_Lock) {
        if (lockstate ==  PM_CL_Locked)
            startUpdate();
    }
}

void PackageManager::onDiagDataUpdateRequested()
{
    emit updateDiagUpdateStatusInfo(0,0); // IDLE
    m_ByDiagDataUpdate = true;

    startUpdate();
}

void PackageManager::onDiagDataDeleteRequested()
{
    QFuture<int> f = QtConcurrent::run(this, &PackageManager::deleteDiagMapData);
}

int PackageManager::deleteDiagMapData()
{
    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as RW
    tryNavdataMountPermitted(2);

    qDebug() << "pacman, delete Diag map data start !!";
    emit updateDiagDeleteStatusInfo(0, 0); // update NavDataDeleteStatus_MAPDATA_DELETE_STARTED
    QDir mapdataDir(PackageUtil::mapPath());
    if (!mapdataDir.exists()) {
        qWarning() << "diag mapdata deletion not found mapdata";
        return 3; // NavDataDeleteStatus_MAPDATA_DELETE_FAILED
    }

    QStringList filter{"*"};
    QFileInfoList flist = mapdataDir.entryInfoList(filter,QDir::AllEntries|QDir::QDir::NoDot|QDir::QDir::NoDotDot);
    int cnt = flist.size();
    int i=0;
    int fails = 0;
    for (auto fi: flist) {
        fails = 0;
        if (fi.isFile())
            fails += !QFile::remove(fi.filePath());
        else {
            QDir rmdir(fi.filePath());
            fails += !rmdir.removeRecursively();
        }
        i++;
        emit updateDiagDeleteStatusInfo((fails)?3:1,(float)i/cnt*100); // success: NavDataDeleteStatus_MAPDATA_DELETE_IN_PROGRESS, error:NavDataDeleteStatus_MAPDATA_DELETE_FAILED
    }
    mapdataDir.removeRecursively();
    emit updateDiagDeleteStatusInfo((fails)?3:2, 100); // NavDataDeleteStatus_MAPDATA_DELETE_FINISHED or NavDataDeleteStatus_MAPDATA_DELETE_FAILED

    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as RW
    tryNavdataMountPermitted(1);
    
    qDebug() << "pacman, delete Diag map data end!!";
    return fails;
}

void PackageManager::handleUpdateProgress(int id, quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount)
{
    if (m_workerId != id) {
        return;
    }
    qDebug() << "handleUpdateProgress: count=" << count << "totalCount=" << totalCount
             << "storedSize=" << storedSize << "totalSize=" << totalSize;
       emit updatePackageProgress(storedSize, totalSize, count, totalCount);

       // update diagnosis data update status info
       int progress = ((storedSize*1.0) / (totalSize*1.0)) * 100.0;
       emit updateDiagUpdateStatusInfo(13, progress); // INSTALLATION_IN_PROGRESS
}

void PackageManager::handleUpdateSuccess(int id)
{
    if (m_workerId != id) {
        return;
    }

    qDebug() << "handleUpdateSuccess";

    finishWorker();

    emit updatePackageStored();
    // update diagnosis data update status info
    emit updateDiagUpdateStatusInfo(15,0); // INSTALLATION_FINISHED
}

void PackageManager::handleUpdateError(int id, SKT::UpdateWorker::UpdateError error)
{
    if (m_workerId != id) {
        return;
    }

    qDebug() << "handleUpdateError error=" << error;

    // update diagnosis data update status info
    emit updateDiagUpdateStatusInfo(14,0); // INSTALLATION_STOPPED
    emit updateDiagUpdateStatusInfo(20,0); // UPDATE_SOURCE_ERROR

    finishWorker();

    if (error == UpdateWorker::UpdateError_Aborted) {
        return;
    }

    UpdateManagerInterface::ErrorType updateErr;

    switch (error) {
    case UpdateWorker::UpdateError_ReadError:
        updateErr = UpdateManagerInterface::ErrorTypeFileRead;
        break;
    case UpdateWorker::UpdateError_WriteError:
        updateErr = UpdateManagerInterface::ErrorTypeFileWrite;
        break;
    case UpdateWorker::UpdateError_IntegrityError:
        updateErr = UpdateManagerInterface::ErrorTypeFileIntegrity;
        break;
    default:
        updateErr = UpdateManagerInterface::ErrorTypeUnknown;
        break;
    }

        emit updatePackageError(updateErr);

    // 2020.2.13 lckxx, SW3037 (SKT SW1949.3D) remount mapdata directory as ro
    tryNavdataMountPermitted(1);
}

void PackageManager::handleRemount(bool success)
{
    // one time connection
    qDebug() << "pkg man remount state:" << success;
    disconnect(m_platformController, &PlatformController::platformRemountStatus, this, 0);

    if (success) {
        success = commitUpdate();
#ifdef BUILD_APP
        m_platformController->remountCommand(E_CMD_CODE_REMOUNT_DIR_RO, PackageUtil::mapUpdatePartition(), QString());
#else
        m_platformController->remount(PackageUtil::mapUpdatePartition(), true);
#endif
    } else {
        qWarning() << "failed to remount!";
    }

    emit updateCommitted(success);
}

PackageContainerPtr PackageManager::readPackageContainer(const QString &fileName)
{
    qDebug() << Q_FUNC_INFO;
    return PackageContainerPtr(PackageContainer::fromFile(fileName, m_publicKey.data()));
}

void PackageManager::setPackageStatus(PackageStatus status)
{
    if (m_packageStatus != status) {
        m_packageStatus = status;
        emit packageStatusChanged(status);
    }
}

bool PackageManager::commitUpdate()
{
    bool success = true;
    const QString oldPath = PackageUtil::mapBackupPath();

    QDir oldDir(oldPath);
    if (oldDir.exists()) {
        if (!oldDir.removeRecursively()) {
            qWarning() << "failed to delete" << oldDir;
            success = false;
        }
    }

    if (success) {
        QDir dir(PackageUtil::mapPath());
        if (dir.exists() && !dir.rename(PackageUtil::mapPath(), oldPath)) {
            qWarning() << "failed to rename" << PackageUtil::mapPath() << "to" << oldPath;
            success = false;
        }
    }

    if (success) {
        QDir dir;
        if (!dir.rename(PackageUtil::mapUpdatePath(), PackageUtil::mapPath())) {
            qWarning() << "failed to rename" << PackageUtil::mapUpdatePath() << "to" << PackageUtil::mapPath();
            success = false;
        }
    }
    m_updateCommitted = success;
    return success;
}

void PackageManager::finishWorker()
{
    m_ByDiagDataUpdate = false;
    // clean up thread
    if (m_worker) {
        QThread* thread = m_worker->thread();
        if (thread->isRunning()) {
            thread->quit();
            thread->wait();
        }
        qDebug() << "UpdateManager: worker is finished.";
        m_worker = 0;
    }
}

}

