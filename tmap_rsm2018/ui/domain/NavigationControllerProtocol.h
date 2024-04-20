#ifndef NAVIGATIONCONTROLLERPROTOCOL_H
#define NAVIGATIONCONTROLLERPROTOCOL_H


namespace SKT {

class NavigationControllerProtocol {
public:
    enum ProtocolK {
        ApplySpeedLevel,
        ApplyCameraGuidance,
        ApplyAttentionGuidance,
        ApplyRoadViolation,
        ApplySafeDrivingSoundAtMute,
        ApplyVoiceGuidance,
        ApplySetVoiceGuidance,
        ApplyFontScale,
        ApplyVoice,
        ApplyNaviVolume,
        ApplyNaviMute,
        ApplyAutoFuelType,
        ApplyVoiceGuidanceVocalPoint,

        GetAppVersion,
        GetGPSInfo,
        GetNaviVolume,
        GetNaviMute,
        GetDateFormat,
        GetTimeFormat,
        GetBluetoothStatus,
        GetAuthOTPstatus,
        GetDeviceId,
        GetAutoFuelType,
        GetAuthOTPTryAvailable,
        GetLang,
        GetSyncedProfileId,
        GetMapError,
        GetSyncedProfileName,
        GetCurrentProfileId,

        ChangeLanguage,
        CallPhoneNumber,
        CreateSessionByOTP,
        ClearSession,
        ClearAllSession,
        ClearSessionByProfileId,

        RequestRegisterSendToCar,
        RequestFindSendToCar,
        RequestRemoveSendToCar,

        RequestResetCurrentUserData,
        RequestResetAllUserData,

        ResponsedFreeTextSearchResult,
    };
};

}

#endif // NAVIGATIONCONTROLLERPROTOCOL_H
