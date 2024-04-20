/*
 * HiddenMenuLoginViewMediator.cpp
 *
 *  Created on: 2017. 12. 12.
 */

#include "HiddenMenuLoginViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "KeyboardConstants.h"
#include "ZoneConstants.h"
#include "ViewRegistry.h"
#include "MapConstants.h"
#include "PlatformHelper.h"

namespace SKT {

HiddenMenuLoginViewMediator::HiddenMenuLoginViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{

}

QList<int> HiddenMenuLoginViewMediator::listNotificationInterests()
{
	static QList<int> notes = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
		KeyboardConstants::ProcessKeyboard,
        KeyboardConstants::SystemKeyboardPressed,
        KeyboardConstants::SystemKeyboardFinished,
        KeyboardConstants::SystemKeyboardBackPressed,
	};
	return notes;
}

bool HiddenMenuLoginViewMediator::onEvent( int event, const QVariant& data )
{

	switch (event) {
	case Activated:
		break;
	case Deactivated:
		sendNotification(KeyboardConstants::HideKeyboard, data);
		break;
	case CreateView:
	{
		auto m   = data.toMap();
		auto pwd = m.value(QStringLiteral("pwd")).toString();
		if (pwd == QStringLiteral("11")) { // 862740364
			// okay open the hidden general setting
			QVariantMap body;
			body[CommonConstants::Url] = m.value(CommonConstants::Url).toString();
			sendNotification(Constants::CreateView, body);
            // close previous setting view, navi menu
            auto objName = ViewRegistry::objectName(ViewRegistry::SettingMainView);
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, objName}});
            objName = ViewRegistry::objectName(ViewRegistry::NaviMenuView);
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, objName}});

            // restore mapview size
            QVariantMap body2;
            body2.insert(QStringLiteral("t"),  337);
            sendNotification(MapConstants::UpdateMapViewPadding, body2);
		}
	}
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

    case ClearText:
#ifdef BUILD_TARGET
        PlatformHelper::instance()->setSearchBarText("");
#endif
        break;

	default:
		break;
	}

	return false;
}


bool HiddenMenuLoginViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
	switch (note->getId()) {
        case ZoneConstants::DoSoftKeyFunction:
        case ZoneConstants::DoSoftkeyIndexMenuFunction: {
            if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool())
                break;
			auto args = note->getBody().value<QVariantMap>();
            auto objectName = args[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::HiddenMenuLoginView))
                return false;

			QString objFunction = CommonConstants::Empty;
			QString objMenu = CommonConstants::Empty;
			if (args.contains(ZoneConstants::Function))
				objFunction = args[ZoneConstants::Function].toString();
			if (args.contains(ZoneConstants::Menu) )
				objMenu = args[ZoneConstants::Menu].toString();
            int  objIndex  = -1;
            if (args.contains(ZoneConstants::Index))
                objIndex = args[ZoneConstants::Index].toInt();
            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
        }
			break;
		case KeyboardConstants::ProcessKeyboard: {
#ifdef QT_DEBUG
			if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;
			auto args = note->getBody().value<QVariantMap>();
			auto key = args[CommonConstants::Key];
			auto text = args[CommonConstants::Text];
			QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
#endif
		}
			break;
#ifdef BUILD_TARGET
        case KeyboardConstants::SystemKeyboardPressed: {
            auto m = note->getBody().toMap();
            auto key = m["key"].toString();
            mKeyboardProxy.keyPressed(key);
            auto text = mKeyboardProxy.string();
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
            break;
        }

        case KeyboardConstants::SystemKeyboardBackPressed:
        case KeyboardConstants::SystemKeyboardFinished: {
            auto m = note->getBody().toMap();
            auto key = m["key"].toInt();
            auto text = m["text"].toString();
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
            break;
        }
#endif
		default:
		break;
	}
	return false;
}

} /* namespace SKT */
