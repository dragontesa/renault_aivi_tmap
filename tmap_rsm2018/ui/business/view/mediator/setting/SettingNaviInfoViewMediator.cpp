#include "SettingNaviInfoViewMediator.h"

#include "SettingDto.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include <QJSValueIterator>
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

SettingNaviInfoViewMediator::SettingNaviInfoViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SettingNaviInfoViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        NavigationControllerConstants::ResponseController,
        NavigationControllerConstants::ChangeLanguage,
    };

    return notes;
}

bool SettingNaviInfoViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingNaviInfoView));
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
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetAppVersion );
            sendNotification( NavigationControllerConstants::RequestController, body );
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
    case CreateView:
        sendNotification(Constants::CreateView, data);
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

bool SettingNaviInfoViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        break;
    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(SettingConstants::AppVersion)) {
                QString appVersion = data[SettingConstants::AppVersion].toString();
                QString mapVersion = data[SettingConstants::MapVersion].toString();
                QString sdiVersion = data[SettingConstants::SDIVersion].toString();
                QString localNetworkVersion = data[SettingConstants::LocalNetworkVersion].toString();
                QString localPoiVersion = data[SettingConstants::LocalPoiVersion].toString();
                QString localMapVersion = data[SettingConstants::LocalMapVersion].toString();
                QVariantList result;
                for(int i=0; i < ResultEnd; i++) {
                    result.append(CommonConstants::Empty);
                }
                //프로그램 버전
                result.insert(ResultAppVersion,          appVersion);
                //지도 버전
                result.insert(ResultMapVersion,          mapVersion);
                //내장 지도 버전
                result.insert(ResultLocalMapVersion,     localMapVersion);
                //내장 전국 도로 버전
                result.insert(ResultLocalNetworkVersion, localNetworkVersion);
                //내장 명칭 데이타 버전
                result.insert(ResultLocalPoiVersion,     localPoiVersion);
                //안전 운전 버전
                result.insert(ResultSDIVersion,          sdiVersion);
                //지도성과심업체
                result.insert(ResultCompany,        CommonConstants::Empty);
                //디버깅
                result.insert(ResultDebuging,       CommonConstants::Empty);
                QQmlProperty::write( mViewComponent, CommonConstants::Settings, QVariant::fromValue( result ) );
            } else if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingNaviInfoView))
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
    default:
        return false;
    }
    return true;
}
