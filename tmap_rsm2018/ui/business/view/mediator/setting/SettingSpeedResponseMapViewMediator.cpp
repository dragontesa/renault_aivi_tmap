#include "SettingSpeedResponseMapViewMediator.h"

#include "SettingDto.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include <QJSValueIterator>
#include "VSM_Navi.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

SettingSpeedResponseMapViewMediator::SettingSpeedResponseMapViewMediator( QObject* viewComponent )
    : AbstractViewMediator(  viewComponent )
{
    mMapUse = false;
    mMapUseChanged = false;
    mDistance = 0;
    mDistanceChanged = false;
}

QList<int> SettingSpeedResponseMapViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SettingConstants::ChangedSetting,
        SettingConstants::SavedSetting,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,

    };

    return notes;
}

bool SettingSpeedResponseMapViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingSpeedResponseMapView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case RequestSetting: {
            QJSValue jsVal = data.value<QJSValue>();
            QString section = jsVal.property( CommonConstants::Section ).toString();

            QVariantMap data;
            data.insert( CommonConstants::Section, section );

            sendNotification( SettingConstants::RequestSetting, data );
        }
        break;
    case SaveSetting: {
            QJSValue jsVal = data.value<QJSValue>();
            QString section = jsVal.property( CommonConstants::Section ).toString();
            auto setting = jsVal.property( CommonConstants::Settings );

            QList<SettingDtoPtr> settings;
            QJSValueIterator it(setting);
            while (it.hasNext()) {
                it.next();
                QString key = it.name();
                QJSValue value = it.value();
                auto dto = t_new_shared<SettingDto>( value.property(CommonConstants::Type).toInt(),
                    section,
                    value.property(CommonConstants::Key).toString(),
                    value.property(CommonConstants::Value).toString()
                );
                settings.append( dto );
            }

            QVariantMap body;
            body.insert( CommonConstants::Section, section );
            body.insert( CommonConstants::Settings, QVariant::fromValue( settings ) );

            sendNotification( SettingConstants::SaveSetting, body );
        }
        break;
    case CloseView:
        sendNotification(Constants::CloseView, data);
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

    default:
        return false;
    }
    return true;
}

bool SettingSpeedResponseMapViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
            QString section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::MapSubSpeedResponse  ) {
                auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();

                QVariantMap value;
                for ( auto setting : settings ) {
                    if (setting->key() == SettingConstants::SpeedResponseMapUse)
                        mMapUse = setting->value().toInt();
                    else if (setting->key() == SettingConstants::Distance)
                        mDistance = setting->value().toInt();

                    value[setting->key().toString()] =
                    QVariantMap({
                        {CommonConstants::Type,setting->type()},
                        {CommonConstants::Section,setting->section()},
                        {CommonConstants::Key,setting->key()},
                        {CommonConstants::Value,setting->value()}
                    });
                }

                QQmlProperty::write( mViewComponent, CommonConstants::Settings, QVariant::fromValue( value ) );
            }
        }
        break;
    case SettingConstants::ChangedSetting: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString sect = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if (sect == SettingConstants::MapSubSpeedResponse) {
                if (key == SettingConstants::SpeedResponseMapUse) {
                    int value = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    T_DEBUG( QString( "SpeedResponseMapUse '%1'" ).arg( value ) );
                    mMapUse = value;
                    mMapUseChanged = true;
                } else if (key == SettingConstants::Distance) {
                    int value = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    T_DEBUG( QString( "Distance '%1'" ).arg( value ) );
                    mDistance = value;
                    mDistanceChanged = true;
                }
            }
        }
        break;
    case SettingConstants::SavedSetting: {
//            if (!mMapUse) // AutoZoom을 사용하지 않더라도 Engine에 설정값이 전될 되어야 함. // 추후 주석 제거.
//                break;
            if (!mMapUseChanged && !mDistanceChanged)
                break;
            mMapUseChanged = false;
            mDistanceChanged = false;

            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ApplySpeedLevel );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingSpeedResponseMapView))
                return false;

            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu))
                objMenu = data[ZoneConstants::Menu].toString();
            int  objIndex  = -1;
            if (data.contains(ZoneConstants::Index))
                objIndex = data[ZoneConstants::Index].toInt();
            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
        }
        break;
    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;
    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
        break;
    default:
        return false;
    }
    return true;
}




