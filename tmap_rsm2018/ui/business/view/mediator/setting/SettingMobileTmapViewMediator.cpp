#include "SettingMobileTmapViewMediator.h"

#include "SettingDto.h"
#include "Constants.h"
#include "ApplicationConstants.h"
#include "PreferenceConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "KeyboardConstants.h"
#include "ViewRegistry.h"

#include <QJSValueIterator>
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "LangConstants.h"

#include "PlatformHelper.h"

using namespace SKT;

#define MAX_OTP_INPUT_TEXT  7

SettingMobileTmapViewMediator::SettingMobileTmapViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    mKeyboardProxy.setMaxStringLength(MAX_OTP_INPUT_TEXT);
}

QList<int> SettingMobileTmapViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        KeyboardConstants::ProcessKeyboard,
        KeyboardConstants::SystemKeyboardPressed,
        KeyboardConstants::SystemKeyboardBackPressed,
        KeyboardConstants::SystemKeyboardFinished,
        NavigationControllerConstants::ResponseController,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,
        ZoneConstants::CswBackTriggered
    };

    return notes;
}

bool SettingMobileTmapViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingMobileTmapView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
#ifdef BUILD_TARGET
            QQmlProperty::write(mViewComponent, "keyboardHeight", SYSTEM_KEYBOARD_HEIGHT);
#endif
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;

    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                PlatformHelper::instance()->setParentForSystemPopup(m);
            }
            sendNotification(Constants::CreateView, m);
        }
        break;

    case ClearText:
#ifdef BUILD_TARGET
        // OTP 입력 화면에서 뒤로가기 할 때 키보드 버퍼를 클리어 해주어야 함.
        PlatformHelper::instance()->setSearchBarText("");
        mKeyboardProxy.clear();
#endif
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);
        // Target에서 키보드를 리셋.
        sendNotification(KeyboardConstants::SetText, "");
        break;

    case RegistKeyboardInput:
        sendNotification(KeyboardConstants::RegistKeyboardInput, data);
        break;
    case ShowKeyboard:
        {
            sendNotification(KeyboardConstants::ShowKeyboard, data);
            auto text = QQmlProperty::read(mViewComponent, "interoperateInputFieldText").toString();
            sendNotification(KeyboardConstants::SetText, text);
            mShowKeyboard = true;
        }
        break;

    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        mShowKeyboard = false;
        break;

    case SetKeyboardText:
        sendNotification(KeyboardConstants::SetText, data);
        mKeyboardProxy.setString(data.toString());
        break;

    case ModalChanged: {
            sendNotification(ZoneConstants::UpdateModal,data.toMap());
        }
        break;

    case CreateSessionByOTP: {
            sendNotification(ApplicationConstants::UserClickOTP);

            QJSValue jsVal = data.value<QJSValue>();
            QString otp = jsVal.property( CommonConstants::Value ).toString();
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::CreateSessionByOTP );
            body.insert(CommonConstants::Value, otp );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;

    case ClearSessionByProfileId: {
            QJSValue jsVal = data.value<QJSValue>();
            int id = jsVal.property( CommonConstants::Value ).toInt();
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ClearSessionByProfileId );
            body.insert(CommonConstants::Value, id );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;

    case GetSyncedProfileId: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetSyncedProfileId );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case GetCurrentProfileId: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetCurrentProfileId );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

     case ClearSession: {
            sendNotification(ApplicationConstants::UserClickOTP);

            // OTP 해제
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ClearSession );
            sendNotification( NavigationControllerConstants::ApplyController, body );

            sendNotification(PreferenceConstants::RemovePreferences, QVariantMap{
                                 {"preferencesName",PreferenceConstants::Rg}
                             });
            sendNotification(PreferenceConstants::AddPreferences, QVariantMap{
                                 {"preferencesName",PreferenceConstants::Rg}
                             });
        }
        break;

    case RequestAuthOTPstatus: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetAuthOTPstatus );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case RequestAuthOTPTryAvailable: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetAuthOTPTryAvailable );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case RequestDeviceId: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetDeviceId );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
         }
         break;

    case GetSyncedProfileName: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetSyncedProfileName );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case QueryOnlineStatus:
        sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        break;

    default:
        return false;
    }
    return true;
}

bool SettingMobileTmapViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMobileTmapView))
                return false;

            if (mShowKeyboard) {

            } else {
                QString objFunction = CommonConstants::Empty;
                QString objMenu = CommonConstants::Empty;
                if (data.contains(ZoneConstants::Function))
                    objFunction = data[ZoneConstants::Function].toString();
                if (data.contains(ZoneConstants::Menu) )
                    objMenu = data[ZoneConstants::Menu].toString();
                int  objIndex  = -1;
                if (data.contains(ZoneConstants::Index))
                    objIndex = data[ZoneConstants::Index].toInt();
                QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
            }
        }
        break;

    case KeyboardConstants::ProcessKeyboard: {
#ifdef QT_DEBUG
             QVariantMap args = note->getBody().value<QVariantMap>();
             QString owner = args[CommonConstants::Owner].toString();
             if (owner != ViewRegistry::objectName(ViewRegistry::SettingMobileTmapView))
                 return false;
             auto key = args[CommonConstants::Key];
             auto text = args[CommonConstants::Text].toString();

             if (text.length() > MAX_OTP_INPUT_TEXT) {
                 text = text.left(MAX_OTP_INPUT_TEXT);
                 sendNotification(KeyboardConstants::SetText, text);
             }
             QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
#endif
         }
         break;
#ifdef BUILD_TARGET
    case KeyboardConstants::SystemKeyboardPressed:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toString();
            mKeyboardProxy.keyPressed(key);
            auto text = mKeyboardProxy.string();
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
    case KeyboardConstants::SystemKeyboardFinished:
    case KeyboardConstants::SystemKeyboardBackPressed:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toInt();
            auto text = m["text"].toString();
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
#endif

    case ZoneConstants::CswBackTriggered:
        {
            auto activated = QQmlProperty::read(mViewComponent, "active").toBool();
            if (activated) {
                auto state = note->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    auto key = -3;
                    auto text = QString{};
                    QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;

    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(SettingConstants::SessionCreated)) {
                bool success = data[SettingConstants::SessionCreated].toBool();
                int validateCode = data[CommonConstants::Code].toInt();
                int errCode = data[CommonConstants::Error].toInt();
                QMetaObject::invokeMethod( mViewComponent, "__sessionCreated", Q_ARG(QVariant, QVariant::fromValue(success)), Q_ARG(QVariant, QVariant::fromValue(validateCode)), Q_ARG(QVariant, QVariant::fromValue(errCode)) );
            } else if(data.contains(SettingConstants::AuthOTPStatus)) {
                bool auth = data[SettingConstants::AuthOTPStatus].toBool();
                QMetaObject::invokeMethod( mViewComponent, "__authOTPstatus", Q_ARG(QVariant, QVariant::fromValue(auth)) );
            } else if(data.contains(SettingConstants::DeviceId)) {
                QString deviceId = data[SettingConstants::DeviceId].toString();
                QMetaObject::invokeMethod( mViewComponent, "__deviceId", Q_ARG(QVariant, QVariant::fromValue(deviceId)) );
            } else if(data.contains(SettingConstants::AuthOTPTryAvailable)) {
                bool tryAuth = data[SettingConstants::AuthOTPTryAvailable].toBool();
                QMetaObject::invokeMethod( mViewComponent, "__authOTPTryAvailable", Q_ARG(QVariant, QVariant::fromValue(tryAuth)) );
            } else if(data.contains(SettingConstants::SyncedProfileId)) {
                int profileId = data[SettingConstants::SyncedProfileId].toInt();
                QMetaObject::invokeMethod( mViewComponent, "__syncProfileId", Q_ARG(QVariant, QVariant::fromValue(profileId)) );
            } else if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            } else if(data.contains(SettingConstants::SyncedProfileName)) {
                QString profileName = data[SettingConstants::SyncedProfileName].toString();
                QMetaObject::invokeMethod( mViewComponent, "__syncProfileName", Q_ARG(QVariant, QVariant::fromValue(profileName)) );
            } else if(data.contains(SettingConstants::CurrentProfileId)) {
                int profileId= data[SettingConstants::CurrentProfileId].toInt();
                QMetaObject::invokeMethod( mViewComponent, "__currentProfileId", Q_ARG(QVariant, QVariant::fromValue(profileId)) );
            }
        }
        break;

    case SettingConstants::DialogAccepted: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SettingMobileTmapView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SettingMobileTmapView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, false));
        }
        break;

    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;
    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:   {
            bool isOnline = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedOnlineStatus", Q_ARG(QVariant, isOnline));
        }
        break;

    default:
        return false;
    }
    return true;
}
