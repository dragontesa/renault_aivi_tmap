#ifndef SETTINGMAPUPDATEVIEWMEDIATOR_H
#define SETTINGMAPUPDATEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "../common/updatemanagerinterface.h"
#include "SettingNaviInfoViewMediator.h"

namespace SKT {

class SettingMapUpdateViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( ResultK )
    Q_ENUMS( PackageStatus )
    Q_ENUMS( ErrorType )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        SelectZone1Title,
        BlockSoftkeyMenu,
        CreateView,
        CloseView,
        ModalChanged,
        ShowToast,
        RequestSetting,
        RequestPackageStatus,
        RequestNeedRestart,
        RequestStartUpdate,
        RequestStopUpdate,
        RequestApplyUpdate,
        RequestCancelUpdate,
        HideKeyboard,
        ResetCsw,
        RegistCsw,
        RequestExit,
        Back,
        RequestPopupInCompleteMapDataCopy,
        RequestIgnoreMapUpdate,
        RequestViewList,
        RequestLang,
        RequestDlgAccepted,
        RequestDlgCanceled,
    };

    enum ResultK {
        ResultAppVersion,
        ResultMapVersion,
        ResultLocalMapVersion,
        ResultLocalNetworkVersion,
        ResultLocalPoiVersion,
        ResultSDIVersion,
        ResultCompany,
        ResultDebuging,
        ResultEnd,
    };

    enum PackageStatus {
        PackageStatusNone           = UpdateManagerInterface::PackageStatusNone,
        PackageStatusNewer          = UpdateManagerInterface::PackageStatusNewer,
        PackageStatusSame           = UpdateManagerInterface::PackageStatusSame,
        PackageStatusOlder          = UpdateManagerInterface::PackageStatusOlder,
        PackageStatusError          = UpdateManagerInterface::PackageStatusError,
    };

    enum ErrorType {
        ErrorTypeNone               = UpdateManagerInterface::ErrorTypeNone,
        ErrorTypeFileRead           = UpdateManagerInterface::ErrorTypeFileRead,
        ErrorTypeFileWrite          = UpdateManagerInterface::ErrorTypeFileWrite,
        ErrorTypeFileIntegrity      = UpdateManagerInterface::ErrorTypeFileIntegrity,
        ErrorTypeMediaUnmounted     = UpdateManagerInterface::ErrorTypeMediaUnmounted,
        ErrorTypeUnknown            = UpdateManagerInterface::ErrorTypeUnknown,
    };

    enum MessageK {
        Zone1Title = 0,
        UpdateButton,
        MapUpdateButton,
        NotiTitle,
        NoMapData,
        MapDataError,
        IncompleteMapData,
        IncompatibleMapData,
        ErrorMapData,
        MapDataComplete,
        CopyMapData1,
        CopyMapData2,
        SameVersion,
        OldVersion,
        CopyComplete,
        CopyError,
        Unmounted,
        CompatibilityError,
        NoMapInfo1st,
        NoMapInfo2nd,
        IncompatibleMapInfo1st,
        IncompatibleMapInfo2nd,
        VersionInfo1st,
        VersionInfo2nd,
        NewerInfo1st,
        NewerInfo2nd,
        CopyInfo1st,
        CopyInfo2nd,
        CopyInfo2ndNoMapData,
        ErrorInfo1st,
        ErrorInfo2nd,
        ReadyInfo1st,
        ReadyInfo2nd,
        CompleteInfo1st,
        CompleteInfo2nd,
        Zone1TitleKo,
        NotiTitleKo,
        CopyMapData1Ko,
        SameVersionKo,
        OldVersionKo,
        CopyMapData2Ko,
        CopyCompleteKo,
        CopyErrorKo,
        UnmountedKo,
        CompatibilityErrorKo,
        InCompleteMapDataKo,
        MapDataCompleteKo,
    };

    enum UpdateStatusK {
        Init = 0,
        Mounted,
        RequestStatus,
        ResponseStatus,
        UpdateStart,
        UpdateProgress,
        UpdateEnd,
        UpdateReady,
        UpdateApply,
        UpdateComplete,
        Error,
        MessageBox
    };

    Q_INVOKABLE SettingMapUpdateViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );
};

}

#endif // SETTINGMAPUPDATEVIEWMEDIATOR_H
