#ifndef SETTINGMOBILETMAPVIEWMEDIATOR_H
#define SETTINGMOBILETMAPVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "KeyboardKeyInputProxy.h"

namespace SKT {

class SettingMobileTmapViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( ErrorCodeK )
public:
    enum EventK {
        Activated,
        Deactivated,
        SelectZone1Title,
        CreateView,
        CloseView,
        RegistKeyboardInput,
        ShowKeyboard,
        HideKeyboard,
        SetKeyboardText,
        ModalChanged,
        CreateSessionByOTP,
        ClearSession,
        ClearSessionByProfileId,
        ResetCsw,
        RegistCsw,
        RequestAuthOTPstatus,
        RequestAuthOTPTryAvailable,
        RequestDeviceId,
        ClearText,
        GetSyncedProfileId,
        RequestLang,
        GetSyncedProfileName,
        GetCurrentProfileId,
        QueryOnlineStatus,
    };

    //NDDS_Define.h의 DS_AUTH_AUTHINFO의 nValidateCode 사용자 확인 결과(성공:2000,
    //실패:3001(사용자정보없음), 3002(사용자정보변경), 4001(authCode 확인 실패), 9001(black list model no 사용자))
    enum ErrorCodeK {
        NoUserInfo         = 3001,
        ChangeUserInfo     = 3002,
        FailAuthCodeCheck  = 4001,
        BlackList          = 9001,
        ExceedSignifDigits = -599,
    };

    Q_INVOKABLE SettingMobileTmapViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:
    KeyboardKeyInputProxy mKeyboardProxy;
    bool mShowKeyboard = false;
};

}

#endif // SETTINGMOBILETMAPVIEWMEDIATOR_H
