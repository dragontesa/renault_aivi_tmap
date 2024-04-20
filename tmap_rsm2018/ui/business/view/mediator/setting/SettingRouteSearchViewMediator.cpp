#include "SettingRouteSearchViewMediator.h"

#include "SettingDto.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include <QJSValueIterator>

#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "NavigationControllerProtocol.h"
#include "ViewRegistry.h"

using namespace SKT;

SettingRouteSearchViewMediator::SettingRouteSearchViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SettingRouteSearchViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
    };

    return notes;
}

bool SettingRouteSearchViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingRouteSearchView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case CreateView:
        sendNotification( Constants::CreateView, data );
        break;
    case SelectZone1Title:
        sendNotification( ZoneConstants::SelectZone1Title, data);
        break;
    case RequestSetting:  {
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

bool SettingRouteSearchViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
        case SettingConstants::ApplySetting: {
                QVariantMap data = note->getBody().value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    break;
                QString section = data[CommonConstants::Section].toString();
                if ( section == CommonConstants::RouteSearch || section == SettingConstants::RouteAlternateSelection ) {
                    auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();

                    QVariantMap value;
                    for ( auto setting : settings ) {
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
        case ZoneConstants::DoSoftKeyFunction:
        case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingRouteSearchView))
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
