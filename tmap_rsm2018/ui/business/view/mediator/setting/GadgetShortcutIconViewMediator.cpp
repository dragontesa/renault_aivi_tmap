#include "GadgetShortcutIconViewMediator.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "SettingDto.h"
#include "SettingConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include <QJSValueIterator>
#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "../common/PoiHelper.h"
#include "NavigationControllerConstants.h"
#include "ViewRegistry.h"
#include "RouteConstants.h"
#include "mediator/app/MapViewMode.h"
#include "LangConstants.h"
#include "UtilConstants.h"
#include "ApplicationConstants.h"
#include "PlatformHelper.h"
#include "uiconstants.h"
#include "NavigationControllerProtocol.h"


using namespace SKT;

GadgetShortcutIconViewMediator::GadgetShortcutIconViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mOnline(false)
{
}

QList<int> GadgetShortcutIconViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::UpdateView,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ReplyToViewList,
        NavigationControllerConstants::ResponseController,
    };
    return notes;
}

bool GadgetShortcutIconViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );
    switch ( event ) {
    case Activated: {
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MenuNavi}});

            sendNotification(NavigationControllerConstants::QueryOnlineStatus);

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::GadgetShortcutIconView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;

    case SelectZone1Title:{
            sendNotification(ZoneConstants::SelectZone1Title, data);
        }
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::GadgetShortcutIconView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    case TouchGadgetShotcutIcon: {
            QJSValue jsVal = data.value<QJSValue>();
            int kind = jsVal.property(SearchConstants::Kind).toInt();
            switch(kind) {
                case SearchConstants::CategoryGasSt:
                case SearchConstants::CategoryChargingSt:
                case SearchConstants::CategoryChargingEvSt:
                    PlatformHelper::instance()->requestAroundSearchPatrol(kind);
                    break;
                case SearchConstants::CategoryParkingSp:
                    PlatformHelper::instance()->requestAroundSearchParking();
                    break;
                default: {//AQI로 사용
                        const int GADGET_TOUCH_RELEASED = 2;
                        PlatformHelper::instance()->gadgetTouched(GADGET_TOUCH_RELEASED, 1, 1, 0, UIConstants::GadgetIconAqi);
                    }
                    break;
            }
        }
        break;

    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
         }
         break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;



    default:
        return false;
    }
    return true;
}

bool GadgetShortcutIconViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::GadgetShortcutIconView))
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
    case SearchConstants::UpdateView: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::GadgetShortcutIconView))
                return false;

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() || CommonConstants::ObjectName == it.key())
                    continue;
               QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
        }
        break;

    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName != ViewRegistry::objectName(ViewRegistry::GadgetShortcutIconView))
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
