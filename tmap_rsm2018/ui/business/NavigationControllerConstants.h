#ifndef NAVIGATIONCONTROLLERCONSTANTS_H
#define NAVIGATIONCONTROLLERCONSTANTS_H

#include "Constants.h"

namespace SKT {

class NavigationControllerConstants {
public:
    enum NotificationK {
        ApplyController = Constants::NaviController,
        RequestController,
        ResponseController,
        ChangeOnlineStatus,
        ShowTrafficInfo,
        QueryOnlineStatus,
        ResponseOnlineStatus,
        ApplyCSW,
        ClearCSW,
        CollectCSW,
        RegistCSW,
        RemoveCSW,
        ResetCSW,
        SelectCSW,
        SelectCSWByObjectName,
        SelectDefaultSoftkeyCSW,
        UpdateCswDebugInfo,
        UpdateCavatarIcon,
        SetCavatarIcon,
        ChangeLanguage,
		UpdatedUserProfile, /** user profile updated */
		ChangedUserProfile,  /**  user profile changed to other one */
		ModifiedUserData,  /** User Data modified from UI */
        GoToStandby,
        GoToRunning,
        PauseMapView,
        ChangeDateFormat,
        ChangeTimeFormat,
        RequestSkyState,
        ResponseSkyState,
        RequestSkyUpdate,
        UpdateSkyState,
        ShowSettingVolume,
        SpeakPhreaseSample,
        CallZoneApi,
        StartMapInClusterRenderer,
        ShowGadget,
        StartGadgetRenderer,
        NavigationAppeared,
        NavigationDisappeared,
        FirstNavigationAppeared,
        SaveGadgetImage,
        SendNotiGetVoiceGuidanceActive,
        SendNotiSetOverSpeedSoundAlertInfoActive,
        FileGpsSetPos,
        FileGpsRewind,
        FileGpsFastForward,
        ReplyToViewList,
        SwitchToAppIfNecessary,
        TestMountMedia,
        AppLostCswFocus,
        UpdateEditFieldText,
        ReserveAppSwitchJob,
        ReserveSystemSwitchJob,
        RequestAroundSearch,
        RequestContactGadgetSearch,
        ContextSwitchRequested,
        SpcxRouteGuidanceStarted,
        ChannelServerPolicyFinished,
        RequestChannelServerPolicyFinished,
        ReplyToChannelServerPolicyFinished,
    };
};

}

#endif
