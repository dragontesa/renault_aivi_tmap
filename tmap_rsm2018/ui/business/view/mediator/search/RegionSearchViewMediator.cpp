#include "RegionSearchViewMediator.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include <QJSValueIterator>
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "SettingDto.h"
#include "SettingConstants.h"
#include "../common/searchrequest.h"
#include "StringHelper.h"
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "UtilConstants.h"
#include "NavigationControllerProtocol.h"
#include "ViewRegistry.h"

using namespace SKT;

RegionSearchViewMediator::RegionSearchViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> RegionSearchViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::ResponseSearch,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        NavigationControllerConstants::ResponseController,
        NavigationControllerConstants::ChangeLanguage,
    };

    return notes;
}

bool RegionSearchViewMediator::onEvent( int event, const QVariant& data )
{
    //T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::RegionSearchView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;
    case CreateView:
        sendNotification( Constants::CreateView, data );
        break;
    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;

    case UpdateView:
        sendNotification(SearchConstants::UpdateSearchResultText);
        sendNotification(SearchConstants::UpdateView, data);
        break;

    case SelectZone1Title:{
            sendNotification(ZoneConstants::SelectZone1Title, data);
        }
        break;
    case RequestSearch:  {
        auto jsVal = data.value<QJSValue>();
            auto code = jsVal.property( SearchConstants::Query).toString();
            auto param = jsVal.property(SearchConstants::Params).toVariant();
            auto params = param.value<QVariantMap>();
            auto addressType = jsVal.property(SearchConstants::AddressType).toString();

            QVariantMap body;
            body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextAddress);
            body.insert( SearchConstants::TextParam, code);
            if (addressType == SearchConstants::NewAddrType) {
                body.insert( SearchConstants::IsRoad, true );
            } else {
                body.insert( SearchConstants::IsRoad, false );
            }
            body.insert( SearchConstants::Params, QVariant::fromValue(params));
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case RequestAddressType: {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplayMethod;
            sendNotification( SettingConstants::RequestSetting, body );
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

bool RegionSearchViewMediator::handleNotification( const mvc::INotificationPtr& note )
{    
    switch ( note->getId() ) {
    case SearchConstants::ResponseSearch: {
            auto data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result)) {
                return false;
            }

            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                return false;
            }
            SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                return false;
            }
            SearchNextAddrReplyInterface* replyAddr = qobject_cast<SearchNextAddrReplyInterface*>(res);
            if (!replyAddr) {
                return false;
            }   

            const TS_RESULT_ADDR *listitem = replyAddr->results();
            auto values = QVariantList{};
            for (int i=0 ; i < replyAddr->resultCount() ; i++) {
                auto& item = listitem[i];
                auto addrName = StringHelper::instance()->fromCp949((const char*)item.szAddrName);
                auto addrCode = QString::number( item.nAddrCode );
                values.append( QVariant::fromValue(QVariantList{ addrName, addrCode}));
            }
            QString search = "";
            int startIndex = -1;
            int depth = -1;
            if (data.contains(SearchConstants::ReqData)) {
                auto reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                if (reqData.contains(SearchConstants::TextParam)){
                    search = reqData[SearchConstants::TextParam].value<QString>();
                }
                if (reqData.contains(SearchConstants::Params)){
                    auto params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                    if (params.contains(SearchConstants::Depth)){
                        depth = params[SearchConstants::Depth].value<int>();
                    }
                }
            }

            auto totalCount = replyAddr->totalCount();
            QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(depth))   );

        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            auto data = note->getBody().value<QVariantMap>();
            auto objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::RegionSearchView))
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
    case SettingConstants::ChangedSetting:
    case SettingConstants::ApplySetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == SettingConstants::AddressDisplayMethod ) {
                auto method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                QQmlProperty::write(mViewComponent, SearchConstants::AddressType, (method == SettingConstants::AddressDisplayMethodLandNumber ) ? SearchConstants::OldAddrType : SearchConstants::NewAddrType );
            }
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

