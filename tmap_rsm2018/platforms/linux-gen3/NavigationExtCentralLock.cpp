#include "NavigationExtCentralLock.h"
#include "interface/navigationext/navigationextserviceinterface.h"

static const QString CL_LOCK_NAME = QStringLiteral("UPDATE_LOCK_CENTRAL");
static const int CL_NAVI_OWNER_APPID = 230;

namespace SKT {
NavigationExtCentralLock::NavigationExtCentralLock(
    OrgBoschCmNavigationNavigationExtServiceInterface *service)
    : mService(service)
    , mIsPending(false)
    , mCurrentRequest(CL_Request_None)
    , mExpectState(CL_State_Unknown)
    , mNextRequest(nullptr)
{
    connect(service, &OrgBoschCmNavigationNavigationExtServiceInterface::propertyChanged,
            this, &NavigationExtCentralLock::updateLockState);
}

bool NavigationExtCentralLock::isLocked() const
{
    bool locked = false;
    NXLockStateInfoList stateInfo = mService->naviPropertyUpdateLockStates();
    for (NXLockStateInfo si: stateInfo) {
        if (si.m_ownerappid == CL_NAVI_OWNER_APPID &&
        si.m_lockstate == CL_Locked) {
            locked = true;
            break;
        }
    }

    return locked;
}

void NavigationExtCentralLock::useLock()
{
    mService->NaviMethUpdateLock(CL_LOCK_NAME, CL_Request_Use);
    mCurrentRequest = CL_Request_Use;
    mExpectState    = CL_Used;
    mNextRequest    = nullptr;
    mIsPending      = true;
}

void NavigationExtCentralLock::reserveLock()
{
    mService->NaviMethUpdateLock(CL_LOCK_NAME, CL_Request_Reserve);
    mCurrentRequest  = CL_Request_Reserve;
    mExpectState     = CL_Reserved;
//    mNextRequest     = &NavigationExtCentralLock::lock;
    mNextRequest     = nullptr;  // fix TMCRSM-2534, stop to update automatically
    mIsPending       = true;
}

void NavigationExtCentralLock::lock()
{
    mService->NaviMethUpdateLock(CL_LOCK_NAME, CL_Request_Lock);
    mCurrentRequest  = CL_Request_Lock;
    mExpectState     = CL_Locked;
    mNextRequest     = nullptr;
    mIsPending       = true;
}

void NavigationExtCentralLock::releaseLock()
{
    mService->NaviMethUpdateLock(CL_LOCK_NAME, CL_Request_Release);
    mCurrentRequest = CL_Request_Release;
    mExpectState    = CL_Released;
    mNextRequest    = nullptr;
    mIsPending      = false;  // 2019-12-03 released 상태는 받지 않으므로, pending 상태는 없다.
}

void NavigationExtCentralLock::updateLockState(const QString &propertyName, const QVariant &value)
{
    if (propertyName == QStringLiteral("NaviPropertyUpdateLockStates"))
    {
        NXLockStateInfoList list = value.value<NXLockStateInfoList>();
        qDebug() << "CL lock state updated: " << list.at(0).m_lockstate << ", owner:" << list.at(0).m_ownerappid;

        for (NXLockStateInfo si : list)
        {
            if (si.m_lockname == CL_LOCK_NAME)
            {
                bool imuser= false;
                // we can find user only in user list.
                for (ushort user: si.m_userlist) {
                    if (user == CL_NAVI_OWNER_APPID) {
                        imuser = true;
                        break;
                    }
                }
                if (si.m_ownerappid != CL_NAVI_OWNER_APPID) {
                    if (imuser) {
                        // fix TMCRSM-2534, we have to consider more scenarios with other processes
                        // 3. SKT navigation should release lock when handle reserved state which owner != 230 and strLockName =“UPDATE_LOCK_CENTRAL”.
                        if (si.m_lockstate == CL_Reserved) {
                            // if navi's current state is 'used'
                             releaseLock();
                        }
                        // if normal case, it must not be in here.
                        else if (si.m_lockstate == CL_Locking || si.m_lockstate == CL_Locked) {
                             releaseLock();
                        }
                        else if (si.m_lockstate == CL_Used) {
                            if (mExpectState == CL_Used && mIsPending) {
                                //  detect navi is in 'USED'
                                qDebug() << "spm lock updated to USED";
                                mIsPending = false;
                                emit centralLockStateChanged(si.m_lockstate);
                            }
                            // if normal case, it must not be in here.
                            // else if (mExpectState == CL_Released && mIsPending == false) {
                            //     useLock();
                            // }
                        }
                        }
                    else {
                         // SKT navigation should request use again when handle used state whichowner != 230, strLockName =“UPDATE_LOCK_CENTRAL” and 230 is not in lu16LockUserList.
                        if (si.m_lockstate == CL_Used) {
                            // 20200320 fix TMCRSM-2534 this cause navi couldn't going to be 'USED'
                        //   if (mExpectState == CL_Released)
                        //     {
                                useLock();
                        //    }
                        }
                    }
                    continue;
                }

                if (si.m_lockstate == mExpectState)
                {
                    qDebug() << "spm lock updated to="<<si.m_lockstate << ", pending="<<mIsPending;
                    if (mNextRequest != nullptr)
                       ((*this).*mNextRequest)();

                    mIsPending = false;

                    emit centralLockStateChanged(si.m_lockstate);
                }
            }
        }
    }
}

}
