#include "SettingAQIdetailViewMediator.h"

#include "SettingDto.h"
#include "ApplicationConstants.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "SearchConstants.h"
#include "RgConstants.h"
#include <QJSValueIterator>
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"

#include "../common/searchrequest.h"
#include "StringHelper.h"
#include "PoiHelper.h"
#include "KeyboardConstants.h"
#include "LangConstants.h"
#include "uiconstants.h"
#include "UtilConstants.h"
#include "WidgetConstants.h"
#include "ViewRegistry.h"
#include "PlatformHelper.h"

using namespace SKT;

SettingAQIdetailViewMediator::SettingAQIdetailViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SettingAQIdetailViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        SearchConstants::ResponseOnlineSearch,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
        SearchConstants::QueueChanged,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::NavigationDisappeared,
        SearchConstants::ResponseGpsTime,
    };

    return notes;
}

bool SettingAQIdetailViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {        
    case Activated: {            
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingAQIdetailView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuWithTabMode, data);
        }
        break;

    case Deactivated:
        break;

    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;

    case SelectSoftkeyIndexMenuMode:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        break;

    case CloseView:
        sendNotification(NavigationControllerConstants::ReserveSystemSwitchJob, QVariantMap{
                             {CommonConstants::Command, Constants::CloseView},
                             {CommonConstants::Arg, data}
                         });
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

    case RequestSearch: {
            QJSValue jsVal = data.value<QJSValue>();
            int kind = jsVal.property(SearchConstants::Kind).toInt();

            QVariantMap body;
            body.insert( SearchConstants::SearchType, SearchRequestBase::SearchAQI);
            body.insert( SearchConstants::Kind, kind);  //현위치, 목적지
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;

    case QueryOnlineStatus:
        sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        break;

    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;

    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;

    case RequestRouteGuidanceState:
        sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        break;

    case ShowToast:
        sendNotification(ToastsConstants::ShowToast, data);
        break;

    case BackPressed:
        back();
        break;

    case RequestLang: {
            QVariantMap body;
            body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case ShowProgressIndicator:
        sendNotification( UtilConstants::ShowProgressIndicator );
        break;

    case HideProgressIndicator:
        sendNotification( UtilConstants::HideProgressIndicator );
        break;

    case RequestGpsTime:
        sendNotification( SearchConstants::RequestGpsTime);
        break;

    default:
        return false;
    }
    return true;
}

bool SettingAQIdetailViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingAQIdetailView))
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

    case SearchConstants::ResponseOnlineSearch:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result))
                return false;

            QString search = CommonConstants::Empty;
            static int totalCount = 0;
            int startIndex = 0;
            if (data.contains(SearchConstants::ReqData)) {
                QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
                if (reqData.contains(SearchConstants::Query)){
                    search = reqData[SearchConstants::Query].value<QString>();
                }

                if(startIndex == 0) {
                    if (reqData.contains(SearchConstants::Max)){
                        totalCount = reqData[SearchConstants::Max].value<int>();
                    }
                }

                if (reqData.contains(SearchConstants::Params)){
                    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                }
            }            
            QString txt = data[SearchConstants::Result].value<QString>();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(txt.toUtf8());
            QJsonObject jsonObject = jsonResponse.object();
            QVariantMap result = jsonObject.toVariantMap();
            QMetaObject::invokeMethod(mViewComponent, "__onResult",  Q_ARG(QVariant, QVariant::fromValue(result)) );

        }
        break;
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState: {
            QVariantMap m = note->getBody().toMap();
            bool isRg = m["isRg"].toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedRgState", Q_ARG(QVariant, isRg));
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

    case SearchConstants::QueueChanged: {
            int count = note->getBody().value<int>();
            QMetaObject::invokeMethod(mViewComponent, "__queueChanged", Q_ARG(QVariant, count));
        }
        break;

    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:   {
            bool isOnline = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedOnlineStatus", Q_ARG(QVariant, isOnline));
        }
        break;

    case NavigationControllerConstants::NavigationDisappeared:
        // TMCRSM-4144. 후진을 넣어 후방 카메라 화면으로 가면 현재 뷰 닫힘.
#if (0)
        sendNotification(Constants::CloseView, QVariantMap{
                             {CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::SettingAQIdetailView)}
                         });
#endif
        break;
    case SearchConstants::ResponseGpsTime:
        QQmlProperty::write(mViewComponent, "gpsTime", note->getBody());
        break;
    }
    return true;
}

void SettingAQIdetailViewMediator::back()
{
    PlatformHelper::instance()->switchToSystem();
    //sendNotification(ApplicationConstants::ChangeToWidget, QVariantMap{{"type", type}});
}
