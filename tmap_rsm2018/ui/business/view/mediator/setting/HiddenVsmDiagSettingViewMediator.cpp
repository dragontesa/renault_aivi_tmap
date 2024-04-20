#include "HiddenVsmDiagSettingViewMediator.h"
#include "HiddenSettingConstants.h"
#include "SettingConstants.h"
#include "VSM_Navi.h"
#include "VSM_Navi_RgData.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "SettingDto.h"


namespace SKT {

HiddenVsmDiagSettingViewMediator::HiddenVsmDiagSettingViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> HiddenVsmDiagSettingViewMediator::listNotificationInterests()
{
	static QList<int> interests {
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::DoSoftKeyFunction,
        KeyboardConstants::ProcessKeyboard,
        KeyboardConstants::SystemKeyboardPressed,
        KeyboardConstants::SystemKeyboardBackPressed,
        KeyboardConstants::SystemKeyboardFinished
    };

	return interests;
}


bool HiddenVsmDiagSettingViewMediator::onEvent( int event, const QVariant& data )
{
	switch (event) {
	case Activated: 
		break;
	case Deactivated:
	sendNotification(SettingConstants::SaveHiddenSetting, data);
		break;
	case RequestVsmHiddenSetting: {
	     sendNotification(SettingConstants::RequestHiddenSetting, data);
	 }
		break;
	case ApplyKeySetting:
	applyKeySetting(data);
	   break;
	case ShowKeyboard:
	sendNotification(KeyboardConstants::ShowKeyboard, data);
	   break;
	case HideKeyboard:
	sendNotification(KeyboardConstants::HideKeyboard, data);
	   break;
	case RegistKeyboardInput:
	sendNotification(KeyboardConstants::RegistKeyboardInput, data);
	   break;
    case SetText:
        {
            auto text = data.toString();
            mKeyboardProxy.setString(text);
        }
        break;
	default:
        break;
	}
	return false;
}


void HiddenVsmDiagSettingViewMediator::applyKeySetting(const QVariant& data)
{

	auto m = data.toMap();
	auto keyId = m[CommonConstants::Index].toInt();
	auto value = m[CommonConstants::Value];

    switch ( keyId ) {
	case HiddenSettingConstants::K_VsmLevelSettingUi:
        sendNotification(HiddenSettingConstants::ChangeVsmLevelSettingUi, value);
	    break;
    case HiddenSettingConstants::K_VsmDebugSettingUi:
	    sendNotification(HiddenSettingConstants::ChangeVsmDebugSettingUi, value);
		break;
	case HiddenSettingConstants::K_VsmServer:
	    sendNotification(HiddenSettingConstants::ChangeVsmServer, value);
		break;
    case HiddenSettingConstants::K_VsmAutoLevelMap:
            sendNotification(HiddenSettingConstants::ChangeVsmAutoLevelMap, value);
	    // it is gonig to save at the noti handler
	    return;
	case HiddenSettingConstants::K_VsmConfigId:
        sendNotification(HiddenSettingConstants::ChangeVsmConfigId, value);
		break;
	case HiddenSettingConstants::K_VsmConfigVer:
         sendNotification(HiddenSettingConstants::ChangeVsmConfigVersion, value);
		break;
	case HiddenSettingConstants::K_UseFps:
	     sendNotification(HiddenSettingConstants::ChangeVsmUseFps, value);
	break;
	case HiddenSettingConstants::K_Disable3dObjects:
         sendNotification(HiddenSettingConstants::ChangeVsmDisable3dObjects, value);
		 break;
    case HiddenSettingConstants::K_GLAntiAliasing:
         sendNotification(HiddenSettingConstants::ChangeVsmGLAntiAliasing, value);
    break;
    case HiddenSettingConstants::K_FlickSpeedTest:
	     sendNotification(HiddenSettingConstants::ChangeVsmFlickSpeedTest, value);
	break;
	case HiddenSettingConstants::K_LocalMapmatching:
		break;
	case HiddenSettingConstants::K_FusedLocation:
		break;
	case HiddenSettingConstants::K_Crashlytics:
		break;
	case HiddenSettingConstants::K_ADBLog:
		break;
	case HiddenSettingConstants::K_CarConnectivityLog:
		break;
	case HiddenSettingConstants::K_HeadUnitEncryptionTransfer:
		break;
	case HiddenSettingConstants::K_GpsTrackingPath:
		break;
	case HiddenSettingConstants::K_GpsChangeAlarm:
		break;
	default:
		break;
    }

    sendNotification(SettingConstants::SaveHiddenSetting, data);
}


bool HiddenVsmDiagSettingViewMediator::handleNotification( const mvc::INotificationPtr& note )
{

	switch (note->getId()) {
		case SettingConstants::ResponseHiddenSetting:
		{
			auto body = note->getBody().value<QVariantMap>();
			auto owner = reinterpret_cast<void*>(body[CommonConstants::Owner].toULongLong());
			if (owner != this) break;
			auto section = body[CommonConstants::Section].toString();
			
			if (section != HiddenSettingConstants::HiddenVsmSetting) break;

			auto settings = body[CommonConstants::Settings].value<QList<SettingDtoPtr>>();

			// apply to VSM with auto level config saved 
            float lowLevel  = 11.0f;
            float midLevel  = 10.5f;
            float highLevel = 10.0f;
            float lowTilt   = 40.0f;
            float midTilt   = 25.0f;
            float highTilt  = 25.0f;

			QVariantMap data;
			for ( auto s : settings ) {
				auto key = s->key().toString();
                auto value = s->value();
				data[key] =
				QVariantMap({
					{CommonConstants::Type,s->type()},
					{CommonConstants::Section,s->section()},
					{CommonConstants::Key,s->key()},
					{CommonConstants::Value,s->value()}
				});

				if (key == HiddenSettingConstants::VsmAutoLevelLowLevel)
					{ lowLevel = value.toFloat(); continue; }
				if (key == HiddenSettingConstants::VsmAutoLevelMiddleLevel)
					{ midLevel = value.toFloat(); continue; }
				if (key == HiddenSettingConstants::VsmAutoLevelHighLevel)
					{ highLevel = value.toFloat(); continue; }
				if (key == HiddenSettingConstants::VsmAutoLevelLowTilt)
					{ lowTilt = value.toFloat(); continue; }
				if (key == HiddenSettingConstants::VsmAutoLevelMiddleTilt)
					{ midTilt = value.toFloat(); continue; }
				if (key == HiddenSettingConstants::VsmAutoLevelHighTilt)
					{ highTilt = value.toFloat(); continue; }
			}

			QMetaObject::invokeMethod(mViewComponent, "responseSettings", Q_ARG(QVariant, QVariant::fromValue(data)));

			QVariantMap mapAutoLevelConfig;
			mapAutoLevelConfig[HiddenSettingConstants::VsmAutoLevelLowLevel]     = lowLevel;
			mapAutoLevelConfig[HiddenSettingConstants::VsmAutoLevelMiddleLevel]  = midLevel;
			mapAutoLevelConfig[HiddenSettingConstants::VsmAutoLevelHighLevel]    = highLevel;
			mapAutoLevelConfig[HiddenSettingConstants::VsmAutoLevelLowTilt]      = lowTilt;
			mapAutoLevelConfig[HiddenSettingConstants::VsmAutoLevelMiddleTilt]   = midTilt;
			mapAutoLevelConfig[HiddenSettingConstants::VsmAutoLevelHighTilt]     = highTilt;
			QMetaObject::invokeMethod(mViewComponent, "setDriveAutoMapConfig", Q_ARG(QVariant, QVariant::fromValue(mapAutoLevelConfig)));

		  break;
		}
		case ZoneConstants::DoSoftKeyFunction:{
			auto body = note->getBody().toMap();
			if (body.contains(QStringLiteral("function"))) {
				QString fn = body[QStringLiteral("function")].toString();
				if (fn == QStringLiteral("back")) {
					QMetaObject::invokeMethod(mViewComponent, "close");
				}

			}
		}
		break;
#ifdef QT_DEBUG
		case KeyboardConstants::ProcessKeyboard: {
			if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;

            auto args = note->getBody().toMap();
			auto key = args[CommonConstants::Key];
            auto text = args[CommonConstants::Text];
			QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
		}
		break;
#endif

#ifdef BUILD_TARGET
    case KeyboardConstants::SystemKeyboardPressed:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toString();

            auto updated = mKeyboardProxy.keyPressed(key);
            auto text = mKeyboardProxy.string();
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
    case KeyboardConstants::SystemKeyboardBackPressed:
    case KeyboardConstants::SystemKeyboardFinished:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toInt();
            auto text = m["text"].toString();
            if (note->getId() == KeyboardConstants::SystemKeyboardFinished) {
                text = mKeyboardProxy.string();
            }
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
#endif
		default:
		  break;
	}

	return false;
}

void HiddenVsmDiagSettingViewMediator::saveSetting(const QVariant& data)
{

}

} // namespace SKT
