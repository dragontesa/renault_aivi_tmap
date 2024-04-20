#include "AddressSearchViewMediator.h"
#include "Constants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "CommonConstants.h"
#include "SettingConstants.h"
#include "SearchConstants.h"
#include "KeyboardConstants.h"
#include "AddressSearchResultVo.h"
#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "ViewRegistry.h"
#include "SettingDto.h"
#include "mediator/app/MapViewMode.h"
#include "NavigationControllerConstants.h"
#include "LangConstants.h"
#include "StringHelper.h"
#include "UtilConstants.h"
#include "ApplicationConstants.h"
#include "QmlAppHelper.h"
#include "RgConstants.h"

#define MAX_ADDRESS_NUMBER_LENGTH  20

namespace {

const QString Bar = QStringLiteral("bar");
const QString List = QStringLiteral("list");
const QString From = QStringLiteral("from");
const QString Lcd = QStringLiteral("lcd");
const QString Mcd = QStringLiteral("mcd");
const QString Road = QStringLiteral("road");
const QString Dong = QStringLiteral("dong");
const QString Bld = QStringLiteral("bld");
const QString Lot = QStringLiteral("lot");
const QString LcdName = QStringLiteral("lcdName");
const QString McdName = QStringLiteral("mcdName");
const QString ScdName = QStringLiteral("scdName");

}

using namespace SKT;

AddressSearchViewMediator::AddressSearchViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mBar( nullptr ),
    mList( nullptr )
{
    mBar = viewComponent->findChild<QObject*>( Bar );
    mList = viewComponent->findChild<QObject*>( List );
#ifdef BUILD_TARGET
    QQmlProperty::write(mViewComponent, "isBuildTarget", true);
#endif
    mKeyboardProxy.setMaxStringLength(MAX_ADDRESS_NUMBER_LENGTH);
    mKeyboardProxy.setEnableChars("0123456789-");
}

QList<int> AddressSearchViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseSearch,
        SearchConstants::ApplyAddressSearch,
        KeyboardConstants::ProcessKeyboard,
        KeyboardConstants::SystemKeyboardPressed,
        KeyboardConstants::SystemKeyboardBackPressed,
        KeyboardConstants::SystemKeyboardFinished,
        ZoneConstants::DoSoftKeyFunction,
        SettingConstants::ApplySetting,        
        SettingConstants::ChangedSetting,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,
        SearchConstants::UpdateView,
        SettingConstants::DialogAccepted,
        ApplicationConstants::ReplyToViewList,
        //[SOFTKEYINDEX]
        ZoneConstants::DoSoftkeyIndexMenuFunction
    };

    return notes;
}

bool AddressSearchViewMediator::onEvent( int event, const QVariant& data )
{
    //T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Mediated: {
//            QVariantMap body;
//            body[CommonConstants::Section] = SettingConstants::MapMain;
//            body[CommonConstants::Key] = SettingConstants::AddressDisplay2ndMethod;
//            sendNotification( SettingConstants::RequestSetting, body );
            auto lang = QmlAppHelper::instance()->language();
            QMetaObject::invokeMethod(mViewComponent, "retransalteUi", Q_ARG(QVariant, lang));
        }
        break;
    case Activated: {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplay2ndMethod;

            sendNotification(NavigationControllerConstants::QueryOnlineStatus);

            // 18.3.4 hskim toggle menu must be re-registered when activating
            sendNotification( SettingConstants::RequestSetting, body );
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{MapConstants::Mode,MapViewMode::MapInquiry}});
            sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));
            sendNotification(RgConstants::BlockReroute, true);
        }
        break;

    case ChangeSoftKey: {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
#if (1)
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuWithTabMode, data);
#else
            sendNotification(ZoneConstants::SelectSoftkeyMenuWithTabMode, data);
#endif
        }
        break;
    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;
    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case RegistKeyboardInput:
        sendNotification(KeyboardConstants::RegistKeyboardInput, data);
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case HideKeyboard: {
         QVariantMap body;
         body[CommonConstants::DontUseEditBox] = true;
         sendNotification(KeyboardConstants::HideKeyboard, body);
#ifdef BUILD_TARGET
        {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplay2ndMethod;
            sendNotification( SettingConstants::RequestSetting, body );

            auto title = tr("주소 검색");
            sendNotification(ZoneConstants::SelectZone1Title, title);
        }
#endif
        }
        break;
    case UpdateKeyboard:
        sendNotification(KeyboardConstants::UpdateKeyboard, data);
        break;        
    case AddressSearchRequested:
        {
            auto args = data.toMap();
            auto searchType = args[SearchConstants::SearchType].toInt();
            auto type = args[CommonConstants::Type].toString();
            auto from = args[From].toString();
            auto code = args[SearchConstants::Code].toString();
            auto name = args[CommonConstants::Name].toString();
            auto params = args[SearchConstants::Params].toMap();
            T_DEBUG( QString("Request searchType:%1, type:%2, code:%3, name:%4 startIndex:%5")
                .arg( searchType ).arg( type ).arg( code ).arg( name ).arg( params[SearchConstants::JibunStIdx].toInt() ) );

            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplay2ndMethod;
            if ( searchType == RoadSearch ) {
                body[CommonConstants::Value] = SettingConstants::AddressDisplayMethodRoadName;
            }
            else {
                body[CommonConstants::Value] = SettingConstants::AddressDisplayMethodLandNumber;
            }
            sendNotification( SettingConstants::SaveSetting, body );

		    if ( type == Lcd ) {
                QVariantMap body;
                body.insert( SearchConstants::RequestContext, QVariant::fromValue( args ) );
                body.insert( SearchConstants::ResponseNotificationId, SearchConstants::ApplyAddressSearch );
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextAddress );
                body.insert( SearchConstants::TextParam, "0" );
                body.insert( SearchConstants::IsRoad, (searchType == RoadSearch) ? true : false );
                sendNotification(SearchConstants::RequestSearch, body);
            }
			else if ( type == Mcd ) {
                QVariantMap body;
                body.insert( SearchConstants::RequestContext, QVariant::fromValue( args ) );
                body.insert( SearchConstants::ResponseNotificationId, SearchConstants::ApplyAddressSearch );
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextAddress );
                body.insert( SearchConstants::TextParam, code );
                body.insert( SearchConstants::IsRoad, (searchType == RoadSearch) ? true : false );
                sendNotification(SearchConstants::RequestSearch, body);
            }
			else if ( type == Road ) {
                QVariantMap body;
                body.insert( SearchConstants::RequestContext, QVariant::fromValue( args ) );
                body.insert( SearchConstants::ResponseNotificationId, SearchConstants::ApplyAddressSearch );
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextAddress );
                body.insert( SearchConstants::TextParam, code );
                body.insert( SearchConstants::IsRoad, (searchType == RoadSearch) ? true : false );
                sendNotification(SearchConstants::RequestSearch, body);
            }
            else if ( type == Dong ) {
                T_DEBUG( QString("<B>Dong %1</B>").arg( searchType ) );
                QVariantMap body;
                body.insert( SearchConstants::RequestContext, QVariant::fromValue( args ) );
                body.insert( SearchConstants::ResponseNotificationId, SearchConstants::ApplyAddressSearch );
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextAddress );
                body.insert( SearchConstants::TextParam, code );
                body.insert( SearchConstants::IsRoad, (searchType == RoadSearch) ? true : false );
                body.insert( SearchConstants::TextParam, code );
                sendNotification(SearchConstants::RequestSearch, body);
            }
            else if ( type == Bld ) {
//                auto searchText = jsVal.property( "searchText" ).toString();
//                QVariantList list;
//                QString bnCode;
//                QString bnName;
//                for ( int i = 0; i < 50; i++ ) {
//                    bnCode = code + QString::asprintf( "%03d", i );
//                    bnName = name + QString("_%1_%2").arg(searchText).arg(i);
//                    list.append( QVariant::fromValue( AddressSearchResultVo( bnCode, bnName ) ) );
//                }

//                updateDataWrap( list, type, from, code, name );
            }
            else if ( type == Lot ) {
//                auto searchText = jsVal.property( "searchText" ).toString();
//                QVariantList list;
//                QString lotCode;
//                QString lotName;
//                for ( int i = 0; i < 50; i++ ) {
//                    lotCode = code + QString::asprintf( "%03d", i );
//                    lotName = name + QString("_%1_%2").arg(searchText).arg(i);
//                    list.append( QVariant::fromValue( AddressSearchResultVo( lotCode, lotName ) ) );
//                }
//                updateDataWrap( list, type, from, code, name );
            }
        }
        break;
    case ReturnToFavorite:
        sendNotification(SearchConstants::ReturnToFavorite);
        break;

    case ReturnToViaSetting:
        {
            QString caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
            sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, caller}});
        }
        break;

    case ReturnToNaviMenu:
        {
            QString url = ViewRegistry::url(ViewRegistry::NaviMenuView);
            sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});
        }
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;


    case RequestViewList: {
            QVariantMap body;
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    case ClearKeyBuffer:
        mKeyboardProxy.clear();
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

 	case UpdateView:
        sendNotification(SearchConstants::UpdateView, data);
        break;
    default:
        return false;
    }

    return true;
}

bool AddressSearchViewMediator::handleNotification( const mvc::INotificationPtr& note )
{    
    auto updateDataWrap = [&](const QVariantList& data, const QString& reqType,
        const QString& reqFrom, const QString& reqCode, const QString& reqName,
        const QVariantMap& shortcutMap){
        QMetaObject::invokeMethod( mViewComponent, "updateData",
            Q_ARG(QVariant, QVariant::fromValue( data )),
            Q_ARG(QVariant, QVariant::fromValue( reqType )),
            Q_ARG(QVariant, QVariant::fromValue( reqFrom )),
            Q_ARG(QVariant, QVariant::fromValue( reqCode )),
            Q_ARG(QVariant, QVariant::fromValue( reqName )),
            Q_ARG(QVariant, QVariant::fromValue( shortcutMap ))
        );
    };

    switch ( note->getId() ) {
    case SearchConstants::ApplyAddressSearch: {
            //T_DEBUG( "<B>ApplyAddressSearch #1<B>" );
            auto data = note->getBody().toMap();
            if (!data.contains(SearchConstants::Result)) {
                return false;
            }

            auto reqData = data[SearchConstants::ReqData].toMap();
            auto reqCtx = reqData[SearchConstants::RequestContext].toMap();
            auto reqType = reqCtx[CommonConstants::Type].toString();
            auto reqFrom = reqCtx[From].toString();
            auto reqCode = reqCtx[SearchConstants::Code].toString();
            auto reqName = reqCtx[CommonConstants::Name].toString();            

            //T_DEBUG( QString("Response type:%1, code:%2, name:%3").arg( reqType ).arg( reqCode ).arg( reqName ) );
            
            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                return false;
            }
            SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                sendNotification( UtilConstants::HideProgressIndicator );
                return false;
            }
            SearchNextAddrReplyInterface* replyAddr = qobject_cast<SearchNextAddrReplyInterface*>(res);
            if (!replyAddr) {
                sendNotification( UtilConstants::HideProgressIndicator );
                return false;
            }

            QVariantMap shortcutMap;
            const TS_RESULT_ADDR *listitem = replyAddr->results();
            int lastChosung = std::numeric_limits<int>::min();

            auto isMcdReqType = reqType == "mcd";

            auto values = QVariantList{};
            for (int i=0, row=0 ; i < replyAddr->resultCount() ; i++) {
                auto& item = listitem[i];

                auto addrCode = QString::number( item.nAddrCode );
                auto addrName = StringHelper::instance()->fromCp949((const char*)item.szAddrName);
                if ( addrName.indexOf( QChar('<') ) > -1 ) {
                    continue;
                }                

                if ( isMcdReqType ) {
                    addrName.replace( ' ', '\n' );
                }

                values.append( QVariant::fromValue(
                    AddressSearchResultVo( addrCode, addrName,
                        item.nJibunStIdx, item.nJibunCnt,
                        item.nAptStIdx, item.nAptCnt,
                        item.nNewAddrStIdx, item.nNewAddrCnt, item.nAddrRiCode )
                ) );

                auto firstCh = addrName.at( 0 );
                auto chosung = StringHelper::extractChosung( firstCh );
//                T_DEBUG( QString( "addrName: %1 cho: %2" ).arg( addrName ).arg( chosung ) );
                if ( chosung != lastChosung ) {
                    shortcutMap[QString(chosung)] = row;
                    lastChosung = chosung.unicode();
                }

//                T_DEBUG( QString( "nAddrCode: %1 addrName: %2 nJibunStIdx: %3 nJibunCnt: %4 nNewAddrStIdx: %5 nNewAddrCnt: %6")
//                    .arg( item.nAddrCode )
//                    .arg( addrName )
//                    .arg( item.nJibunStIdx )
//                    .arg( item.nJibunCnt )
//                    .arg( item.nNewAddrStIdx )
//                    .arg( item.nNewAddrCnt )
//                    );

                row++;
            }
            updateDataWrap( values, reqType, reqFrom, reqCode, reqName, shortcutMap );
            sendNotification( UtilConstants::HideProgressIndicator );
        }
        break;

    case KeyboardConstants::ProcessKeyboard: {
#ifdef QT_DEBUG
             auto args = note->getBody().value<QVariantMap>();
             auto owner = args[CommonConstants::Owner].toString();
             auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
             if (owner != objectName)
                 return false;
             auto key = args[CommonConstants::Key];
             auto text = args[CommonConstants::Text];
             QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );

#ifdef BUILD_TARGET
             // 도로명, 지번 주소로 실제 키보드 입력을 받을 때
             // 우분투 호스트의 경우 키보드 시뮬레이터에서 직접 입력 객체에 텍스트를 집어 넣어준다.
             // 타겟에서는 시뮬레이터를 사용하지 않으므로 별도의 프로퍼티를 설정해서
             // 입력 텍스트 객체들이 이 프로퍼티를 쓰도록 함.
             // 18.06.03 ymhong
             QQmlProperty::write(mViewComponent, "keyboardText", text);
#endif
#endif
         }
         break;

#ifdef BUILD_TARGET
    case KeyboardConstants::SystemKeyboardPressed:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toString();

            auto updated = mKeyboardProxy.keyPressed(key);
            auto text = mKeyboardProxy.string();
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );

            QQmlProperty::write(mViewComponent, "keyboardText", text);
        }
        break;
    case KeyboardConstants::SystemKeyboardBackPressed:
    case KeyboardConstants::SystemKeyboardFinished:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toInt();
            auto text = m["text"].toString();
            if (note->getId() == KeyboardConstants::SystemKeyboardFinished) {
                text = mKeyboardProxy.string();
            }
            QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
#endif
    case ZoneConstants::DoSoftkeyIndexMenuFunction:
    case ZoneConstants::DoSoftKeyFunction:
        {
            auto m = note->getBody().toMap();
            auto func = m[ZoneConstants::Function].toString();
            auto index = m["index"].toInt();
            qDebug() << "ZoneConstants::DoSoftkeyIndexMenuFunction:" << index;

            if (m[CommonConstants::Name].toString() == ViewRegistry::objectName(ViewRegistry::AddressSearchView)) {
                if (func == ZoneConstants::Back) {
                    QMetaObject::invokeMethod(mViewComponent, "onSoftkeyBackClicked");
                }
                else if (func == ZoneConstants::Menu) {
                    auto menuName = m[ZoneConstants::Menu].toString();
                    QMetaObject::invokeMethod(mViewComponent, "onSoftkeyMenuClicked", Q_ARG(QVariant,menuName));
                }
            }
        }
        break;
    case SettingConstants::ChangedSetting:
    case SettingConstants::ApplySetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == SettingConstants::AddressDisplay2ndMethod ) {
                auto method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                QQmlProperty::write(mViewComponent, "searchTypePref", method);

                // 18.3.4 hskim toggle menu must be re-registered whenever it is activated
                QMetaObject::invokeMethod(mViewComponent, "setupToggleMenu");
            }

            // 18.3.4 hskim the initial request must be proccessed right after receiving the toggle setting value
            // in order not to change it as this view's default value
            if ( !mInitialRequestProcessed ) {
                QMetaObject::invokeMethod(mViewComponent, "doInitialRequest");
                mInitialRequestProcessed = true;
            }
        }
        break;
    case NavigationControllerConstants::ChangeLanguage:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();

            T_DEBUG( QString( "ChangeLanguage lang:%1" ).arg( lang ) );

            QMetaObject::invokeMethod(mViewComponent, "retransalteUi", Q_ARG(QVariant, lang));
        }
        break;
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:   {
            bool isOnline = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedOnlineStatus", Q_ARG(QVariant, isOnline));
        }
        break;

    case SearchConstants::UpdateView: {
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::ObjectName)) {
                QString objName = data[CommonConstants::ObjectName].toString();
                if (objName != objectName)
                    return false;
            }

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::ReFocus == it.key()) {
                    continue;
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::ReFocus)) {
                QQmlProperty::write( mViewComponent, CommonConstants::ReFocus, QVariant::fromValue( data[CommonConstants::ReFocus]) );
            }
        }
        break;

    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;

    case SettingConstants::DialogAccepted:
        onAccepted(note->getBody());
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

void AddressSearchViewMediator::onAccepted(const QVariant &v)
{
#ifdef BUILD_TARGET
    auto m = v.toMap();
    auto message = m["message"].toString();
    if (message == tr("검색 결과가 없습니다.")) {
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
    }
#endif
}
