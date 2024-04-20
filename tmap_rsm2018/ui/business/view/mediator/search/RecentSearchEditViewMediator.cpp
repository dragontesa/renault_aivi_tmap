#include "RecentSearchEditViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"

#include "SettingConstants.h"
#include "ZoneConstants.h"
#include "ToastsConstants.h"
#include <QJSValueIterator>
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "ViewRegistry.h"
#include "PlatformHelper.h"

using namespace SKT;

RecentSearchEditViewMediator::RecentSearchEditViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> RecentSearchEditViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        NavigationControllerConstants::ChangeLanguage
    };

    return notes;
}

bool RecentSearchEditViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );
    switch ( event ) {
    case Activated: {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                mDeleteAllMessage = m[CommonConstants::Message].toString();
                PlatformHelper::instance()->setParentForSystemPopup(m);
            }
            sendNotification(Constants::CreateView, m);
        }
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;
    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput);
        break;
    case FocusZone1TextInput:
        sendNotification(ZoneConstants::FocusZone1TextInput);
        break;
    case EditSoftkeyMenu:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        break;
    case DeleteRecentSearch:
    	sendNotification(SearchConstants::DeleteRecentSearch, data);
    	break;
    case ClearRecentSearch:
    	sendNotification(SearchConstants::ClearRecentSearch, data);
    	break;
    case ShowToast:
        sendNotification(ToastsConstants::ShowToast, data);
    	break;
    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        return true;
    case ResetCsw:
    	sendNotification(NavigationControllerConstants::ResetCSW);
    	break;
    case RegistCsw:
    	sendNotification(NavigationControllerConstants::RegistCSW, data);
    	break;
    case RemoveCsw:
    	sendNotification(NavigationControllerConstants::RemoveCSW, data);
    	break;
    case ShowZone3:
         //sendNotification(ZoneConstants::ShowZone3, ZoneConstants::Animation);
         break;
     case HideZone3:
         //sendNotification(ZoneConstants::HideZone3, ZoneConstants::Animation);
    default:
        return false;
    }
    return true;
}

bool RecentSearchEditViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::DialogAccepted:
        onDialogAccepted(note->getBody());
        break;

    case SettingConstants::DialogCanceled:
        onDialogCanceled(note->getBody());
        break;

    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            auto data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Name)) {
                auto objName = data[CommonConstants::Name].toString();
                if (objName != objectName)
                    return false;
            }

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
        break;
    case NavigationControllerConstants::ChangeLanguage:
            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        break;
    default:
        return false;
    }
    return true;
}

void RecentSearchEditViewMediator::onDialogAccepted(const QVariant &v)
{
    auto m = v.toMap();
    auto message = m[CommonConstants::Message].toString();
    // CommonDialogView를 생성할 때 전달했던 Message를 mDeleteAllMessage에 보존하고
    // 이를 비교하여 전체 삭제 메시지를 accept 했는지 확인한다.
    if (message == mDeleteAllMessage) {
        QMetaObject::invokeMethod(mViewComponent, "doneDeleteAllDlg", Q_ARG(QVariant, true));
    }
}

void RecentSearchEditViewMediator::onDialogCanceled(const QVariant &v)
{
    auto m = v.toMap();
    auto title = m[CommonConstants::Title].toString();
    auto message = m[CommonConstants::Message].toString();
    if (message == mDeleteAllMessage) {
        QMetaObject::invokeMethod(mViewComponent, "doneDeleteAllDlg", Q_ARG(QVariant, false));
    }
}
