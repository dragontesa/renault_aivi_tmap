#ifndef NAVIGATIONUPDATEMANAGERCONSTANTS_H
#define NAVIGATIONUPDATEMANAGERCONSTANTS_H

#include "Constants.h"

namespace SKT {

class NavigationUpdateManagerConstants {
public:
    enum NotificationK {
        Apply = Constants::UpdateManager,        
        ApplyStartUpdate,
        ApplyStopUpdate,
        ApplyCancelUpdate,
        ApplyUpdate,
        ApplyRgStatus,
        Request,
        RequestPackageStatus,
        RequestNeedRestart,

        Response,
        ResponseUpdatePackageMounted,
        ResponseUpdatePackageUnmounted,
        ResponseUpdatePackageProgress,
        ResponseUpdatePackageStored,
        ResponseUpdatePackageError,
        ResponsePackageStatus,        
        ResponseUpdateApplied,
        ResponseNeedRestart,

        Get,
        GetStatusUpdateCommitted,

        ChangeOnlineStatus,
        QueryOnlineStatus,
        ResponseOnlineStatus,
        ApplyCSW,
        ClearCSW,
        RegistCSW,
        RemoveCSW,
        ResetCSW,
        SelectCSW,
        ChangeLanguage
    };
};

}

#endif
