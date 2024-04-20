#include "HiddenBoschSettingViewMediator.h"
#include "HiddenSettingConstants.h"
#include "SettingConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "CommonConstants.h"
#include "SettingDto.h"
#include "VSM_Navi.h"
#include "NavigationControllerConstants.h"

namespace SKT {

HiddenBoschSettingViewMediator::HiddenBoschSettingViewMediator( QObject* viewComponent)
    : AbstractViewMediator( viewComponent )
{

}

QList<int> HiddenBoschSettingViewMediator::listNotificationInterests()
{
	static QList<int> interests {
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::DoSoftKeyFunction,
        KeyboardConstants::ProcessKeyboard,
        HiddenSettingConstants::UpdateDaynightTimeset,
//        KeyboardConstants::PullDownKeyboard,
//        KeyboardConstants::PullUpKeyboard,
    };

	return interests;
}

bool HiddenBoschSettingViewMediator::onEvent( int event, const QVariant& data )
{
	switch (event) {
	case Activated:
	    sendNotification(HiddenSettingConstants::EnableDaynightTimeset, true);
		break;
	case Deactivated:
	case SaveHiddenSetting:
	     sendNotification(HiddenSettingConstants::EnableDaynightTimeset, false);
	     saveSetting(data);
		break;
	case RequestHiddenSetting:
	     sendNotification(SettingConstants::RequestHiddenSetting, data);
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
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
	default:
		break;
	}
	return false;
}


bool HiddenBoschSettingViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
	switch (note->getId()) {
		case SettingConstants::ResponseHiddenSetting:
		{
			auto body = note->getBody().value<QVariantMap>();
			auto owner = reinterpret_cast<void*>(body[CommonConstants::Owner].toULongLong());
			if (owner != this) break;
			auto section = body[CommonConstants::Section].toString();
			auto settings = body[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
			QVariantMap data;
			for ( auto s : settings ) {
					// filter only bosch settings
				if (s->key().toString() == HiddenSettingConstants::MapMatching || 
						s->key().toString() == HiddenSettingConstants::UseScreenCapture ||
						s->key().toString() == HiddenSettingConstants::GpsTimeBias || 
						s->key().toString() == HiddenSettingConstants::MapmatchingFeedback ||
                        s->key().toString() == HiddenSettingConstants::MapmatchingDebugText ||
						s->key().toString() == HiddenSettingConstants::OnlyTunnelMMFB ||
						s->key().toString() == HiddenSettingConstants::MMFBLog ||
						s->key().toString() == HiddenSettingConstants::MmfbGpsMethod ||
						s->key().toString() == HiddenSettingConstants::MmfbAngleOnTunnel ||
                        s->key().toString() == HiddenSettingConstants::MmfbDrOnly ||
                        s->key().toString() == HiddenSettingConstants::MmfbDrCep ||
                        s->key().toString() == HiddenSettingConstants::MmfbDrCepRange ||
                        s->key().toString() == HiddenSettingConstants::MmfbDrCepMMRange
                        )  {
						data[s->key().toString()] =
					QVariantMap({
						{CommonConstants::Type,s->type()},
						{CommonConstants::Section,s->section()},
						{CommonConstants::Key,s->key()},
						{CommonConstants::Value,s->value()}
					});
			    }
		    }

            // ensure dr only on available
            QVariantMap gps = data[HiddenSettingConstants::MmfbGpsMethod].toMap();
            QVariantMap dronly = data[HiddenSettingConstants::MmfbDrOnly].toMap();
            QVariantMap angleTunnel = data[HiddenSettingConstants::MmfbAngleOnTunnel].toMap();
            int gpsDrOnly = gps[CommonConstants::Value].toInt();
            bool isDrOnly = (dronly[CommonConstants::Value].toBool() || gpsDrOnly == 2);
            if (isDrOnly) gpsDrOnly = 2;
            dronly[CommonConstants::Value] = isDrOnly;
            gps[CommonConstants::Value] = gpsDrOnly;
            angleTunnel[CommonConstants::Value] = isDrOnly;
            data[HiddenSettingConstants::MmfbDrOnly] = dronly;
            data[HiddenSettingConstants::MmfbGpsMethod] = gps;
            data[HiddenSettingConstants::MmfbAngleOnTunnel] = angleTunnel;

		QMetaObject::invokeMethod(mViewComponent, "responseSettings", Q_ARG(QVariant, QVariant::fromValue(data)));

		break;
	}
	case HiddenSettingConstants::UpdateDaynightTimeset: {
        const QList<QDateTime>& timeset = note->getBody().value<QList<QDateTime>>();
        QStringList fmts;
        for (auto d : timeset) {
            fmts << d.toString("yyyy-MM-dd hh:mm:ss:zzz");
        }
		QQmlProperty::write(mViewComponent, QStringLiteral("daynightTimeSet"),QVariant::fromValue(fmts));
	}
	break;
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
	case KeyboardConstants::ProcessKeyboard: {
		if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;

		auto args = note->getBody().toMap();
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
	break;
}
return false;
}

void HiddenBoschSettingViewMediator::applyKeySetting(const QVariant& data)
{
	auto m = data.toMap();
	auto key = m[CommonConstants::Index].toInt();
	auto value   = m[CommonConstants::Value];

	switch ( key ) {
    case HiddenSettingConstants::K_MapMatching:
		sendNotification(HiddenSettingConstants::ChangeGpsMapmatch,value);
		break;
    case HiddenSettingConstants::K_MapmatchingFeedback:
        sendNotification(HiddenSettingConstants::ChangeMapmatchingFeedback, value);
        // turn off all mmfb feature
        if (value.toBool() == false) {
            // save MMFB if it got enabled
            QVariantMap mmfb;
            mmfb[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            mmfb[CommonConstants::Key] = HiddenSettingConstants::OnlyTunnelMMFB;
            mmfb[CommonConstants::Value] = false;
            sendNotification(SettingConstants::SaveHiddenSetting, QVariant::fromValue(mmfb));
        }
        break;
    case HiddenSettingConstants::K_MapmatchingDebugText: {
            sendNotification(HiddenSettingConstants::ChangeMapmatchingDebugText, value);
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            m[CommonConstants::Key] = HiddenSettingConstants::MapmatchingDebugText;
            m[CommonConstants::Value] = value.toBool();
            sendNotification(SettingConstants::SaveHiddenSetting, QVariant::fromValue(m));
        }
        break;

    case HiddenSettingConstants::K_OnlyTunnelMMFB: {
        sendNotification(HiddenSettingConstants::ChangeOnlyTunnelMMFB, value);
        // save MMFB if it got enabled
        QVariantMap mmfb;
        mmfb[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
        mmfb[CommonConstants::Key] = HiddenSettingConstants::MapmatchingFeedback;
        mmfb[CommonConstants::Value] = value.toBool();
        sendNotification(SettingConstants::SaveHiddenSetting, QVariant::fromValue(mmfb));
    }
    	break;
    case HiddenSettingConstants::K_MMFBLog:
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackLog, value);
    	break;
    case HiddenSettingConstants::K_MMFeedbackDrOnly: {
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrOnly, value);
        // save Gps method and Tunnel angle
        int enabled = (int)value.toBool();
        QVariantMap mmfbGps;
        mmfbGps[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
        mmfbGps[CommonConstants::Key] = HiddenSettingConstants::MmfbGpsMethod;
        mmfbGps[CommonConstants::Value] = enabled ? 2 : 1; // 0:GPS(Sock), 1: GPS(DBus), 2:DR Only
        sendNotification(SettingConstants::SaveHiddenSetting, QVariant::fromValue(mmfbGps));

        QVariantMap mmfbTunnelAngle;
        mmfbTunnelAngle[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
        mmfbTunnelAngle[CommonConstants::Key] = HiddenSettingConstants::MmfbAngleOnTunnel;
        mmfbTunnelAngle[CommonConstants::Value] = 1*enabled; // 0: GPS Road line 1: DR 
        sendNotification(SettingConstants::SaveHiddenSetting, QVariant::fromValue(mmfbTunnelAngle));
    }
    break;
    case HiddenSettingConstants::K_MMFeedbackGpsMethod: {
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackGpsMethod, value);
        int gpsMethod = value.toInt();
        QVariantMap mmfbDrOnly;
        mmfbDrOnly[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
        mmfbDrOnly[CommonConstants::Key] = HiddenSettingConstants::MmfbDrOnly;
        mmfbDrOnly[CommonConstants::Value] = (gpsMethod == 2); // 0: GPS Road line 1: DR 
        sendNotification(SettingConstants::SaveHiddenSetting, QVariant::fromValue(mmfbDrOnly));       
        }
    	break;
    case HiddenSettingConstants::K_MMFeedbackAngleOnTunnel:
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackAngleOnTunnel, value);
    	break;
    case HiddenSettingConstants::K_MMFeedbackDrCEP:
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrCEP, value);
        break;        
    case HiddenSettingConstants::K_MMFeedbackDrCEPrange:
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrCEPrange, value);
        break;

    case HiddenSettingConstants::K_MMFeedbackDrCEPMMrange:
        sendNotification(HiddenSettingConstants::ChangeMMFeedbackDrCEPMMrange, value);
        break;

//    case HiddenSettingConstants::K_GpsTimeBias:
//         sendNotification(HiddenSettingConstants::ApplyGpsTimeBias, value);
//        break;
    case HiddenSettingConstants::K_CaptureScreenshot:
        break;
    case HiddenSettingConstants::K_ExportLogsToUSB:
        sendNotification(HiddenSettingConstants::DoExportLogsToUSB);
        return;
    case HiddenSettingConstants::K_GpsLog2KML:
        sendNotification(HiddenSettingConstants::ChangeGpsLogToKML, value);
        break;
    #ifdef TMAP_TMC_DEV_TESTING
    case HiddenSettingConstants::K_MapInClusterStart: {
        sendNotification(HiddenSettingConstants::MapInClusterStartStop,true);
        return;
    }
    break;
    case HiddenSettingConstants::K_MapInClusterStop: {
    	sendNotification(HiddenSettingConstants::MapInClusterStartStop,false);
        return;
    }
    case HiddenSettingConstants::K_MapInClusterLayout: {
    	sendNotification(HiddenSettingConstants::ChangeMapInClusterLayout,value);
        return;
    }
    case HiddenSettingConstants::K_ForceToOnlineState: {
    	sendNotification(HiddenSettingConstants::ForceToOnlineState);
        return;
    }
    break;
    #endif
	default:
		break;
	}

	sendNotification(SettingConstants::SaveHiddenSetting, data);

}

void HiddenBoschSettingViewMediator::saveSetting(const QVariant& data)
{
	auto m = data.toMap();
    auto section = m[CommonConstants::Section].toString();
    auto settings = m[CommonConstants::Settings].toMap();
    QMapIterator<QString,QVariant> it(settings);
    QList<SettingDtoPtr> sets;
    while (it.hasNext()) {
    	it.next();
    	auto dto = t_new_shared<SettingDto>(
    		0, // we don't use type, so it can get anything
    		section,
    		it.key(),
    		it.value()
    		);
    	sets.append(dto);
    }

     QVariantMap body;
     body[CommonConstants::Section] = section;
     body[CommonConstants::Settings] = QVariant::fromValue(sets);

     sendNotification(SettingConstants::SaveHiddenSetting, body);


    // apply keys for text types
     if (settings.contains(HiddenSettingConstants::GpsTimeBias)) {
     	int bias   = settings[HiddenSettingConstants::GpsTimeBias].toInt();
     		sendNotification(HiddenSettingConstants::ApplyGpsTimeBias, QVariant(bias));
     }
 }


} // namespace SKT
