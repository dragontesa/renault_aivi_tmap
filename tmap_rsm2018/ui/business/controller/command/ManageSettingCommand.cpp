#include "ManageSettingCommand.h"
#include "SettingDto.h"
#include "RecentDestinationDto.h"
#include "RecentSearchDto.h"
#include "FavoriteDto.h"
#include "SettingProxy.h"
#include "Constants.h"
#include "SettingConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerProxy.h"
#include "UserProfileConstants.h"
#include "HiddenSettingConstants.h"
#include "PreferenceConstants.h"
#include "DatabaseHelper.h"
#include "LangConstants.h"
#include "ApplicationConstants.h"
#include "RgConstants.h"
#include <QDir>

using namespace SKT;

ManageSettingCommand::ManageSettingCommand( QObject* parent )
    : SimpleCommand( parent )
{

}

bool ManageSettingCommand::execute( const mvc::INotificationPtr& note )
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();

    switch ( note->getId() ) {
    case SettingConstants::RequestSetting:
        {
            auto owner = note->getSource().data();
            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();
            if ( data.contains(CommonConstants::Key)) {
                auto key = data[CommonConstants::Key].toString();
                auto setting = proxy->setting( section, key );
                QVariantMap body;
                body.insert(CommonConstants::Section, section );
                body.insert(CommonConstants::Key, key );
                body.insert(CommonConstants::Setting, QVariant::fromValue( setting ) );

                if ( data.contains(CommonConstants::Var) ) {
                    auto var = data[CommonConstants::Var].toString();
                    body.insert(CommonConstants::Var, var);
                }

                if ( data.contains(CommonConstants::Slot) ) {
                    auto slot = data[CommonConstants::Slot].toString();
                    body.insert(CommonConstants::Slot, slot);
                }

                int respNotiId = SettingConstants::ApplySetting;
                if ( data.contains(CommonConstants::ResponseNotificationId)) {
                    respNotiId = data[CommonConstants::ResponseNotificationId].toInt();
                }
                body[CommonConstants::Owner] = QVariant(reinterpret_cast<qulonglong>(owner));
                sendNotification( respNotiId, body );
            }
            else {
                auto settings = proxy->settings( section );
                QVariantMap body;
                body.insert(CommonConstants::Section, section );
                body.insert(CommonConstants::Settings, QVariant::fromValue( settings ) );
                if (data.contains(CommonConstants::Var)) {
                    auto var = data[CommonConstants::Var].toString();
                    body.insert(CommonConstants::Var, var);
                }
                if ( data.contains(CommonConstants::Slot) ) {
                    auto slot = data[CommonConstants::Slot].toString();
                    body.insert(CommonConstants::Slot, slot);
                }

                int respNotiId = SettingConstants::ApplySetting;
                if ( data.contains(CommonConstants::ResponseNotificationId)) {
                    respNotiId = data[CommonConstants::ResponseNotificationId].toInt();
                }
                body[CommonConstants::Owner] = QVariant(reinterpret_cast<qulonglong>(owner)); // to prevent redundent listeners
                sendNotification( respNotiId, body );
            }
        }
        break;

    case SettingConstants::SaveSetting:  {
            auto data = note->getBody().value<QVariantMap>();
            saveSetting(data);
        }
        break;
     case SettingConstants::ResetToFactorySettings: {
            resetToFactorySettings();
        }
        break;
     case SettingConstants::InitSetting: {
            initSettings();
        }
        break;

    case SettingConstants::RequestSettingByProfileChange:
        {
            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            proxy->requestSetting();
        }
        break;
     // request hidden setting
    case SettingConstants::RequestHiddenSetting:
    {
        auto owner = note->getSource().data();
        auto data = note->getBody().value<QVariantMap>();
        auto section = data[CommonConstants::Section].toString();
        QVariantMap body;
        body[CommonConstants::Owner] = QVariant(reinterpret_cast<qulonglong>(owner)); // to prevent redundent listeners
        if ( data.contains(CommonConstants::Key)) {
            auto key = data[CommonConstants::Key].toString();
            auto setting = proxy->setting( section, key );
            QList<SettingDtoPtr> res;
            res.append(setting);
            body.insert(CommonConstants::Section, section );
            body.insert(CommonConstants::Key, key );
            body.insert(CommonConstants::Setting, QVariant::fromValue( res ) );
        }
        else {
            auto settings = proxy->settings( section );
            body.insert(CommonConstants::Section, section );
            body.insert(CommonConstants::Settings, QVariant::fromValue( settings ) );
        }
        sendNotification( SettingConstants::ResponseHiddenSetting, body );
    }
        break;
    case SettingConstants::SaveHiddenSetting:
    {
        auto data = note->getBody().value<QVariantMap>();
        saveHiddenSetting(data);
    }
    break;
    default:
        return false;
    }

    return true;
}

void  ManageSettingCommand::saveSetting(const QVariantMap &data)
{
        auto section = data[CommonConstants::Section].toString();
        auto key = data[CommonConstants::Key].toString();
        auto value = data[CommonConstants::Value];
        auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();

        auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
        QList<QVariantMap> list;

        // set single value
        if ( !key.isEmpty() && !value.isNull() ) {
            auto setting = proxy->setting( section, key );
            setting->setValue( value );
            settings.append( setting );
        }

        for ( int i = 0; i< settings.count(); i++) {
            SettingDtoPtr p = settings.at(i);
            auto section = p->section().toString();
            auto key = p->key().toString();
            auto reqValue = p->value();

            auto setting = proxy->setting( section, key );
            auto resValue = setting->value();

            if (reqValue.toString() != resValue.toString()) {
                QVariantMap body;
                body.insert(CommonConstants::Section, section );
                body.insert(CommonConstants::Key, key );
                body.insert(CommonConstants::Value, QVariant::fromValue( reqValue ) );
                list.insert(list.count(), body);
            }
        }
        proxy->setSettings( section, settings );

        if (!list.isEmpty()) {
            QVariantMap body;

            for ( int i = 0; i<list.count(); i++) {
                body  = list.at(i);
                auto section = body[CommonConstants::Section].toString();
                auto key = body[CommonConstants::Key].toString();
                auto reqValue = body[CommonConstants::Value].toString();
                auto setting = proxy->setting( section, key );
                auto resValue = setting->value().toString();

                if (reqValue != resValue)
                    continue;
                body.remove(CommonConstants::Value);
                body.insert(CommonConstants::Setting, QVariant::fromValue( setting ) );
                sendNotification( SettingConstants::ChangedSetting, body );

#ifdef SYNC_FOR_USER_SETTINGS
                // notify what user data modified
                QVariantMap body2;
                body2[CommonConstants::Type] = UserProfileConstants::Preference;
                body2[CommonConstants::Key] = UserProfileConstants::PreferenceK;
                sendNotification(NavigationControllerConstants::ModifiedUserData,body2);
#endif
            }
        }

        if ( data.contains(CommonConstants::Key ) ) {
            auto key = data[CommonConstants::Key].toString();
            auto setting = proxy->setting( section, key );
            QVariantMap body;
            body.insert(CommonConstants::Section, section );
            body.insert(CommonConstants::Key, key );
            body.insert(CommonConstants::Setting, QVariant::fromValue( setting ) );
            sendNotification( SettingConstants::SavedSetting, body );
        }
        else {
            auto settings = proxy->settings( section );
            QVariantMap body;
            body.insert(CommonConstants::Section, section );
            body.insert(CommonConstants::Settings, QVariant::fromValue( settings ) );
            sendNotification( SettingConstants::SavedSetting, body );
        }
}


void ManageSettingCommand::saveHiddenSetting(const QVariantMap& data)
{
    auto section = data[CommonConstants::Section].toString();
    auto key = data[CommonConstants::Key].toString();
    auto value = data[CommonConstants::Value];

    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();

        // save single value
    if ( !key.isEmpty() && !value.isNull() ) {
        auto setting = proxy->setting( section, key );
        setting->setValue( value );
        proxy->setSetting( section, setting );
    }
    else if ( data.contains(CommonConstants::Settings) ) {
        auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
        proxy->setSettings( section, settings );
    }
}


void  ManageSettingCommand::initDefaultSettings()
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    QHash<QString,QHash<QString,QHash<int,QVariant>>> hash_i = proxy->getItems();
     QHash<QString,QHash<QString,QHash<int,QVariant>>>::const_iterator i = hash_i.constBegin();
     while (i != hash_i.constEnd()) {
        QHash<QString,QHash<int,QVariant>> hash_j = i.value();
        QHash<QString,QHash<int,QVariant>>::const_iterator j = hash_j.constBegin();
        while (j != hash_j.constEnd()) {
            QHash<int,QVariant> hash_k = j.value();
            QHash<int,QVariant> ::const_iterator k = hash_k.constBegin();
            while (k != hash_k.constEnd()) {
                QList<SettingDtoPtr> settings;
                QString section = i.key();
                QString key = j.key();
                QString value = k.value().toString();

                QVariantMap data;
                data[CommonConstants::Section] = section;
                data[CommonConstants::Key] = key;
                data[CommonConstants::Value] = value;
                data[CommonConstants::Settings] = QVariant::fromValue( settings );
                saveSetting(data);
                k++;
            }
            j++;
        }
        ++i;
     }
}

void ManageSettingCommand::resetAudioFile()
{
    QString audioPath = SettingConstants::RgDataPath;

    QDir directory = QDir(audioPath);
    QStringList delFileList;
    delFileList = directory.entryList(QStringList("*.wav"), QDir::Files|QDir::NoSymLinks);
    for(int i = 0 ; i < delFileList.length() ; i++){
          QString deleteFilePath = audioPath+ "/" + delFileList[i];
          QFile::remove(deleteFilePath);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//[환경설정 초기화시]
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//1.db파일을 지우지 않고 디폴트값으로  되돌림
//2.네비볼륨 초기화
//3.전기차인 경우 유종을 전기로 설정
//4.카바타를 디폴트로 vsm에 설정
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void ManageSettingCommand::initSettings()
{
    // TMCRSM-1223: 환경설정 초기화시 db파일을 지우던 코드 삭제, 환경설정db파일의 값만 바꾸도록 수정
    initDefaultSettings();

    initEtcSettings();
    resetAudioFile();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//[공장초기화시 처리사항]
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//1.캐시db,session db, 환경설정 db, 마지막 사용자 정보파일,/dynamic/databses/밑에 있는 모든 파일
//2.사용자 프로파일 DB삭제
//3.연결된 모든 세션 삭제
//4.네비볼륨 초기화
//5.전기차인 경우 유종을 전기로 설정
//6.카바타를 디폴트로 VSM에 설정
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void ManageSettingCommand::resetToFactorySettings()
{
    qDebug() << "ManageSettingCommand::resetToFactorySettings()";

    //경로 안내중이면 경로 취소
    sendNotification(RgConstants::StopRouteGuidance, QVariantMap{{"clearVia", true}});

    //동적으로 화면 갱신이 되지 않으므로 모든 창을 닫는다.
    sendNotification(ApplicationConstants::ResetNavi);

    // TMCRSM-1223: 환경설정 초기화시 db파일을 지우지 않고 공장초기화시만 지우도록 함
    DatabaseHelper::instance()->deleteAllUserDB();

    // fixed TMCRSM-2779, we don't gurentee all table consist so needs below 
	QSqlDatabase db = DatabaseHelper::instance()->connection();
	orm::TOrm::instance()->create<SettingDto>(db);
	orm::TOrm::instance()->create<RecentDestinationDto>(db);
	orm::TOrm::instance()->create<RecentSearchDto>(db);
	orm::TOrm::instance()->create<FavoriteDto>(db);

    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::RequestResetAllUserData);
    sendNotification( NavigationControllerConstants::ApplyController, body );

    initDefaultSettings();

    QVariantMap body3;
    body3.insert( CommonConstants::Type, NavigationControllerProtocol::ClearAllSession);
    sendNotification( NavigationControllerConstants::ApplyController, body3 );

    initEtcSettings();

    // init cavata resources, fix, TMCRSM-2260: OTP 연동 후 시스템 공장 초기화 진행시 카바타 아이콘 사라지는 현상 2018.12.14
   sendNotification(NavigationControllerConstants::UpdateCavatarIcon);
}

void ManageSettingCommand::initEtcSettings()
{
    QVariantMap body;
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyNaviVolume );
    body.insert( SettingConstants::NaviVolume, SettingConstants::DefaultVolume );
    sendNotification( NavigationControllerConstants::ApplyController, body );

    body.clear();
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyAutoFuelType);
    body.insert( SettingConstants::FuelType, SettingConstants::FUEL_ELECTRICITY );
    sendNotification( NavigationControllerConstants::ApplyController, body );    


    body.clear();
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyFontScale );
    sendNotification( NavigationControllerConstants::ApplyController, body );

    body.clear();
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
    sendNotification( NavigationControllerConstants::ApplyController, body );

    body.clear();
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyVoiceGuidance );
    sendNotification( NavigationControllerConstants::ApplyController, body );

    body.clear();
    body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplyVoice );
    sendNotification( NavigationControllerConstants::ApplyController, body );

    // noti, to set the cavatar side by VSM engine, by leeck
     sendNotification(NavigationControllerConstants::SetCavatarIcon, QVariant::fromValue(0));


    // request Map Auto Level Speed View mode
    QVariantMap autoLevelSetting;
    autoLevelSetting[CommonConstants::Section] = SettingConstants::MapSubSpeedResponse;
    autoLevelSetting[CommonConstants::Key] = SettingConstants::Distance;
    sendNotification( SettingConstants::RequestSetting, autoLevelSetting );

    sendNotification( SettingConstants::RequestHiddenSettingByReset);

    qDebug() << "[OTP] initEtcSetting";

    //Rg
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Rg },
         {CommonConstants::Section,QStringLiteral("Termination")},
         {CommonConstants::Key,QStringLiteral("isNormallyTerminated")},
         {CommonConstants::Value, true}
     });
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Rg },
         {CommonConstants::Section,QStringLiteral("Termination")},
         {CommonConstants::Key,QStringLiteral("selectedRouteOption")},
         {CommonConstants::Value, -1}
     });
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Rg },
         {CommonConstants::Section,QStringLiteral("Termination")},
         {CommonConstants::Key,QStringLiteral("via")},
         {CommonConstants::Value, ""}
     });


    //Test
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Test},
         {CommonConstants::Section,QStringLiteral("MonkeyTest")},
         {CommonConstants::Key,QStringLiteral("enable")},
         {CommonConstants::Value, false}
     });
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Test},
         {CommonConstants::Section,QStringLiteral("MonkeyTest")},
         {CommonConstants::Key,QStringLiteral("repeatsPerSecond")},
         {CommonConstants::Value, 10}
     });
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Test},
         {CommonConstants::Section,QStringLiteral("MonkeyTest")},
         {CommonConstants::Key,QStringLiteral("durationMins")},
         {CommonConstants::Value, 60 *3}
     });
    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
         {"preferencesName",PreferenceConstants::Test},
         {CommonConstants::Section,QStringLiteral("Language")},
         {CommonConstants::Key,QStringLiteral("lang")},
         {CommonConstants::Value, LangConstants::Ko}
     });

    // request monkey test
    sendNotification( PreferenceConstants::RequestPreference,
        QVariantMap{
            { CommonConstants::Caller, QStringLiteral("ApplicationMediator") },
            { QStringLiteral("preferencesName"), PreferenceConstants::Test },
            { CommonConstants::Section, QStringLiteral("MonkeyTest") }
        } );

    // host.ini파일 삭제
    QString hostIniPath = SettingConstants::PreferencesPath + QDir::separator() + QStringLiteral("host.ini");
    QFile file(hostIniPath);
    if (file.exists()) {
        if (!QFile::remove(hostIniPath) ) {
            T_DEBUG(QStringLiteral("failure to delete the use file :%1").arg(hostIniPath));
        }
    }
}
