#include "HiddenDiagSettingViewMediator.h"
#include "HiddenSettingConstants.h"
#include "SettingConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "CommonConstants.h"
#include "SettingDto.h"
#include "RouteConstants.h"
#include "VSM_Navi.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"
#include "LangConstants.h"
#include "MapConstants.h"
#include "../common/mapinclusterlayout.h"
#include "ViewRegistry.h"
#include "TestConstants.h"

#include <QFile>
#include <QFileInfo>

namespace SKT {
HiddenDiagSettingViewMediator::HiddenDiagSettingViewMediator( QObject* viewComponent)
    : AbstractViewMediator( viewComponent )
{

}

QList<int> HiddenDiagSettingViewMediator::listNotificationInterests()
{
	static QList<int> interests {
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::DoSoftKeyFunction,
        KeyboardConstants::ProcessKeyboard,
        PreferenceConstants::ApplyPreference,
        PreferenceConstants::ChangedPreference,
//        KeyboardConstants::PullDownKeyboard,
//        KeyboardConstants::PullUpKeyboard,
    };

	return interests;
}

QString HiddenDiagSettingViewMediator::sizeHuman(double size)
{
    float num = size;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

bool HiddenDiagSettingViewMediator::removeDir(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);
    if (dir.exists()) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot |
                                    QDir::System |
                                    QDir::Hidden  |
                                    QDir::AllDirs |
                                    QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }        
    }
    return result;
}

void HiddenDiagSettingViewMediator::deletFile(const QVariant& data)
{
    QVariantList list = data.toList();
    QFile file;
    for (int i=0; i< list.count(); i++) {
        QString item = list.at(i).toString();
        QFileInfo fileInfo(item);
        if (fileInfo.isDir()) {
            removeDir(item);
        } else {
            file.setFileName(item);
            if (file.exists()) {
                file.remove();
            }
        }
    }
}

double HiddenDiagSettingViewMediator::getDirSize(const QString& dirName)
{
    double totalFileSize = 0;
    QDir dir(dirName);
    if (!dir.exists())
        return totalFileSize;
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        totalFileSize += fileInfo.size();
    }
    return totalFileSize;
}

void HiddenDiagSettingViewMediator::getFileSize(const QVariant& data)
{
    QVariantList list = data.toList();
    QFile file;
    QString fileHsize;
    for (int i=0; i< list.count(); i++) {
        QString item = list.at(i).toString();
        QFileInfo fileInfo(item);
        if (fileInfo.isDir()) {
            double fileSize = getDirSize(item);
            fileHsize = sizeHuman(fileSize);
        } else {
            file.setFileName(item);
            if (file.exists()) {
                QFileInfo fileInfo(item);
                double fileSize = fileInfo.size();
                fileHsize = sizeHuman(fileSize);
            } else {
                fileHsize = sizeHuman(0);
            }
        }
        QMetaObject::invokeMethod(mViewComponent, "responseFile",
                                  Q_ARG(QVariant, QVariant::fromValue(item)),
                                  Q_ARG(QVariant, QVariant::fromValue(fileHsize)) );

    }

}

bool HiddenDiagSettingViewMediator::onEvent( int event, const QVariant& data )
{
	switch (event) {
	case Activated:
		break;
	case Deactivated:
	case SaveHiddenSetting:
	     saveSetting(data);
		break;
	case RequestHiddenSetting:
	     sendNotification(SettingConstants::RequestHiddenSetting, data);         
         sendNotification( PreferenceConstants::RequestPreference,
             QVariantMap{
                 { CommonConstants::Caller, QStringLiteral("HiddenDiagSettingViewMediator") },
                 { QStringLiteral("preferencesName"), PreferenceConstants::Test },
                 { CommonConstants::Section, QStringLiteral("MonkeyTest") }
             } );
         sendNotification( PreferenceConstants::RequestPreference,
             QVariantMap{
                 { CommonConstants::Caller, QStringLiteral("HiddenDiagSettingViewMediator") },
                 { QStringLiteral("preferencesName"), PreferenceConstants::Test },
                 { CommonConstants::Section, QStringLiteral("Language") }
             } );
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
	case ForceCrashApp:
	sendNotification(HiddenSettingConstants::ForceCrashApp);
	break;
    case DisplayCenteralMapInClusterInWindow:
    sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true},{QStringLiteral("layout"),(int)MapInClusterLayout::Layout::CenteralMap}});
    break;
    case DisplaySideMapInClusterInWindow:
    sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true},{QStringLiteral("layout"),(int)MapInClusterLayout::Layout::SideMap}});
    break;
    case DisplayHudMapInClusterInWindow:
        {
            //sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true},{QStringLiteral("layout"),(int)MapInClusterLayout::Layout::HudMap}});
            auto m = QVariantMap{
                {CommonConstants::Url,ViewRegistry::url(ViewRegistry::TestView)},
                {QStringLiteral("parentObjectName"),"testWrap"}
            };
            sendNotification(Constants::CreateView, m);
        }
        break;
    case DeleteFile:
    deletFile(data);
    getFileSize(data);
    break;
    case GetFileSize:
    getFileSize(data);
    break;
    case DisplayGadgetShortcutIcon: {
            QString objectName = ViewRegistry::objectName(ViewRegistry::HiddenDiagSettingView);
            QVariantMap data;
            data[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::GadgetShortcutIconView);
            data[CommonConstants::Caller] = objectName;
            data[CommonConstants::Owner ] = objectName;
            sendNotification(Constants::CreateView, data);
        }
        break;
    case OutputSdiLog: {
            QVariantMap body;
            body.insert( "enable", true );
            sendNotification( TestConstants::LogSdiData, body );
        }
        break;
	default:
		break;
	}
	return false;
}

bool HiddenDiagSettingViewMediator::handleNotification( const mvc::INotificationPtr& note )
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
				data[s->key().toString()] =
				QVariantMap({
					{CommonConstants::Type,s->type()},
					{CommonConstants::Section,s->section()},
					{CommonConstants::Key,s->key()},
					{CommonConstants::Value,s->value()}
				});
			}

			QMetaObject::invokeMethod(mViewComponent, "responseSettings", Q_ARG(QVariant, QVariant::fromValue(data)));

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
		case KeyboardConstants::ProcessKeyboard: {
			if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;

            auto args = note->getBody().toMap();
			auto key = args[CommonConstants::Key];
			auto text = args[CommonConstants::Text];
			QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
		}
		break;
        case PreferenceConstants::ApplyPreference: {
                auto result = note->getBody().value<QVariantMap>();
                auto req = result[QStringLiteral("request")].value<QVariantMap>();
                if ( req[CommonConstants::Caller].toString() == QStringLiteral("HiddenDiagSettingViewMediator") ) {
                    if ( req[CommonConstants::Section].toString() == QStringLiteral("MonkeyTest") ) {
                        auto resp = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                        auto enable = resp["enable"].value().toBool();
                        QMetaObject::invokeMethod(mViewComponent, "setKeySettingById",
                            Q_ARG(QVariant, HiddenSettingConstants::K_MonkeyTest), Q_ARG(QVariant, HiddenSettingConstants::EnableMonkeyTest), Q_ARG(QVariant, enable));
                    }
                    else if ( req[CommonConstants::Section].toString() == QStringLiteral("Language") ) {
                        auto resp = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                        auto lang = resp["lang"].value().toString();
                        QMetaObject::invokeMethod(mViewComponent, "setKeySettingById",
                            Q_ARG(QVariant, HiddenSettingConstants::K_ChangeLanguage), Q_ARG(QVariant, HiddenSettingConstants::LanguageTest), Q_ARG(QVariant, lang));
                    }
                }
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

void HiddenDiagSettingViewMediator::applyKeySetting(const QVariant& data)
{
	auto m = data.toMap();
	auto key = m[CommonConstants::Index].toInt();
	auto value   = m[CommonConstants::Value];

	switch ( key ) {
	case HiddenSettingConstants::K_TnappServerIp:
    case HiddenSettingConstants::K_TnappServerPort:
        sendNotification(HiddenSettingConstants::ChangeTnappServerIpPort,value);
	    break;
	case HiddenSettingConstants::K_NddsServer:
	    sendNotification(HiddenSettingConstants::ChangeNddsServer,value);
		break;
    case HiddenSettingConstants::K_AvNextServer:
        sendNotification(HiddenSettingConstants::ChangeAvNextServer,value);
        break;
	case HiddenSettingConstants::K_NetworkLog:
		break;
	case HiddenSettingConstants::K_SafeHelperServerIp:
	case HiddenSettingConstants::K_SafeHelperServerPort:
	     sendNotification(HiddenSettingConstants::ChangeSafeHelperServerIpPort,value);
		 break;
	case HiddenSettingConstants::K_TmapLog:
		break;
	case HiddenSettingConstants::K_GpsLog:
	     sendNotification(HiddenSettingConstants::ChangeGpsLogUsage, value);
		break;
	case HiddenSettingConstants::K_FixedRerouteTime: {
        float t[5] = { 0, 10, 5, 1, 0.5 };
		int  intervalTime = int(t[value.toInt()%5] * 60000);
	     sendNotification(RouteConstants::ChangeReRouteIntervalTime, QVariant::fromValue(intervalTime));
	 }
		break;
	case HiddenSettingConstants::K_MapMatching:
		sendNotification(HiddenSettingConstants::ChangeGpsMapmatch,value);
		break;
    case HiddenSettingConstants::K_CaptureScreenshot:
        break;
	case HiddenSettingConstants::K_TvasVersion:
	    sendNotification(HiddenSettingConstants::ChangeTvasVersion, value);
		break;
	case HiddenSettingConstants::K_VmsTest:
	    break;
//    case HiddenSettingConstants::K_GpsTimeBias:
//         sendNotification(HiddenSettingConstants::ApplyGpsTimeBias, value);
//        break;
    case HiddenSettingConstants::K_MonkeyTest:
        sendNotification(PreferenceConstants::SavePreference, QVariantMap{
             {"preferencesName",PreferenceConstants::Test},
             {CommonConstants::Section,QStringLiteral("MonkeyTest")},
             {CommonConstants::Key,QStringLiteral("enable")},
             {CommonConstants::Value,value.toBool()}
         });
         break;
    case HiddenSettingConstants::K_MultisenseTheme: {
    	int themes[4] = { 0x75e04b, 0x0091ff,  0xadb7c9, 0xe82e15};
    	int selected  = value.toInt();
    	sendNotification(HiddenSettingConstants::ChangeTheme, themes[selected]);
    }
	case HiddenSettingConstants::K_ForceCrash:	
	    break;
    case HiddenSettingConstants::K_ChangeLanguage: {
            sendNotification(PreferenceConstants::SavePreference, QVariantMap{
                 {"preferencesName",PreferenceConstants::Test},
                 {CommonConstants::Section,QStringLiteral("Language")},
                 {CommonConstants::Key,QStringLiteral("lang")},
                 {CommonConstants::Value,value}
             });
            sendNotification(HiddenSettingConstants::ChangeLanguage, value);
        }
        break;
    case HiddenSettingConstants::K_DisplayCenteralMapInClusterInWindow: {
            sendNotification(MapConstants::StartMapInCluster,
                QVariantMap{{QStringLiteral("asWindow"),true},
                {QStringLiteral("layout"),(int)MapInClusterLayout::Layout::CenteralMap}});
        }
        break;
    case HiddenSettingConstants::K_DisplaySideMapInClusterInWindow: {
            sendNotification(MapConstants::StartMapInCluster,
                QVariantMap{{QStringLiteral("asWindow"),true},
                {QStringLiteral("layout"),(int)MapInClusterLayout::Layout::SideMap}});
        }
        break;
    case HiddenSettingConstants::K_DisplayHudMapInClusterInWindow: {
            sendNotification(MapConstants::StartMapInCluster,
                QVariantMap{{QStringLiteral("asWindow"),true},
                {QStringLiteral("layout"),(int)MapInClusterLayout::Layout::HudMap}});
        }
        break;		
    case HiddenSettingConstants::K_SdiLog: {
            QVariantMap body;
            body.insert( "SdiLog", true );
            sendNotification( TestConstants::RunTest, body );
        }
        break;
	default:
		break;
	}

	sendNotification(SettingConstants::SaveHiddenSetting, data);

}

void HiddenDiagSettingViewMediator::saveSetting(const QVariant& data)
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

    if (settings.contains(HiddenSettingConstants::EnableMonkeyTest)) {
        auto value = settings[HiddenSettingConstants::EnableMonkeyTest].toBool();

        T_DEBUG( QString("EnableMonkeyTest: %1").arg(value) );

        sendNotification(PreferenceConstants::SavePreference, QVariantMap{
             {"preferencesName",PreferenceConstants::Test},
             {CommonConstants::Section,QStringLiteral("MonkeyTest")},
             {CommonConstants::Key,QStringLiteral("enable")},
             {CommonConstants::Value,value}
         });
    }

    if (settings.contains(HiddenSettingConstants::LanguageTest)) {
        auto value = settings[HiddenSettingConstants::LanguageTest].toString();

        T_DEBUG( QString("ChangeLanguage: %1").arg(value) );

        sendNotification(PreferenceConstants::SavePreference, QVariantMap{
             {"preferencesName",PreferenceConstants::Test},
             {CommonConstants::Section,QStringLiteral("Language")},
             {CommonConstants::Key,QStringLiteral("lang")},
             {CommonConstants::Value,value}
         });
    }

    if (settings.contains(HiddenSettingConstants::CenteralMapInClusterTest)) {
        T_DEBUG( QString("MapInClusterTest: #1") );
        sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true}});
    }

	QVariantMap body;
	body[CommonConstants::Section] = section;
	body[CommonConstants::Settings] = QVariant::fromValue(sets);

	sendNotification(SettingConstants::SaveHiddenSetting, body);

    // apply keys for text types
	if (settings.contains(HiddenSettingConstants::TnappServerIp) && 
		settings.contains(HiddenSettingConstants::TnappServerPort) ) {
		QString ip = settings[HiddenSettingConstants::TnappServerIp].toString();
		int     port = settings[HiddenSettingConstants::TnappServerPort].toInt();
		if (!ip.isEmpty() && port > 0) {
			QVariantMap m{ {QStringLiteral("ip"),ip}, {QStringLiteral("port"),port} };
            sendNotification(HiddenSettingConstants::ChangeTnappServerIpPort, m);
		}
    }

    if (settings.contains(HiddenSettingConstants::SafeHelperServerIp) &&
        settings.contains(HiddenSettingConstants::SafeHelperServerPort) ) {
        QString ip = settings[HiddenSettingConstants::SafeHelperServerIp].toString();
        int port   = settings[HiddenSettingConstants::SafeHelperServerPort].toInt();
        if (!ip.isEmpty() && port > 0) {
			QVariantMap m{ {QStringLiteral("ip"),ip}, {QStringLiteral("port"),port} };
            sendNotification(HiddenSettingConstants::ChangeSafeHelperServerIpPort, m);
		}
    }

    if (settings.contains(HiddenSettingConstants::GpsTimeBias)) {
        int bias   = settings[HiddenSettingConstants::GpsTimeBias].toInt();
            sendNotification(HiddenSettingConstants::ApplyGpsTimeBias, QVariant(bias));
    }
}


} // namespace SKT
