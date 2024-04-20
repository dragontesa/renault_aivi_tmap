#include "ManagePreferenceCommand.h"
#include "Constants.h"
#include "NavibookmarkAgentProxy.h"
#include "PreferenceProxy.h"
#include "PreferenceVo.h"
#include "PreferenceConstants.h"
#include "RgConstants.h"
#include "SettingConstants.h"
#include "DebugUtils.h"
#include "TStringUtils.h"
#include "DatabaseHelper.h"

using namespace SKT;

#define LAST_ROUTE_USE_DB

ManagePreferenceCommand::ManagePreferenceCommand( QObject* parent )
    : SimpleCommand( parent )
{

}

ManagePreferenceCommand::~ManagePreferenceCommand()
{
    //T_DEBUG( "I'm done..." );
}

bool ManagePreferenceCommand::execute( const mvc::INotificationPtr& note )
{
    auto proxy = Facade::instance()->retrieveProxy<PreferenceProxy>();

    switch (note->getId()) {
    case PreferenceConstants::AddPreferences: {
        auto data = note->getBody().value<QVariantMap>();
        auto prefsName = data[QStringLiteral("preferencesName")].toString();
        proxy->addPreferences( prefsName );
    }
    break;
    case PreferenceConstants::RemovePreferences: {
        auto data = note->getBody().value<QVariantMap>();
        auto prefsName = data[QStringLiteral("preferencesName")].toString();
        proxy->removePreferences( prefsName );
    }
    break;
    case PreferenceConstants::RequestPreference: {
        auto req = note->getBody().value<QVariantMap>();
        auto prefsName = req[QStringLiteral("preferencesName")].toString();
        auto section = req[CommonConstants::Section].toString();
        auto key = req[CommonConstants::Key].toString();

        QVariantMap result;
        result["request"] = req;

        auto isRgUseDb = false;

#ifdef LAST_ROUTE_USE_DB
        if (prefsName == PreferenceConstants::Rg) {
            DebugUtils::printMap("[MIGRATION] RequestPreference(DB)", req);
            auto isCloudDb = DatabaseHelper::instance()->isCloudDb();

            qDebug() << "[OTP] Request. isCloud: " << isCloudDb << ", section: " << section << ", key: " << key;

            if (!isCloudDb) {
                isRgUseDb = true;
            }
        }
#endif
        if (isRgUseDb) {
            auto naviBookmarkProxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();

            auto dto = naviBookmarkProxy->lastRouteInfo();
            if (key.isEmpty()) {
                QMap<QString,PreferenceVo> ret;
                if (section == RgConstants::Termination) {
                    setRgPreference(section, RgConstants::IsNormallyTerminated, dto->isNormallyTerminated(), ret);
                    setRgPreference(section, RgConstants::SelectedRouteIndex, dto->selectedRouteIndex(), ret);
                    setRgPreference(section, RgConstants::SelectedRouteOption, dto->selectedRouteOption(), ret);
                    setRgPreference(section, RgConstants::Address, dto->address(), ret);
                    auto base64 = dto->via().toString();
                    auto via = TStringUtils::stringListFromBase64(base64);
                    setRgPreference(section, RgConstants::Via, via, ret);
                } else if (section == RgConstants::PastPos) {
                    setRgPreference(section, RgConstants::PastPosX, dto->tpX(), ret);
                    setRgPreference(section, RgConstants::PastPosY, dto->tpY(), ret);
                    setRgPreference(section, RgConstants::PastSessionId, dto->sessionId(), ret);
                    setRgPreference(section, RgConstants::PastDate, dto->tpTime(), ret);
                }
                result["response"] = QVariant::fromValue(ret);
            } else {
                QVariant value;
                if (key == RgConstants::IsNormallyTerminated) {
                    value = dto->isNormallyTerminated();
                } else if (key == RgConstants::SelectedRouteIndex) {
                    value = dto->selectedRouteIndex();
                } else if (key == RgConstants::SelectedRouteOption) {
                    value = dto->selectedRouteOption();
                } else if (key == RgConstants::Address) {
                    value = dto->address();
                } else if (key == RgConstants::Via) {
                    auto base64 = dto->via().toString();
                    value = TStringUtils::stringListFromBase64(base64);
                } else if (key == RgConstants::ViewLevel) {
                    value = dto->mapLevel();
                } else if (key == RgConstants::PastPosX) {
                    value = dto->tpX();
                } else if (key == RgConstants::PastPosY) {
                    value = dto->tpY();
                } else if (key == RgConstants::PastSessionId) {
                    value = dto->sessionId();
                } else if (key == RgConstants::PastDate) {
                    value = dto->tpTime();
                }
                result["response"] = QVariant::fromValue(PreferenceVo{section, key, value});
            }
        } else {
            if ( !key.isEmpty() ) {
                // PreferenceVo
                result["response"] = QVariant::fromValue( proxy->preference( prefsName, section, key ) );
            }
            else {
                // QMap<QString,PreferenceVo>
                result["response"] = QVariant::fromValue( proxy->preferencesInSection( prefsName, section ) );
            }
        }
        sendNotification( PreferenceConstants::ApplyPreference, result );
    }
    break;
    case PreferenceConstants::SavePreference: {
        auto data = note->getBody().value<QVariantMap>();
        auto prefsName = data[QStringLiteral("preferencesName")].toString();
        auto section = data[CommonConstants::Section].toString();
        auto key = data[CommonConstants::Key].toString();
        auto value = data[CommonConstants::Value];

        auto isRgUseDb = false;

#ifdef LAST_ROUTE_USE_DB
        if (prefsName == PreferenceConstants::Rg) {
            auto isCloudDb = DatabaseHelper::instance()->isCloudDb();
            if (!isCloudDb) {
                isRgUseDb = true;
            }

            qDebug() << "[OTP] Save. isCloud: " << isCloudDb << ", section: " << section << ", key: " << key << ", value: " << value;
        }
#endif
        if (isRgUseDb) {
            auto naviBookmarkProxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();

            auto dto = naviBookmarkProxy->lastRouteInfo();

            if (section == RgConstants::PastPos) {
                if (key == RgConstants::PastPosData) {
                    auto m = value.toMap();
                    auto x = m[RgConstants::PastPosX].toInt();
                    auto y = m[RgConstants::PastPosY].toInt();
                    auto sessionId = m[RgConstants::PastSessionId].toString();
                    auto date = m[RgConstants::PastDate].toString();
                    auto update = (x != dto->tpX() ||
                                   y != dto->tpY() ||
                                   date != dto->tpTime() ||
                                   sessionId != dto->sessionId());
                    if (update) {
                        dto->setSessionId(sessionId);
                        dto->setTpTime(date);
                        dto->setTpX(x);
                        dto->setTpY(y);
                        naviBookmarkProxy->resetLastRouteInfo();
                        naviBookmarkProxy->setLastRouteInfo(dto);
                    }
                }
            } else {
                auto oldValue = QVariant{};
                if (key == RgConstants::IsNormallyTerminated) {
                    oldValue = dto->isNormallyTerminated();
                    if (value != oldValue) {
                        dto->setIsNormallyTerninated(value);
                    }
                } else if (key == RgConstants::SelectedRouteIndex) {
                    oldValue = dto->selectedRouteIndex();
                    if (value != oldValue) {
                        dto->setSelectedRouteIndex(value);
                    }
                } else if (key == RgConstants::SelectedRouteOption) {
                    oldValue = dto->selectedRouteOption();
                    if (value != oldValue) {
                        dto->setSelectedRouteOption(value);
                    }
                } else if (key == RgConstants::Address) {
                    oldValue = dto->address();
                    if (value != oldValue) {
                        dto->setAddress(value);
                    }
                } else if (key == RgConstants::Via) {
                    oldValue = dto->via();
                    if (value != oldValue) {
                        auto base64 = TStringUtils::stringListToBase64(value.toStringList());
                        dto->setVia(base64);
                    }
                } else if (key == RgConstants::ViewLevel) {
                    oldValue = dto->mapLevel();
                    if (value != oldValue) {
                        dto->setMapLevel(value);
                    }
                }
                if (value != oldValue) {
                    naviBookmarkProxy->setLastRouteInfo(dto);

                    sendNotification(PreferenceConstants::ChangedPreference, QVariantMap{
                                         {"preferencesName", prefsName},
                                         {CommonConstants::Section, section},
                                         {CommonConstants::Key, key},
                                         {CommonConstants::Value, value}
                                     });
                } else {
                    qDebug() << oldValue << " == " << value;
                }
            }
        } else {
            auto oldPref = proxy->preference( prefsName, section, key );

            proxy->setPreference( prefsName, section, key, value );

            if ( value.toString() != oldPref.value().toString() ) {
                QVariantMap body;
                body.insert(QStringLiteral("preferencesName"), prefsName );
                body.insert(CommonConstants::Section, section );
                body.insert(CommonConstants::Key, key );
                body.insert(CommonConstants::Value, value );

                sendNotification( PreferenceConstants::ChangedPreference, body );
            }
        }
    }
    break;
    default:
        return false;
    }

    return true;
}

void ManagePreferenceCommand::setRgPreference(const QString &section, const QString &key, const QVariant &value, QMap<QString,PreferenceVo> &ret)
{
    ret[key] = PreferenceVo{section, key, value};
}
