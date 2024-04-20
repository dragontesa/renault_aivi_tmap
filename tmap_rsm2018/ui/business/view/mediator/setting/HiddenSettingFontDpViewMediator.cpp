#include "HiddenSettingFontDpViewMediator.h"

#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "KeyboardConstants.h"
#include "ViewRegistry.h"
#include "ResourceResolver.h"

#include <QJSValueIterator>
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"

using namespace SKT;

HiddenSettingFontDpViewMediator::HiddenSettingFontDpViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> HiddenSettingFontDpViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        KeyboardConstants::ProcessKeyboard,
//        KeyboardConstants::PullDownKeyboard,
//        KeyboardConstants::PullUpKeyboard,
    };

    return notes;
}

bool HiddenSettingFontDpViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::HiddenSettingFontDpView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;
    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;
    case RegistKeyboardInput:
        sendNotification(KeyboardConstants::RegistKeyboardInput, data);
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;
    case ChangeFontDp: {
            QJSValue jsVal = data.value<QJSValue>();
            auto dpi = jsVal.property( CommonConstants::Value ).toInt();
            ResourceResolver::instance()->setLogicalDpi(dpi);
        }
        break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    default:
        return false;
    }
    return true;
}

bool HiddenSettingFontDpViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::HiddenSettingFontDpView))
                return false;

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

    case KeyboardConstants::ProcessKeyboard: {
             QVariantMap args = note->getBody().value<QVariantMap>();
             QString owner = args[CommonConstants::Owner].toString();
             if (owner != ViewRegistry::objectName(ViewRegistry::HiddenSettingFontDpView))
                 return false;
             auto key = args[CommonConstants::Key];
             auto text = args[CommonConstants::Text];
             QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
         }
         break;

    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;


    default:
        return false;
    }
    return true;
}
