#ifndef APPLICATIONCONSTANTS_H
#define APPLICATIONCONSTANTS_H

#include "Constants.h"
#include "uilayout.h"

namespace SKT {

class ApplicationConstants {
	Q_GADGET
public:
    enum StateK {
        StateNone = 0,
        StateWithMapData,
        StateNoMapData,
    };
    Q_ENUM(StateK)

    enum LayoutK {
        L800x480 = UILayout::L800x480,
        L820x1024 = UILayout::L820x1024,
        L1920x720 = UILayout::L1920x720
    };
    Q_ENUM( LayoutK )

    enum ImageType {
        OnlineCrossImage = 0,
    };
	
    enum NotificationK {
        ChangeState = Constants::Application,
        RequestState,
        ReplyToState,
        ChangedState,
        UpdatedUserProfile,
        ChangedUserProfile,
        ChangedSession,
        UserClickOTP,
        ActivatedView,
        DeactivatedView,
        ShowGadget,
        RequestGadgetState,
        ReplyToGadgetState,
        ChangedGadgetState,
        RequestViewList,
        ReplyToViewList,
        RequestMode,
        ReplyToMode,
        ChangedMode,
        EnableBlur,
        ResetNavi,
        CloseAllViews,
        CreateSystemPopup,
        SystemPopupAccepted,
        SystemPopupCanceled,
        SystemPopupCreated,
        SystemPopupClosed,
        RequestCommonDialogExist,
        ReplyToCommonDialogExist,
        RequestCommonDialogAccepted,
        RequestCommonDailogCanceled,
        CloseCommonDialogForSystemPopup,
        CloseCommonDialogWithoutMapUpdate,
        ChangeToApp,
        ChangeToWidget,
        BlockSearchTabView,
        UpdateLastSesstionId,
        UpdateImageProvider,
        FocusToWidget,
    };

    ApplicationConstants();
	
    static const QString ObjectName;
    static const QString ViewId;
    static const QString RegId;
    static const QString Function;
    static const QString Hold;
    static const QString Release;
    static const QString ByTraits;
    static const QString ExceptFor;
    static const QString Traits;
    static const QString Visible;
    static const QString ViewIdList;
    static const QString ViewDataMap;
    static const QString Active;	

    static const int ScreenWidth = 820;
    static const int ScreenHeight = 1024;
};

}

#endif
