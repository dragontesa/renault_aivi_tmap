#ifndef UI_BUSINESS_HIDDENSETTINGCONSTANTS_H_
#define UI_BUSINESS_HIDDENSETTINGCONSTANTS_H_

#include "Constants.h"
#include <QObject>
#include "qctestingfeatures.h"

namespace SKT {

class HiddenSettingConstants : public QObject
{
	Q_OBJECT

    // db section names
	Q_PROPERTY( QString HiddenSetting       MEMBER   HiddenSetting       CONSTANT )
	Q_PROPERTY( QString HiddenVsmSetting    MEMBER   HiddenVsmSetting    CONSTANT )

	// db key names
	Q_PROPERTY( QString TnappServerIp       MEMBER   TnappServerIp      CONSTANT )
	Q_PROPERTY( QString TnappServerPort     MEMBER   TnappServerPort    CONSTANT )
	Q_PROPERTY( QString NddsServer          MEMBER   NddsServer         CONSTANT )
    Q_PROPERTY( QString AvNextServer        MEMBER   AvNextServer       CONSTANT )
	Q_PROPERTY( QString NetworkLog          MEMBER   NetworkLog         CONSTANT )
	Q_PROPERTY( QString SafeHelperServerIp   MEMBER  SafeHelperServerIp          CONSTANT )
	Q_PROPERTY( QString SafeHelperServerPort MEMBER  SafeHelperServerPort     CONSTANT )
	Q_PROPERTY( QString TmapLog             MEMBER   TmapLog            CONSTANT )
	Q_PROPERTY( QString GpsMode             MEMBER   GpsMode            CONSTANT )
	Q_PROPERTY( QString GpsLog              MEMBER   GpsLog             CONSTANT )
	Q_PROPERTY( QString FixedRerouteTime    MEMBER   FixedRerouteTime   CONSTANT )
	Q_PROPERTY( QString MapMatching         MEMBER   MapMatching        CONSTANT )
	// Q_PROPERTY( QString RoadSignVoiceStreamingServer   MEMBER       RoadSignVoiceStreamingServer   CONSTANT )
	Q_PROPERTY( QString TvasVersion         MEMBER   TvasVersion    CONSTANT )

	// db vsm key names
	Q_PROPERTY( QString VsmDebugSettingUi     MEMBER       VsmDebugSettingUi  CONSTANT )
    Q_PROPERTY( QString VsmLevelSettingUi     MEMBER       VsmLevelSettingUi  CONSTANT )
	Q_PROPERTY( QString VsmServer        MEMBER       VsmServer     CONSTANT )
	Q_PROPERTY( QString VsmConfigId      MEMBER       VsmConfigId   CONSTANT )
	Q_PROPERTY( QString VsmConfigVer     MEMBER       VsmConfigVer  CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelMap     MEMBER       VsmAutoLevelMap  CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelLowLevel        MEMBER       VsmAutoLevelLowLevel  CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelMiddleLevel     MEMBER       VsmAutoLevelMiddleLevel  CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelHighLevel       MEMBER       VsmAutoLevelHighLevel    CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelLowTilt         MEMBER       VsmAutoLevelLowTilt      CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelMiddleTilt      MEMBER       VsmAutoLevelMiddleTilt   CONSTANT )
	Q_PROPERTY( QString VsmAutoLevelHighTilt        MEMBER       VsmAutoLevelHighTilt     CONSTANT )
    Q_PROPERTY( QString VsmUseFps        MEMBER       VsmUseFps     CONSTANT )
    Q_PROPERTY( QString VsmDisable3dObjects        MEMBER       VsmDisable3dObjects     CONSTANT )
    Q_PROPERTY( QString VsmGLAntiAliasing        MEMBER       VsmGLAntiAliasing     CONSTANT )
    Q_PROPERTY( QString VsmFlickSpeedTest        MEMBER       VsmFlickSpeedTest     CONSTANT )
	// Q_PROPERTY( QString LocalMapmatching     MEMBER  LocalMapmatching     CONSTANT )
	// Q_PROPERTY( QString FusedLocation   MEMBER       FusedLocation          CONSTANT )
	// Q_PROPERTY( QString Crashlytics     MEMBER       Crashlytics            CONSTANT )
	Q_PROPERTY( QString ForceCrash      MEMBER       ForceCrash             CONSTANT )
    Q_PROPERTY( QString LanguageTest      MEMBER       LanguageTest             CONSTANT )
	// Q_PROPERTY( QString ADBLog          MEMBER       ADBLog                 CONSTANT )
	// Q_PROPERTY( QString CarConnectivityLog           MEMBER       CarConnectivityLog    CONSTANT )
	Q_PROPERTY( QString GpsTrackingPath              MEMBER       GpsTrackingPath       CONSTANT )
	Q_PROPERTY( QString GpsChangeAlarm               MEMBER       GpsChangeAlarm        CONSTANT )
	Q_PROPERTY( QString UseScreenCapture             MEMBER       UseScreenCapture        CONSTANT )
	Q_PROPERTY( QString GpsTimeBias             MEMBER       GpsTimeBias        CONSTANT )
	
	Q_PROPERTY( QString MapmatchingFeedback               MEMBER       MapmatchingFeedback        CONSTANT )
    Q_PROPERTY( QString MapmatchingDebugText               MEMBER       MapmatchingDebugText        CONSTANT )
	Q_PROPERTY( QString OnlyTunnelMMFB                    MEMBER       OnlyTunnelMMFB        CONSTANT )
	Q_PROPERTY( QString MMFBLog                           MEMBER       MMFBLog               CONSTANT )
	
	Q_PROPERTY( QString VmsTest             MEMBER       VmsTest        CONSTANT )
    Q_PROPERTY( QString EnableMonkeyTest             MEMBER       EnableMonkeyTest        CONSTANT )
    
    Q_PROPERTY( QString ExportGpslogToUSB             MEMBER       ExportGpslogToUSB        CONSTANT )
    
    Q_PROPERTY( QString Theme             MEMBER       Theme        CONSTANT )

    Q_PROPERTY( QString MClusterStart             MEMBER       MClusterStart        CONSTANT )
    Q_PROPERTY( QString MClusterStop              MEMBER       MClusterStop         CONSTANT )
    Q_PROPERTY( QString MClusterLayout            MEMBER       MClusterLayout       CONSTANT )

    
    Q_PROPERTY( QString GpsLog2Kml                MEMBER          GpsLog2Kml         		CONSTANT )
    Q_PROPERTY( QString MmfbGpsMethod                MEMBER       MmfbGpsMethod 	        CONSTANT )
    Q_PROPERTY( QString MmfbAngleOnTunnel                MEMBER   MmfbAngleOnTunnel 		CONSTANT )
    Q_PROPERTY( QString MmfbDrOnly                MEMBER   MmfbDrOnly 		CONSTANT )
    Q_PROPERTY( QString MmfbDrCep                  MEMBER   MmfbDrCep 		CONSTANT )
    Q_PROPERTY( QString MmfbDrCepRange                  MEMBER   MmfbDrCepRange 		CONSTANT )
    Q_PROPERTY( QString MmfbDrCepMMRange                  MEMBER   MmfbDrCepMMRange 		CONSTANT )

    Q_PROPERTY( QString CenterlMapInClusterTest                MEMBER   CenteralMapInClusterTest 		CONSTANT )

    Q_PROPERTY( QString GadgetShortcutIconTest  MEMBER GadgetShortcutIconTest CONSTANT)

public:

	enum NotificationK {
		ChangeGpsMapmatch = Constants::Test+500,
		ChangeGpsLogUsage,
		ChangeTnappServerIpPort,
		ChangeNddsServer,
        ChangeAvNextServer,
		ChangeSafeHelperServerIpPort,
		ApplyGpsTimeBias,
		CaptureScreenshot,
        ForceCrashApp,
        EnableDaynightTimeset,
        UpdateDaynightTimeset,
        ChangeTvasVersion,
        ChangeVsmLevelSettingUi,
        ChangeVsmDebugSettingUi,
        ApplyVsmLastDebugSetting,
        ChangeVsmServer,
        ChangeVsmAutoLevelMap,
        ChangeMapmatchingFeedback,
        ChangeMapmatchingDebugText,
        ChangeOnlyTunnelMMFB,
        ChangeMMFeedbackLog,
        ChangeMMFeedbackDrOnly,
        ChangeMMFeedbackDrCEP,
        ChangeMMFeedbackDrCEPrange,
        ChangeMMFeedbackDrCEPMMrange,
        ChangeVsmConfigId,
        ChangeVsmConfigVersion,
        ChangeVsmUseFps,
        ChangeVsmDisable3dObjects,
        ChangeVsmGLAntiAliasing,
		ChangeVsmFlickSpeedTest,
        DoExportLogsToUSB,
        ChangeTheme,
        MapInClusterStartStop,
        ChangeMapInClusterLayout,
        ChangeMMFeedbackGpsMethod,
        ChangeMMFeedbackAngleOnTunnel,
        ChangeGpsLogToKML,
        ForceToOnlineState,
        ChangeLanguage,
        DisplayMapInClusterInWindow,
	};

	enum SectionKeyIdK {
		K_TnappServerIp = 0,
		K_TnappServerPort,
		K_NddsServer,
        K_AvNextServer,
		K_NetworkLog,
		K_SafeHelperServerIp,
		K_SafeHelperServerPort,
		K_TmapLog,
		K_GpsMode,
		K_GpsLog,
		K_FixedRerouteTime,
		K_MapMatching,
		// K_RoadSignVoiceStreamingServer,
		K_CaptureScreenshot,
		K_TvasVersion,
		K_VmsTest,
//		K_GpsTimeBias,
		K_ForceCrash,
        K_MonkeyTest,
		K_ExportLogsToUSB,
		K_MultisenseTheme,
		K_MapInClusterStart,
		K_MapInClusterStop,
		K_MapInClusterLayout,
		K_GpsLog2KML,
		K_MMFeedbackGpsMethod,
		K_MMFeedbackAngleOnTunnel,
		K_MMFeedbackDrOnly,
		K_MMFeedbackDrCEP,
        K_MMFeedbackDrCEPrange,
        K_MMFeedbackDrCEPMMrange,
		K_ForceToOnlineState,
        K_ChangeLanguage,
        K_DisplayCenteralMapInClusterInWindow,
        K_DisplaySideMapInClusterInWindow,
		K_DisplayHudMapInClusterInWindow,
        K_DisplayGadgetShortcutIcon,
        K_FileName,
        K_DeleteFile,
        K_SdiLog,
	};
	Q_ENUM(SectionKeyIdK)

	enum VSMSectionKeyIdK {
		K_VsmLevelSettingUi = 0,
		K_VsmDebugSettingUi,
		K_VsmServer,
		K_VsmConfigId,
		K_VsmConfigVer,
		K_VsmAutoLevelMap,
		K_MapmatchingFeedback,
        K_MapmatchingDebugText,
		K_OnlyTunnelMMFB,
		K_MMFBLog,
        K_LocalMapmatching,
        K_FusedLocation,
        K_Crashlytics,
        K_ADBLog,
		K_CarConnectivityLog,
		K_HeadUnitEncryptionTransfer,
		K_GpsTrackingPath,
		K_GpsChangeAlarm,
		K_UseFps,
		K_Disable3dObjects,
		K_GLAntiAliasing,
		K_FlickSpeedTest,
	};
	Q_ENUM(VSMSectionKeyIdK)

	enum NddsServerTypeK {
		K_Commercial = 0,
		K_Trial,
        K_Development,
    };
	Q_ENUM(NddsServerTypeK)

	enum GpsModeK {
		K_LocationGps = 0,
		K_NMEAAGps,
		K_FileGps,
		K_MirrorLinkGps,
		K_MirrorLinkFileGps,
		K_MySpinGps,
		K_SwitchableGps,
		K_SwitchableFileGps,
	};
	Q_ENUM(GpsModeK)

	enum FixedRerouteTimeK {
		K_ByServer = 0,
		K_10Min,
		K_5Min,
		K_1Min,
		K_500MS,
	};
	Q_ENUM(FixedRerouteTimeK)

	enum TvasVersionK {
		K_Tvas_50 = 0,
        K_Tvas_51,
		K_Tvas_52,
		K_Tvas_53,
		K_Tvas_54,
	};
	Q_ENUM( TvasVersionK )


	enum CrashlyticsValueK {
		K_Crsh_Off = 0,
		K_Crsh_On,
		K_Crsh_Server,
	};
	Q_ENUM( CrashlyticsValueK )



    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new HiddenSettingConstants;
    }


    // db section names
	static const QString HiddenSetting;
	static const QString HiddenVsmSetting;

	// db key names 
	static const QString TnappServerIp;
	static const QString TnappServerPort;
	static const QString NddsServer;
    static const QString AvNextServer;
	static const QString NetworkLog;
	static const QString SafeHelperServerIp;
	static const QString SafeHelperServerPort;
	static const QString TmapLog;
	static const QString GpsMode;
	static const QString GpsLog;
	static const QString FixedRerouteTime;
	static const QString MapMatching;
	// static const QString RoadSignVoiceStreamingServer;
	static const QString TvasVersion;

    static const QString VsmLevelSettingUi;
	static const QString VsmDebugSettingUi;

	static const QString VsmServer;
	static const QString VsmConfigId; // debug feature
	static const QString VsmConfigVer; // debug feature
	static const QString VsmAutoLevelMap;
	static const QString VsmAutoLevelLowLevel;
	static const QString VsmAutoLevelMiddleLevel;
	static const QString VsmAutoLevelHighLevel;
	static const QString VsmAutoLevelLowTilt;
	static const QString VsmAutoLevelMiddleTilt;
	static const QString VsmAutoLevelHighTilt;

	static const QString VsmUseFps; // debug feature
    static const QString VsmDisable3dObjects; // debug feature, on/off building pattern, filtering building, skybox, land-mark
    static const QString VsmGLAntiAliasing;
    static const QString VsmFlickSpeedTest;
	
	// static const QString LocalMapmatching;
	// static const QString FusedLocation;
	// static const QString Crashlytics;
    static const QString ForceCrash;
	// static const QString ADBLog;
	// static const QString CarConnectivityLog;
	// static const QString HeadUnitEncryptionTransfer;
	static const QString GpsTrackingPath;
	static const QString GpsChangeAlarm;
	static const QString MapmatchingFeedback;
    static const QString MapmatchingDebugText;
	static const QString OnlyTunnelMMFB;
	static const QString MMFBLog;

	static const QString UseScreenCapture;
	static const QString VmsTest;
	static const QString GpsTimeBias;
    static const QString EnableMonkeyTest;
    static const QString LanguageTest;
    static const QString CenteralMapInClusterTest;
    static const QString SideMapInClusterTest;
	static const QString HudMapInClusterTest;    
    static const QString GadgetShortcutIconTest;
    static const QString ExportGpslogToUSB;
    static const QString SdiLog;
    
    static const QString Theme;

    static const QString MClusterStart;
    static const QString MClusterStop;
    static const QString MClusterLayout;

    static const QString GpsLog2Kml;
    static const QString MmfbGpsMethod;
    static const QString MmfbAngleOnTunnel;
    static const QString MmfbDrOnly;
    static const QString MmfbDrCep;
    static const QString MmfbDrCepRange;
    static const QString MmfbDrCepMMRange;

};

}

#endif //UI_BUSINESS_HIDDENSETTINGCONSTANTS_H_
