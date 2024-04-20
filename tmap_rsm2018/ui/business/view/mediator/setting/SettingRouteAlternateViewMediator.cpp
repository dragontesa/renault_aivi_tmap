#include "SettingRouteAlternateViewMediator.h"
#include "SettingDto.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "ZoneConstants.h"
#include "CommonConstants.h"
#include "SettingConstants.h"
#include "ViewRegistry.h"
#include <QJSValueIterator>
#include "NavigationControllerConstants.h"
#include "RouteSummaryHelper.h"
#include "PlatformHelper.h"
#include "LangConstants.h"
#include "NavigationControllerProtocol.h"

using namespace SKT;

SettingRouteAlternateViewMediator::SettingRouteAlternateViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SettingRouteAlternateViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
    };

    return notes;
}

bool SettingRouteAlternateViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingRouteAlternateView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case ApplySetting:
        {
            QVariantList vlist = data.toList();
            QStringList list;
            for (QVariant v : vlist) {
                list.append(v.toString());
            }

            qSort(mRouteTypes);
            qSort(list);

            bool reRoute = mRouteTypes != list;
            if (reRoute) {
                close();
                sendNotification(Constants::ShowView, ViewRegistry::objectName(ViewRegistry::RouteCandidateView));

                sendNotification(RouteConstants::RequestRouteWithOtherOptions, QVariantMap{{"options",list}});
            } else {
                gotoBack();
            }
        }
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
            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();

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

            if (caller == ViewRegistry::objectName(ViewRegistry::RouteCandidateView)) {
                sendNotification(SettingConstants::ChangedOtherRouteSetting, body);
            } else {
                sendNotification( SettingConstants::SaveSetting, body );
            }
        }
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                PlatformHelper::instance()->setParentForSystemPopup(m);
            }
            sendNotification(Constants::CreateView, m);
        }
        break;
    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;
    case ShowView:
        sendNotification(Constants::ShowView, data);
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

bool SettingRouteAlternateViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting:
        {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
            QString section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::RouteAlternateSelection ) {
                auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
                QVariantMap value;

                mRouteTypes.clear();
                for ( auto setting : settings ) {
                    value[setting->key().toString()] =
                    QVariantMap({
                        {CommonConstants::Type,setting->type()},
                        {CommonConstants::Section,setting->section()},
                        {CommonConstants::Key,setting->key()},
                        {CommonConstants::Value,setting->value()}
                    });

                    if (setting->value().toInt()) {
                        mRouteTypes.append(setting->key().toString());
                    }
                }
                QMetaObject::invokeMethod(mViewComponent, "__updateModel", Q_ARG(QVariant, QVariant::fromValue(value)));
            }
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingRouteAlternateView))
                return false;

            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu) )
                objMenu = data[ZoneConstants::Menu].toString();

            QString theCaller = caller();
            if (theCaller == ViewRegistry::objectName(ViewRegistry::RouteCandidateView)) {
                if (objFunction == ZoneConstants::Back) {
                    int  objIndex  = -1;
                    if (data.contains(ZoneConstants::Index))
                        objIndex = data[ZoneConstants::Index].toInt();
                    QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
                }
            } else {
                int  objIndex  = -1;
                if (data.contains(ZoneConstants::Index))
                    objIndex = data[ZoneConstants::Index].toInt();
                QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );

            }
        }
        break;

    case SettingConstants::DialogAccepted: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SettingRouteAlternateView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SettingRouteAlternateView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, false));
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

QString SettingRouteAlternateViewMediator::caller() const
{
    return QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
}

void SettingRouteAlternateViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void SettingRouteAlternateViewMediator::gotoBack()
{
    QString theCaller = caller();

    sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, theCaller}});
    close();
}
