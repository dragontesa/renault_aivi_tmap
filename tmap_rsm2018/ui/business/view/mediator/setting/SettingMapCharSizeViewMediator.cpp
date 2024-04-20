#include "SettingMapCharSizeViewMediator.h"

#include "SettingDto.h"
#include "Constants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include <QJSValueIterator>
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "mediator/app/MapViewMode.h"
#include "RouteConstants.h"
#include "RgConstants.h"
#include "LangConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

SettingMapCharSizeViewMediator::SettingMapCharSizeViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
      mChanged(false)
{
}

QList<int> SettingMapCharSizeViewMediator::listNotificationInterests()
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

bool SettingMapCharSizeViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            //TMCRSM-563 - 주행중 지도 글씨 크기 관련 건
            sendNotification(RgConstants::BlockReroute, true);

            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            sendNotification(MapConstants::ShowCarvata, false);
            sendNotification(MapConstants::EnableCallout, false);

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingMapCharSizeView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);

            sendNotification(MapConstants::EnableCallout, false);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});

        //TMCRSM-563 - 주행중 지도 글씨 크기 관련 건
        sendNotification(MapConstants::EnableCallout, true);
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

            //TMCRSM-563 - 주행중 지도 글씨 크기 관련 건
            QVariantMap body;
            body.insert( "type", NavigationControllerProtocol::ApplyFontScale );
            sendNotification( NavigationControllerConstants::ApplyController, body );
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

    case ViewSizeChanged: {
            QJSValue jsVal = data.value<QJSValue>();
            int top = jsVal.property( CommonConstants::T ).toInt();
            QVariantMap body;
            body.insert(CommonConstants::T, top);
            sendNotification(MapConstants::UpdateMapViewPadding, body);
        }
        break;

    case UpdateView: {
            sendNotification(SettingConstants::UpdateView, data);
        }
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

bool SettingMapCharSizeViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
            QString section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::MapSubMapCharacter  ) {
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
    case SettingConstants::ChangedSetting: {
            mChanged = true;
        }
        break;
    case SettingConstants::SavedSetting: {
            if (mChanged) {
                mChanged = false;

                QVariantMap body;
                body.insert( "type", NavigationControllerProtocol::ApplyFontScale );
                sendNotification( NavigationControllerConstants::ApplyController, body );
            }
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMapCharSizeView))
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
