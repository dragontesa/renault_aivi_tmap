#include "SettingMainViewMediator.h"
#include "Constants.h"
#include "SettingDto.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "ToastsConstants.h"
#include <QJSValueIterator>
#include "SearchConstants.h"
#include "ViewRegistry.h"
#include "NavigationControllerConstants.h"
#include "MapConstants.h"
#include "mediator/app/MapViewMode.h"
#include "RouteConstants.h"
#include "LangConstants.h"
#include "PlatformHelper.h"
#include "NavigationControllerProtocol.h"
#include "ApplicationConstants.h"

using namespace SKT;

SettingMainViewMediator::SettingMainViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SettingMainViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SettingConstants::UpdateView,
        RouteConstants::Hitting,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
        ApplicationConstants::ReplyToViewList,
    };

    return notes;
}

bool SettingMainViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MenuNavi}});
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingMainView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                PlatformHelper::instance()->setParentForSystemPopup(m);
            }
            sendNotification( Constants::CreateView, m );
        }
        break;
    case UpdateView:
        sendNotification(SettingConstants::UpdateView, data);
        break;
    case RequestSetting: {
            QJSValue jsVal = data.value<QJSValue>();
            QString section = jsVal.property( CommonConstants::Section ).toString();

            QVariantMap body;
            body.insert( CommonConstants::Section, section );
            if (jsVal.hasOwnProperty(CommonConstants::Var)) {
                QString var = jsVal.property( CommonConstants::Var ).toString();
                body.insert( CommonConstants::Var, var);
            }
            if (jsVal.hasOwnProperty(CommonConstants::Slot)) {
                QString slot = jsVal.property( CommonConstants::Slot ).toString();
                body.insert( CommonConstants::Slot, slot);
            }
            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;
    case SaveSetting:  {
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
    case InitSetting:
        sendNotification( SettingConstants::InitSetting, data );
        break;
    case SelectZone1Title: {
            sendNotification(ZoneConstants::SelectZone1Title, data);
        }
        break;
    case ShowToast: {
            sendNotification(ToastsConstants::ShowToast, data);
        }
        break;
    case ModalChanged: {
            sendNotification(ZoneConstants::UpdateModal,data.toMap());
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

    case ViewSizeChanged:
        {
            auto jsVal = data.value<QJSValue>();
            int bottom = jsVal.property( CommonConstants::B ).toInt();
            QVariantMap body;
            body.insert(SearchConstants::B,  bottom);
            sendNotification(MapConstants::UpdateMapViewPadding, body);
        }
        break;
    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
         }
         break;

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::SettingMainView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    default:
        return false;
    }
    return true;
}

bool SettingMainViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SettingConstants::ApplySetting: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
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
            if (data.contains(CommonConstants::Var)) {
                QString var = data[CommonConstants::Var].toString();
                QQmlProperty::write( mViewComponent, var, QVariant::fromValue( value ) );
            } else if (data.contains(CommonConstants::Slot)) {
                QString slot = data[CommonConstants::Slot].toString();
                QMetaObject::invokeMethod(mViewComponent, slot.toLocal8Bit().data(), Q_ARG(QVariant, value));
            } else {
               QQmlProperty::write( mViewComponent, CommonConstants::Settings, QVariant::fromValue( value ) );
            }
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMainView))
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
    case SettingConstants::UpdateView: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMainView))
                return false;

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::Visible == it.key() ||
                    CommonConstants::Sync == it.key())
                    continue;
               QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::Sync)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Sync, QVariant::fromValue( data[CommonConstants::Sync]) );
            }
            if (data.contains(CommonConstants::Visible)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, QVariant::fromValue( data[CommonConstants::Visible]) );
            }
        }
        break;
    case RouteConstants::Hitting:   {
            if (!isActive()) {
                break;
            }
            sendNotification(RouteConstants::RelayHitting);
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;

    case SettingConstants::DialogAccepted: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            if (caller != objectName)
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SettingMainView))
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
    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            if (!data.contains(CommonConstants::ObjectName)) {
                break;
            }
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            QString requestObjectName = data[CommonConstants::ObjectName].value<QString>();
            if (requestObjectName != objectName)
                break;

            auto viewList = data[ApplicationConstants::ViewIdList].value<QList<int>>();
            auto viewDataMap = data[ApplicationConstants::ViewDataMap].value<QMap<int,QVariantMap>>();
            QVariantList values = QVariantList{};
            for ( auto viewId :viewList ) {
                QVariantList result;
                CommonConstants::resultViewInit(result);
                result[CommonConstants::ResultViewId        ] = viewId;
                result[CommonConstants::ResultViewName      ] = ViewRegistry::objectName( viewId );
                result[CommonConstants::ResultViewVisible   ] = viewDataMap[viewId][ApplicationConstants::Visible].toBool();
                result[CommonConstants::ResultViewActive    ] = viewDataMap[viewId][ApplicationConstants::Active].toBool();
                values.append(QVariant::fromValue(result));
            }
            QMetaObject::invokeMethod(mViewComponent, "__onResultViewList",Q_ARG(QVariant, QVariant::fromValue(values)) );
        }
        break;


    default:
        return false;
    }
    return true;
}

bool SettingMainViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool SettingMainViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}

