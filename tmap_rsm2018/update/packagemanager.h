#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include "updatemanagerinterface.h"
#include "updateworker.h"
#include "packageinfo.h"

namespace SKT {

class RSAPublicKey;
class PlatformController;

class PackageManager : public QObject, public UpdateManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::UpdateManagerInterface)

public:
    PackageManager(PlatformController* platformController, QObject *parent = nullptr);
    virtual ~PackageManager();

    bool checkUpdatePackageInstalled();
    void commitUpdatePackage();
    bool loadInstalledPackage();

    QString packageVersion() const;
    QString mapVersion() const;
    QString localNetworkVersion() const;
    QString poiVersion() const;

    QString mapPath() const;
    QString localNetworkPath() const;
    QString crossImagePath() const;

    QObject* qobject() override { return this; }

public slots:
    PackageStatus packageStatus() const override { return m_packageStatus; }
    bool getStatusUpdateCommitted() const override { return m_updateCommitted;}
    void cancelUpdate() override; // dialog cancel to update
    void startUpdate() override;
    void stopUpdate() override;
    void applyUpdate() override;

    void onMediaMounted(const QString &mountPath);
    void onMediaUnmounted();

    void onSpmCentralLockStateUpdated(int lockstate);
    void onDiagDataUpdateRequested();
    void onDiagDataDeleteRequested();

private:
    int deleteDiagMapData();

signals:
    void packageStatusChanged(SKT::UpdateManagerInterface::PackageStatus status);
    void updatePackageRequestCentralLock(int state); // 2019-11-29, fix TMCRSM-2534, call to platform, he can request reserve, lock,
    void updatePackageMounted(bool locked);
    void updatePackageProgress(quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount);
    void updatePackageStored();
    void updatePackageError(SKT::UpdateManagerInterface::ErrorType errorType);
    void updateApplied();
    void updateCommitted(bool success);
    void updatePackageUnmounted();

    void updateDiagUpdateStatusInfo(int status, int progress);
    void updateDiagDeleteStatusInfo(int status, int progress);

private slots:
    void handleUpdateProgress(int id, quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount);
    void handleUpdateSuccess(int id);
    void handleUpdateError(int id, SKT::UpdateWorker::UpdateError error);
    void handleRemount(bool success);

private:
    PackageContainerPtr readPackageContainer(const QString &fileName);
    void setPackageStatus(PackageStatus status);
    bool commitUpdate();
    void finishWorker();
    void replaceDestinationTag(QString &str) const;
    bool tryMediaMountPermitted( const QString& mediaPath);
    bool tryNavdataMountPermitted(int rw);

private:
    PlatformController* m_platformController;
    PackageStatus m_packageStatus;

    QSharedPointer<RSAPublicKey> m_publicKey;
    PackageContainerPtr m_current;
    PackageContainerPtr m_usbPackage;

    QString m_usbPackagePath;
    QString m_mountMediaPath;

    UpdateWorker* m_worker;
    int m_workerId;
    int m_centralLocked; // central lock state
    bool m_updateCommitted;
    bool m_ByDiagDataUpdate; // diagnostic data update case
};

}

#endif // PACKAGEMANAGER_H
