#ifndef PLATFORMS_LINUX_GEN3_NAVIGATIONEXTCENTRALLOCK_H_
#define PLATFORMS_LINUX_GEN3_NAVIGATIONEXTCENTRALLOCK_H_
#include <QObject>

class OrgBoschCmNavigationNavigationExtServiceInterface;
namespace SKT {
class NavigationExtCentralLock: public QObject {
	Q_OBJECT
public:
    enum CentralLockStateType {
		CL_State_Unknown = 0x0,
		CL_Used,
		CL_Reserved,
		CL_Locking,
		CL_Locked,
		CL_Released
	};

	enum CentralLockRequestType {
		CL_Request_Use = 0x0,
		CL_Request_Reserve,
		CL_Request_Lock,
		CL_Request_MasterLock,
		CL_Request_Release,
		CL_Request_None
	};

	NavigationExtCentralLock(OrgBoschCmNavigationNavigationExtServiceInterface* service);
	bool isLocked() const;
	bool isPending() const { return mIsPending; }


public Q_SLOTS:
	void useLock();
	void reserveLock();
	void lock();
	void releaseLock();

protected Q_SLOTS:
    void updateLockState(const QString& name, const QVariant& value);

	Q_SIGNALS:
    void centralLockStateChanged(int lockState);

protected:
	void setCentralLockState(int state);

private:
    OrgBoschCmNavigationNavigationExtServiceInterface *mService;
	CentralLockRequestType mCurrentRequest;
    CentralLockStateType   mExpectState;
	void (NavigationExtCentralLock::* mNextRequest)();
	bool mIsPending;
};

}

#endif // PLATFORMS_LINUX_GEN3_NAVIGATIONEXTCENTRALLOCK_H_