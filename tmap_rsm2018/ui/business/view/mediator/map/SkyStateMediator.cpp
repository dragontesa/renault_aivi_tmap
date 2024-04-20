#include "SkyStateMediator.h"

#include "NavigationControllerConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "skyconstants.h"
#include "ApplicationConstants.h"

namespace SKT
{

enum SkyState {
    SkySunset = SKT::SkyConstants::SkySunset,
    SkySunny = SKT::SkyConstants::SkySunny,
    SkySunrise = SKT::SkyConstants::SkySunrise,
    SkyNight = SKT::SkyConstants::SkyNight,
    SkyRain = SKT::SkyConstants::SkyRain,
    SkySnow = SKT::SkyConstants::SkySnow,
    SkyCluster = SKT::SkyConstants::SkyCluster
};

SkyStateMediator::SkyStateMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
    , mNightMode(0)
    , mSunState(NavigationController::SunDawn)
    , mWeatherState(NavigationController::WeatherSunny)
{

}

QList<int> SkyStateMediator::listNotificationInterests()
{
    return QList<int> {
        NavigationControllerConstants::ResponseSkyState,
        NavigationControllerConstants::RequestSkyUpdate,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile
    };
}

bool SkyStateMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case SettingConstants::ApplySetting: // 초기 설정
        {
            // skyState, weatherState 요청
            sendNotification(NavigationControllerConstants::RequestSkyState);

            auto m = note->getBody().toMap();
            parseNightMode(m);
        }
        break;

    case SettingConstants::ChangedSetting:
        {
            auto m = note->getBody().toMap();
            parseNightMode(m);
        }
        break;

    case NavigationControllerConstants::ResponseSkyState:
        {
            auto m = note->getBody().toMap();
            if (m.contains("sunState")) {
                mSunState = (NavigationController::SunState) m["sunState"].toInt();
            }
            if (m.contains("weatherState")) {
                mWeatherState = (NavigationController::WeatherState) m["weatherState"].toInt();
            }
        }
        break;

    case NavigationControllerConstants::RequestSkyUpdate:
        updateSkyState();
        break;

    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        {
            QVariantMap m;
            m[CommonConstants::Section] = SettingConstants::MapMain;
            m[CommonConstants::Key] = SettingConstants::NightMode;
            sendNotification( SettingConstants::RequestSetting, m);
        }
        break;

    default:
        break;
    }
    return true;
}

void SkyStateMediator::parseNightMode(const QVariantMap &m)
{
    auto section = m[CommonConstants::Section].toString();
    if (section == SettingConstants::MapMain) {
        auto key = m[CommonConstants::Key].toString();
        if (key == SettingConstants::NightMode) {
           mNightMode = m[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();

           updateSkyState();
        }
    }
}

void SkyStateMediator::printSkyState(int skyState)
{
    QString str;

    switch (mNightMode) {
    case 0: str = "Auto,"; break;
    case 1: str = "Day,"; break;
    case 2: str = "Night,"; break;
    }

    switch (mSunState) {
    case NavigationController::SunDawn:
        str += "SunDawn,";
        break;
    case NavigationController::SunDay:
        str += "SunDay,";
        break;
    case NavigationController::SunTwilight:
        str += "SunTwilight,";
        break;
    case NavigationController::SunNight:
        str += "SunNight,";
        break;
    }

    switch (mWeatherState) {
    case NavigationController::WeatherSunny:
        str += "Sunny";
        break;
    case NavigationController::WeatherRainy:
        str += "Rainy";
        break;
    case NavigationController::WeatherSnowy:
        str += "Snowy";
        break;
    }

    str += " = ";

    switch (skyState) {
    case SkyNight: str += "SkyNight"; break;
    case SkyRain: str += "SkyRain"; break;
    case SkySnow: str += "SkySnow"; break;
    case SkySunny: str += "SkySunny"; break;
    case SkySunrise: str += "SkySunrise"; break;
    case SkySunset: str += "SkySunset"; break;
    }

    qDebug() << "[Time][SkyStateMediator]skyState: " << str;
}

static int skyStateFromWeather(NavigationController::WeatherState weather)
{
    switch (weather)
    {
    case NavigationController::WeatherSunny:
        return SkySunny;
    case NavigationController::WeatherRainy:
        return SkyRain;
    case NavigationController::WeatherSnowy:
        return SkySnow;
    default:
        return SkySunny;
    }
}

static int skyStateFromSunAndWeather(NavigationController::SunState sun, NavigationController::WeatherState weather)
{
    switch (sun) {
    case NavigationController::SunDawn:
        return SkySunrise;

    case NavigationController::SunDay:
        return skyStateFromWeather(weather);

    case NavigationController::SunTwilight:
        return SkySunset;

    case NavigationController::SunNight:
        return SkyNight;
    }
}

void SkyStateMediator::updateSkyState()
{
    int skyState = SkySunny;
    bool nightMode = false;

    switch (mNightMode) {
    case 0:
        // 자동
        skyState = skyStateFromSunAndWeather(mSunState, mWeatherState);
        nightMode = mSunState == NavigationController::SunNight;
        break;
    case 1:
        // 주간
        skyState = skyStateFromWeather(mWeatherState);
        break;
    case 2:
        // 야간
        skyState = SkyNight;
        nightMode = true;
        break;
    }

#ifdef QT_DEBUG
    printSkyState(skyState);
#endif

    QVariantMap m;
    m["skyState"] = skyState;
    m["nightMode"] = nightMode;

    qDebug() << "[Time][SkyStateMediator]skyState: " << skyState << " nightMode:" << nightMode;

    sendNotification(NavigationControllerConstants::UpdateSkyState, m);
}

}
