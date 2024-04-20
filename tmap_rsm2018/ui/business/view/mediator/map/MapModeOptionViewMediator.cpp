#include "MapModeOptionViewMediator.h"

#include "SettingDto.h"
#include "ApplicationConstants.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "ZoneConstants.h"
#include "HiddenSettingConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include <QJSValueIterator>
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "LangConstants.h"
#include "MNS_Define.h"
#include "PlatformHelper.h"

using namespace SKT;

MapModeOptionViewMediator::MapModeOptionViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> MapModeOptionViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseRouteOption,
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        ZoneConstants::FaderClicked,
        ZoneConstants::CswBackTriggered,
        NavigationControllerConstants::ChangeLanguage,
        SettingConstants::ResponseHiddenSetting,
        NavigationControllerConstants::ResponseController,
    };

    return notes;
}

bool MapModeOptionViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Mediated:
        {
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification(SettingConstants::RequestHiddenSetting, m);
        }
        break;

    case RequestSetting:
        {
            QJSValue jsVal = data.value<QJSValue>();
            QString section = jsVal.property( CommonConstants::Section ).toString();

            QVariantMap data;
            data.insert( CommonConstants::Section, section );

            sendNotification( SettingConstants::RequestSetting, data );
        }
        return true;
    case SaveSetting:
        {
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
        return true;

    case CancelRouteGuidance:
		{
            QVariantMap m;
            m["userCancel"] = true;
            sendNotification( RgConstants::StopRouteGuidance, m );
            sendNotification(ZoneConstants::SelectSoftkeyNormalDriveMode);// SelectSoftkeyNaviMode);
            sendNotification(RgConstants::BlockReroute, true);

            m.clear();
            m[CommonConstants::Position] = ToastsConstants::ToastUp;
            m[CommonConstants::Message] = tr("경로가 취소되었습니다.");
            sendNotification(ToastsConstants::ShowToast, m);
		}
        return true;

    case RequestRouteSummary:
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteSummaryView);
            m[CommonConstants::Caller] = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            m["routeOption"] = mRouteOption;
            sendNotification(Constants::CreateView, m);
            sendNotification(RouteConstants::RequestRouteSummaryDetail);
        }
        return true;

    case OtherRouteClicked:
        {
            int value = data.toInt();
            TeRouteOption option = E_ROUTEOPTION_OPTIMUM;
            switch (value) {
            case SettingConstants::OtherRouteTMapOptimalPath: // 최적
                option = E_ROUTEOPTION_OPTIMUM;
                break;
            case SettingConstants::OtherRouteMinimalTime:     // 최소
                option = E_ROUTEOPTION_SHORTTIME;
                break;
            case SettingConstants::OtherRouteFreeRoad:        // 무료
                option = E_ROUTEOPTION_FREEWAY;
                break;
            case SettingConstants::OtherRouteShortestDistance: // 최단
                option = E_ROUTEOPTION_SHORTESTWAY;
                break;
            case SettingConstants::OtherRouteHighwayPriority:  // 고속도로 우선
                option = E_ROUTEOPTION_EXPRESSWAY;
                break;
            case SettingConstants::OtherRouteBeginnerPass:     // 초보자
                option = E_ROUTEOPTION_EASY;
                break;

            default:
                break;
            }

            sendNotification(RgConstants::ChangeRouteOption, QVariant((int)option));
        }
        break;

    case Activated:
        sendNotification(SearchConstants::RequestRouteOption);
        sendNotification(ApplicationConstants::EnableBlur, true);
        PlatformHelper::instance()->showView(APP_POPUP_VIEW_NAME, "MapModeOptionViewMediator");
        return true;

    case Deactivated:
        PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
        sendNotification(ApplicationConstants::EnableBlur, false);
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        return true;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case RemoveCsw:
       sendNotification(NavigationControllerConstants::RemoveCSW, data);
       break;

    case CaptureScreen:
        sendNotification(HiddenSettingConstants::CaptureScreenshot);
        break;

    case RequestLang: {
            QVariantMap body;
            body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case ShowToast:
        sendNotification(ToastsConstants::ShowToast, data);
        break;

    case QueryOnlineStatus:
        sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        break;

    }
    return false;
}

bool MapModeOptionViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ResponseHiddenSetting:
        {
            auto m = note->getBody().toMap();
            auto section = m[CommonConstants::Section].toString();
            if (section == HiddenSettingConstants::HiddenSetting) {
                auto settings = m[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
                for (auto setting : settings) {
                    auto key = setting->key().toString();
                    if (key == HiddenSettingConstants::UseScreenCapture) {
                        auto enabled = setting->value().toBool();
                        QQmlProperty::write(mViewComponent, "captureEnable", enabled);
                        break;
                    }
                }
            }
        }
        break;

    case SearchConstants::ResponseRouteOption:
        {
            auto routeOption = note->getBody().toInt();
            mRouteOption = routeOption;
            switch(routeOption) {
                case E_ROUTEOPTION_OPTIMUM : 	routeOption = SettingConstants::OtherRouteTMapOptimalPath;  break;
                case E_ROUTEOPTION_EXPRESSWAY: 	routeOption = SettingConstants::OtherRouteHighwayPriority;  break;
                case E_ROUTEOPTION_FREEWAY:     routeOption = SettingConstants::OtherRouteFreeRoad;         break;
                case E_ROUTEOPTION_SHORTESTWAY: routeOption = SettingConstants::OtherRouteShortestDistance; break;
                case E_ROUTEOPTION_EASY:		routeOption = SettingConstants::OtherRouteBeginnerPass;     break;
                case E_ROUTEOPTION_SHORTTIME: 	routeOption = SettingConstants::OtherRouteMinimalTime;      break;
            }
            QMetaObject::invokeMethod(mViewComponent, "__setRouteOption",  Q_ARG(QVariant, QVariant::fromValue(routeOption)) );
        }
        break;

    case SettingConstants::ApplySetting:
        {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::DirectionGuid ) {
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
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Name)) {
                QString objName = data[CommonConstants::Name].toString();
                if (objName != objectName)
                    return false;
            }

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

    case ZoneConstants::FaderClicked:
        QMetaObject::invokeMethod(mViewComponent, "close");
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

    case ZoneConstants::CswBackTriggered:
        {
            auto activated = QQmlProperty::read(mViewComponent, "active").toBool();
            if (activated) {
                auto state = note->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    QMetaObject::invokeMethod(mViewComponent, "close");
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;

    default:
        return false;
    }
    return true;
}
