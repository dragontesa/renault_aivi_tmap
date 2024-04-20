#ifndef UPDATEMANAGERINTERFACE_H
#define UPDATEMANAGERINTERFACE_H

#include <QObject>

namespace SKT {

class UpdateManagerInterface
{
public:
    enum PackageStatus {
        PackageStatusNone = 0,
        PackageStatusNewer,     // USB has a newer package
        PackageStatusSame,
        PackageStatusOlder,
        PackageStatusError
    };

    enum ErrorType {
        ErrorTypeNone = 0,
        ErrorTypeFileRead,
        ErrorTypeFileWrite,
        ErrorTypeFileIntegrity,
        ErrorTypeMediaUnmounted,
        ErrorTypeUnknown
    };

    enum PmSystemLockRequestType {
        PM_CL_Request_Use = 0x0,
        PM_CL_Request_Reserve,
        PM_CL_Request_Lock,
        PM_CL_Request_MasterLock,
        PM_CL_Request_Release,
        PM_CL_Request_None
    };

    enum PmSystemLockStateType {
        PM_CL_State_Unknown = 0x0,
        PM_CL_Used,
        PM_CL_Reserved,
        PM_CL_Locking,
        PM_CL_Locked,
        PM_CL_Released
    };

    virtual ~UpdateManagerInterface() {}

    /**
     * @brief qobject
     * @return
     */
    virtual QObject* qobject() = 0;

    // usb package status
    virtual PackageStatus packageStatus() const = 0;

    virtual bool getStatusUpdateCommitted() const = 0;

    // dialog cancel to update
    virtual void cancelUpdate() = 0;

    // start copying
    virtual void startUpdate() = 0;

    // stop copying
    virtual void stopUpdate() = 0;

    // prepare to apply update (e.g. restart)
    virtual void applyUpdate() = 0;

signals:    // just hint
    void packageStatusChanged(SKT::UpdateManagerInterface::PackageStatus status);

    // emit when the usb stick has the update package
    void updatePackageMounted(bool locked);

    // emit during update data is transferring
    void updatePackageProgress(quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount);

    // emit when all the map data are transferred
    void updatePackageStored();

    // update error
    void updatePackageError(SKT::UpdateManagerInterface::ErrorType errorType);

    // prepared to apply update
    void updateApplied();

    // swapped copied map data for original map data
    void updateCommitted(bool success);
};

}

Q_DECLARE_METATYPE(SKT::UpdateManagerInterface::PackageStatus)
Q_DECLARE_METATYPE(SKT::UpdateManagerInterface::ErrorType)
Q_DECLARE_INTERFACE(SKT::UpdateManagerInterface, "com.skt.tmap.update.UpdateManagerInterface/1.0")

#endif // UPDATEMANAGERINTERFACE_H
