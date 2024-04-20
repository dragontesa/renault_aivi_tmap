#include "TextSearchViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "SettingDto.h"
#include "SettingConstants.h"
#include "ZoneConstants.h"
#include "RouteConstants.h"
#include "KeyboardConstants.h"
#include <QJSValueIterator>

#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "ViewRegistry.h"

#include "NavigationControllerConstants.h"

#include "OfflineJobCacheProxy.h"
#include "SearchResultViewMediator.h"
#include "RecentDestinationDto.h"
#include "FavoriteDto.h"

#include "UtilConstants.h"
#include "MapConstants.h"
#include "NavibookmarkConstants.h"
#include "PoiHelper.h"
#include "PlatformHelper.h"

#include "RecentSearchDto.h"
#include "LangConstants.h"
#include "ApplicationConstants.h"
#include "NavigationControllerProtocol.h"
#include "RgConstants.h"
#include "mediator/app/MapViewMode.h"

using namespace SKT;

#define MAX_SEARCH_TEXT_LENGTH  18

TextSearchViewMediator::TextSearchViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mIsRg(false)
    , mSearchResultCount(0)
{
}

QList<int> TextSearchViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        KeyboardConstants::ProcessKeyboard,
        SearchConstants::ResponseSearch,
        SearchConstants::ResponseCacheSearch,
        SearchConstants::ResponseOnlineSearch,
        SearchConstants::ResponseFavorSearch,
        SearchConstants::ResponseDestSearch,
        SearchConstants::TextSearchHasNoResult,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        MapConstants::ResponseDriveMode,
        SearchConstants::ResponseNoResult,
        SearchConstants::ResponseRecentSearch,
        KeyboardConstants::PullDownKeyboard,
        KeyboardConstants::PullUpKeyboard,
        NavigationControllerConstants::NavigationAppeared,
        SearchConstants::QueueChanged,
        ApplicationConstants::ReplyToViewList,
        NavigationControllerConstants::ResponseController,
        SearchConstants::UpdateView,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,
        SearchConstants::UpdateTextSearchZone1,
        SearchConstants::ClearTextSearchZone1,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
        SearchConstants::ResponseLockStatus,
    };
    return notes;
}

bool TextSearchViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );
    switch ( event ) {
    case Activated: {
            qDebug() << "[ZONE1] TextSearchView.Activated";

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::TextSearchView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
#ifdef BUILD_TARGET
            QQmlProperty::write(mViewComponent, "keyboardHeight", SYSTEM_KEYBOARD_HEIGHT);
#endif
            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            auto query = QQmlProperty::read(mViewComponent, "query").toString();
            if (caller == CommonConstants::System && query.length()) {
                mUpdateTextCount = 0;
            } else {
                auto isBookmark = caller == ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
                auto isVia = caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView);
                if (isBookmark || isVia) {
                    if (mUpdateZone1) {

                    } else {
                        // 경유지 설정, 즐겨 찾기 추가로부터 진입
                        mText = "";
                    }
                }
            }
            sendNotification(RgConstants::BlockReroute, true);
        }
        break;
    case Deactivated:
        break;
    case CreateView: {
        auto m = data.toMap();
        auto url = m[CommonConstants::Url].toString();
        auto query = QString{};
        if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
            PlatformHelper::instance()->setParentForSystemPopup(m);
        } else {
            // SearchResultView

            if (isCallerBookmark() || isCallerViaSetting()) {
                // 키보드가 pulldown인 상태로 검색 결과 화면으로 갔다가 복귀 하면 keyboardDown이 true
                // 고로 ShowKeyboard가 호출 되지 않으면서 타이틀이 변경 되지 않음.
                //
                // 19.11.24 ymhong
                QQmlProperty::write(mViewComponent, "keyboardDown", false);
            } else {
                query = m[SearchConstants::Query].toString();
                sendNotification(ZoneConstants::UpdateSearchText, query);

                auto setEditFieldText = mText != query;
                if (setEditFieldText) {
                    sendNotification(SearchConstants::UpdateSearchResultText);
                }
            }
        }
        sendNotification(Constants::CreateView, m);
        break;
    }
    case UpdateView:
        sendNotification(SearchConstants::UpdateView, data);
        break;
    case RouteSettingUpdateView:
        {
            QVariantMap m = data.toMap();
            bool visible = m[CommonConstants::Visible].toBool();
            QString viewName = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
            auto notiId = visible ? Constants::ShowView : Constants::HideView;

            sendNotification(notiId, QVariantMap {{CommonConstants::ObjectName, viewName}});
        }
        break;
    case CloseView:
        {
            bool switchToSystem = false;
            auto m = data.toMap();
            auto objectName = m[CommonConstants::ObjectName].toString();
            if (objectName == ViewRegistry::objectName(ViewRegistry::TextSearchView)) {
                auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
                if (caller == CommonConstants::System) {
                    QQmlProperty::write(mViewComponent, "caller", "");
                    switchToSystem = true;
                }
            }
            if (switchToSystem) {
                sendNotification(NavigationControllerConstants::ReserveSystemSwitchJob, QVariantMap{
                                     {CommonConstants::Command, Constants::CloseView},
                                     {CommonConstants::Arg, data}
                                 });
                PlatformHelper::instance()->switchToSystem();
            } else {
                sendNotification(Constants::CloseView, data);
            }
        }
        break;

    case BackClicked:
        sendNotification(ZoneConstants::UpdateSearchText, "");
        break;

    case SelectZone1TextInput: {
#ifdef QT_DEBUG
        // 경유지 -> 통합 검색 시 ZONE1의 타입이 SEARCH_TEXT_1 에서 NAV_SEARCH_BAR로 변경 되는 문제 막음
        // 19.06.20 ymhong.
        auto m = data.toMap();
        auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
        m[CommonConstants::Caller] = caller;
        sendNotification(ZoneConstants::SelectZone1TextInput, m);
#else

#endif
        break;
    }
    case FocusZone1TextInput:
        sendNotification(ZoneConstants::FocusZone1TextInput);
        break;
    case RegistKeyboardInput:
        sendNotification(KeyboardConstants::RegistKeyboardInput, data);
        break;
    case ShowKeyboard:
        qDebug() << "[ZONE1] TextSearchView.ShowKeyboard";
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        // SearchResultView로부터 복귀
        if (mUpdateZone1) {
            qDebug() << "[ZONE1] TextSearchView.ShowKeyboard.mUpdateZone1: " << mText;
//            if (mIsPulldown) {
//                pullupKeyboard();
//            }
            PlatformHelper::instance()->setEditFieldText(mText);
            mUpdateZone1 = false;
        }
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;
    case PulldownKeyboard:
        pulldownKeyboard();
        break;

    case RequestSearch: {
            QJSValue jsVal = data.value<QJSValue>();
            QString search = jsVal.property( SearchConstants::Query).toString();
            QString query = StringHelper::instance()->normalized(search);

            T_DEBUG( QString("TextParam: %1").arg(query) );
            int order = jsVal.property( SearchConstants::Order).toInt();
            QVariant param = jsVal.property(SearchConstants::Params).toVariant();
            QVariantMap params = param.value<QVariantMap>();
            QString addressType = jsVal.property(SearchConstants::AddressType).toString();

            QVariantMap body;
            body.insert( SearchConstants::SearchType, SearchRequestBase::SearchBySisulNamePrediction);
            if (addressType == SearchConstants::NewAddrType) {
                body.insert( SearchConstants::RoadSearch, true );
            } else {
                body.insert( SearchConstants::RoadSearch, false );
            }
            body.insert( SearchConstants::TextParam, (QString)query);
            body.insert( SearchConstants::Query,   (QString)search);
            body.insert( SearchConstants::Params, QVariant::fromValue(params));
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;

    case RequestRecentSearch:
        sendNotification(SearchConstants::RequestRecentSearch, data);
        break;


    case ReturnToFavorite:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, SearchConstants::FavoritePageIndex);
        QMetaObject::invokeMethod(mViewComponent, "close");
        break;


    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case ReturnToNaviMenu:
        {
            QString url = ViewRegistry::url(ViewRegistry::NaviMenuView);
            sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});

            QString searchTabView = ViewRegistry::objectName(ViewRegistry::SearchTabView);
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, searchTabView}});
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;

    case RequestAddressType: {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplayMethod;
            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;

    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;

    case ShowProgressIndicator:
        sendNotification( UtilConstants::ShowProgressIndicator );
        break;

    case HideProgressIndicator:
        sendNotification( UtilConstants::HideProgressIndicator );
        break;

    case RemoveSearchQueue: {
            QVariantMap body;
            body.insert( SearchConstants::SearchType, SearchRequestBase::RemoveSearchQueue);
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;
    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::TextSearchView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;
    case HideView:
        sendNotification(Constants::HideView, data);
        break;

    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
         }
         break;

    case UpdateSearchResultCount: {
             QJSValue jsVal = data.value<QJSValue>();
             QString search = jsVal.property( CommonConstants::Text ).toString();
             int totalCount = jsVal.property( CommonConstants::Count ).toInt();
             updateSearchResultCount(search, totalCount);

             // ContactGaget에 의해 검색어 입력 화면으로 왔을 때
             // 첫 UpdateSearchResultCount일 때 zone1의 editfield의 text를 갱신한다.
             // 19.07.10 ymhong
             auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
             if (caller == CommonConstants::System && mUpdateTextCount == 0) {
                 ++mUpdateTextCount;
                 sendNotification(NavigationControllerConstants::UpdateEditFieldText);
             }
         }
         break;
    case ShowToast:
        sendNotification(ToastsConstants::ShowToast, data);
        break;

    case CloseAllViews:
        sendNotification(ApplicationConstants::ResetNavi);
        break;

    case QueryInDriveMode:
        mDestData = data;
        queryInDriveMode();
        break;

    case QueryOnlineStatus:
        sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        break;

    case RequestRouteGuidanceState:
        sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        break;

    default:
        return false;
    }
    return true;
}

bool TextSearchViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    qDebug()<<"###noti id="<<note->getId();
    switch ( note->getId() ) {
    case SearchConstants::UpdateView:
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:
    case NavigationControllerConstants::ChangeLanguage:
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
    case SearchConstants::UpdateTextSearchZone1:
    case SearchConstants::ClearTextSearchZone1:
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        break;
    default: {
            if (QQmlProperty::read(mViewComponent, CommonConstants::IsClosed).toBool()) {
                switch ( note->getId() ) {
                    case SearchConstants::ResponseSearch:
                    case SearchConstants::ResponseCacheSearch:{
                            QVariantMap data = note->getBody().value<QVariantMap>();
                            QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                            auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                            if (owner != this) {
                                return false;
                            }
                            sendNotification( UtilConstants::HideProgressIndicator );
                    }
                    break;
                 }
                return false;
            }
        }
        break;
    }

    switch ( note->getId() ) {
    case SearchConstants::ClearTextSearchZone1:
        mText = "";
        break;

    case SearchConstants::UpdateTextSearchZone1:
        {
            auto update = note->getBody().toBool();
            mUpdateZone1 = update;
        }
        break;

    case NavigationControllerConstants::NavigationAppeared:
        if (0 < mText.length()) {
            // TMCRSM-3135
            // 경유지 편집>통합검색>검색 결과>검색 결과 상세에서 전화번호 클릭 하면 시스템의 전화 걸기 화면으로 이동
            // 네비 화면으로 복귀 시 updateSearchResultCount()호출로 인해 키보드가 노출 됨.
            // 따라서 updateSearchResultCount 호출 전에 현재 뷰가 보이고 있는지 체크 한다.
            // 19.05.14
            auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
            if (visible) {
                auto isKeyboardDown = QQmlProperty::read(mViewComponent, "keyboardDown").toBool();

                if (!isKeyboardDown) {
                    qDebug() << "[KBD] TextSearchView.NavigationAppeared: updateSearchResultCount(keyboard.show? " << true << ")";
                    updateSearchResultCount(mText, mSearchResultCount, true);
                }
            }
        }
        break;

    case KeyboardConstants::ProcessKeyboard: {
             QVariantMap args = note->getBody().value<QVariantMap>();
             QString owner = args[CommonConstants::Owner].toString();
             if (owner != ViewRegistry::objectName(ViewRegistry::TextSearchView)) {
                 if (!isVisible())
	                 return false;
             }
             auto key = args[CommonConstants::Key];
             auto text = args[CommonConstants::Text].toString();

             // Ubuntu host에서는 zone1 editbox에서 자체적으로 filtering 함.
             QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
         }
         break;
    case SearchConstants::ResponseSearch: {
            qDebug() << "[Perf][TextSearchView][ResponseSearch]";
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result)) {
                return false;
            }

            QVariantList values = QVariantList{};
            int totalCount = 0;
            QString search = CommonConstants::Empty;
            int startIndex = -1;
            if (data.contains(SearchConstants::ReqData)) {
                QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
                if (reqData.contains(SearchConstants::Query)){
                    search = reqData[SearchConstants::Query].value<QString>();
                }
                if (reqData.contains(SearchConstants::Params)){
                    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                }
            }

            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                QMetaObject::invokeMethod(mViewComponent, "__onResultOffline", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                return false;
            }
            SearchReplyBaseInterface *reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                QMetaObject::invokeMethod(mViewComponent, "__onResultOffline", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                return false;
            }
            SearchReplyInterface* searchReply = qobject_cast<SearchReplyInterface*>(res);
            if (!searchReply) {
                QMetaObject::invokeMethod(mViewComponent, "__onResultOffline", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                return false;
            }

            auto req = reply->request();
            auto isOnline = req->getOnline();

            const TPOI_SEARCH_LIST *listitem = searchReply->results();            
            for (int i=0 ; i < searchReply->resultCount() ; i++) {
                QVariantList result;

                auto navSeq = QString(listitem[i].szNavseq);
                auto poiId = isOnline ? PoiHelper::fromServer(listitem[i].nPoiID, navSeq.toInt())
                                      : PoiHelper::fromLocal(listitem[i].nPoiID);

                SearchConstants::resultInit(result);
                result[SearchConstants::ResultIndex               ] = listitem[i].nIdx;
                result[SearchConstants::ResultName                ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szName);
                result[SearchConstants::ResultStdName             ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szStdName);
                result[SearchConstants::ResultAddress             ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szAddrName);
                result[SearchConstants::ResultTelNum              ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szPhoneNumber);
                result[SearchConstants::ResultPOIID               ] = poiId;//listitem[i].nPoiID;
                result[SearchConstants::ResultNavseq              ] = navSeq;
                result[SearchConstants::ResultAddrCode            ] = listitem[i].nAddrCode;
                result[SearchConstants::ResultRpFlag              ] = listitem[i].byRPFlag;
                result[SearchConstants::ResultDistance            ] = listitem[i].nDist;
                result[SearchConstants::ResultDeg                 ] = listitem[i].nDeg;
                result[SearchConstants::ResultKind                ] = listitem[i].nKind;
                result[SearchConstants::ResultX                   ] = listitem[i].tp.x;
                result[SearchConstants::ResultY                   ] = listitem[i].tp.y;
                result[SearchConstants::ResultOPIPoleCate         ] = listitem[i].nOPIPoleCate;
                result[SearchConstants::ResultOPIPrice            ] = listitem[i].nOPIPrice;
                result[SearchConstants::ResultYear                ] = listitem[i].sGatherTime.nYear;
                result[SearchConstants::ResultMonth               ] = listitem[i].sGatherTime.nMonth;
                result[SearchConstants::ResultDay                 ] = listitem[i].sGatherTime.nDay;
                result[SearchConstants::ResultHour                ] = listitem[i].sGatherTime.nHour;
                result[SearchConstants::ResultMinute              ] = listitem[i].sGatherTime.nMinute;
                result[SearchConstants::ResultSecond              ] = listitem[i].sGatherTime.nSecond;
                result[SearchConstants::ResultWeek                ] = listitem[i].sGatherTime.nWeek;
                result[SearchConstants::ResultOilCompany          ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szOilCompany);
                result[SearchConstants::ResultHighGasoline        ] = listitem[i].bHighGasoline;
                result[SearchConstants::ResultLowestOilSt         ] = listitem[i].bLowestOilSt;
                result[SearchConstants::ResultOPIGasolinePrice    ] = listitem[i].nOPIGasolinePrice;
                result[SearchConstants::ResultOPIDieselPrice      ] = listitem[i].nOPIDieselPrice;
                result[SearchConstants::ResultOPILPGPrice         ] = listitem[i].nOPILPGPrice;
                result[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem[i].nOPIHiGasolinePrice;
                result[SearchConstants::ResultOPIHiLPGPrice       ] = listitem[i].nOPIHiLPGPrice;
                result[SearchConstants::ResultOnline              ] = isOnline;
                result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeNormal;
                values.append(QVariant::fromValue(result));
            }


            totalCount = searchReply->totalCount();
            QMetaObject::invokeMethod(mViewComponent, "__onResultOffline", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)) );

        }
        break;
    case SearchConstants::ResponseCacheSearch:{
            qDebug() << "[Perf][TextSearchView][ResponseCacheSearch]";
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

            SearchCacheInfoListPtr list = data[SearchConstants::Result].value<SearchCacheInfoListPtr>();
            QVariantList values = QVariantList{};
            if(list) {
                for (int i=0 ; i < list->size(); i++) {
                    TPOI_SEARCH_LIST listitem = list->at(i);
                    QVariantList result;
                    SearchConstants::resultInit(result);
                    result[SearchConstants::ResultIndex               ] = listitem.nIdx;
                    result[SearchConstants::ResultName                ] = (const char*)listitem.szName;
                    result[SearchConstants::ResultStdName             ] = (const char*)listitem.szStdName;
                    result[SearchConstants::ResultAddress             ] = (const char*)listitem.szAddrName;
                    result[SearchConstants::ResultTelNum              ] = (const char*)listitem.szPhoneNumber;
                    result[SearchConstants::ResultPOIID               ] = listitem.nPoiID;
                    result[SearchConstants::ResultAddrCode            ] = listitem.nAddrCode;
                    result[SearchConstants::ResultRpFlag              ] = listitem.byRPFlag;
                    result[SearchConstants::ResultNavseq              ] = QString(listitem.szNavseq);
                    result[SearchConstants::ResultDistance            ] = listitem.nDist;
                    result[SearchConstants::ResultDeg                 ] = listitem.nDeg;
                    result[SearchConstants::ResultKind                ] = listitem.nKind;
                    result[SearchConstants::ResultX                   ] = listitem.tp.x;
                    result[SearchConstants::ResultY                   ] = listitem.tp.y;
                    result[SearchConstants::ResultOPIPoleCate         ] = listitem.nOPIPoleCate;
                    result[SearchConstants::ResultOPIPrice            ] = listitem.nOPIPrice;
                    result[SearchConstants::ResultYear                ] = listitem.sGatherTime.nYear;
                    result[SearchConstants::ResultMonth               ] = listitem.sGatherTime.nMonth;
                    result[SearchConstants::ResultDay                 ] = listitem.sGatherTime.nDay;
                    result[SearchConstants::ResultHour                ] = listitem.sGatherTime.nHour;
                    result[SearchConstants::ResultMinute              ] = listitem.sGatherTime.nMinute;
                    result[SearchConstants::ResultSecond              ] = listitem.sGatherTime.nSecond;
                    result[SearchConstants::ResultWeek                ] = listitem.sGatherTime.nWeek;
                    result[SearchConstants::ResultOilCompany          ] = (const char*)listitem.szOilCompany;
                    result[SearchConstants::ResultHighGasoline        ] = listitem.bHighGasoline;
                    result[SearchConstants::ResultLowestOilSt         ] = listitem.bLowestOilSt;
                    result[SearchConstants::ResultOPIGasolinePrice    ] = listitem.nOPIGasolinePrice;
                    result[SearchConstants::ResultOPIDieselPrice      ] = listitem.nOPIDieselPrice;
                    result[SearchConstants::ResultOPILPGPrice         ] = listitem.nOPILPGPrice;
                    result[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem.nOPIHiGasolinePrice;
                    result[SearchConstants::ResultOPIHiLPGPrice       ] = listitem.nOPIHiLPGPrice;
                    result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeCache;
                    values.append(QVariant::fromValue(result));
                }
            }



            QMetaObject::invokeMethod(mViewComponent, "__onResultCache", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)) );


        }
        break;

    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState: {
            QVariantMap m = note->getBody().toMap();
            bool isRg = m["isRg"].toBool();
            mIsRg = isRg;
            QMetaObject::invokeMethod(mViewComponent, "__changedRgState", Q_ARG(QVariant, isRg));
        }
        break;

    case SearchConstants::ResponseOnlineSearch:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result))
                return false;

            QString search = CommonConstants::Empty;
            static int totalCount = 0;
            int startIndex = 0;
            int errCode = 0;
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

                if (reqData.contains(CommonConstants::Error)){
                    errCode = reqData[CommonConstants::Error].value<int>();
                }
            }

            QVariantList list = data[SearchConstants::Result].value<QVariantList>();
            QVariantList values = QVariantList{};

            for (int i=0 ; i < list.size(); i++) {
                TPOI_SEARCH_LIST listitem;
                memset(&listitem, 0, sizeof(listitem));
                QVariantList result;
                SearchConstants::resultInit(result);
                result[SearchConstants::ResultIndex               ] = listitem.nIdx;
                result[SearchConstants::ResultName                ] = (const char*)list.at(i).toString().toUtf8().constData();
                result[SearchConstants::ResultStdName             ] = (const char*)list.at(i).toString().toUtf8().constData();
                result[SearchConstants::ResultAddress             ] = (const char*)listitem.szAddrName;
                result[SearchConstants::ResultTelNum              ] = (const char*)listitem.szPhoneNumber;
                result[SearchConstants::ResultPOIID               ] = listitem.nPoiID;
                result[SearchConstants::ResultAddrCode            ] = listitem.nAddrCode;
                result[SearchConstants::ResultRpFlag              ] = listitem.byRPFlag;
                result[SearchConstants::ResultDistance            ] = listitem.nDist;
                result[SearchConstants::ResultDeg                 ] = listitem.nDeg;
                result[SearchConstants::ResultKind                ] = listitem.nKind;
                result[SearchConstants::ResultX                   ] = listitem.tp.x;
                result[SearchConstants::ResultY                   ] = listitem.tp.y;
                result[SearchConstants::ResultOPIPoleCate         ] = listitem.nOPIPoleCate;
                result[SearchConstants::ResultOPIPrice            ] = listitem.nOPIPrice;
                result[SearchConstants::ResultYear                ] = listitem.sGatherTime.nYear;
                result[SearchConstants::ResultMonth               ] = listitem.sGatherTime.nMonth;
                result[SearchConstants::ResultDay                 ] = listitem.sGatherTime.nDay;
                result[SearchConstants::ResultHour                ] = listitem.sGatherTime.nHour;
                result[SearchConstants::ResultMinute              ] = listitem.sGatherTime.nMinute;
                result[SearchConstants::ResultSecond              ] = listitem.sGatherTime.nSecond;
                result[SearchConstants::ResultWeek                ] = listitem.sGatherTime.nWeek;
                result[SearchConstants::ResultOilCompany          ] = (const char*)listitem.szOilCompany;
                result[SearchConstants::ResultHighGasoline        ] = listitem.bHighGasoline;
                result[SearchConstants::ResultLowestOilSt         ] = listitem.bLowestOilSt;
                result[SearchConstants::ResultOPIGasolinePrice    ] = listitem.nOPIGasolinePrice;
                result[SearchConstants::ResultOPIDieselPrice      ] = listitem.nOPIDieselPrice;
                result[SearchConstants::ResultOPILPGPrice         ] = listitem.nOPILPGPrice;
                result[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem.nOPIHiGasolinePrice;
                result[SearchConstants::ResultOPIHiLPGPrice       ] = listitem.nOPIHiLPGPrice;
                result[SearchConstants::ResultOnline              ] = true;
                result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeNormal;
                values.append(QVariant::fromValue(result));
            }

            QMetaObject::invokeMethod(mViewComponent, "__onResultOnline", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(errCode))
                                                                    );
        }
        break;

    case SearchConstants::ResponseFavorSearch: {
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
        QList<FavoriteDtoPtr> list = data[SearchConstants::Result].value<QList<FavoriteDtoPtr>>();
        QVariantList values = QVariantList{};
        for (int i=0 ; i < list.size(); i++) {
            FavoriteDtoPtr listitem = list.at(i);
            QVariantList result;
            SearchConstants::resultInit(result);
            result[SearchConstants::ResultIndex               ] = listitem->id();
            result[SearchConstants::ResultName                ] = listitem->dest();
            result[SearchConstants::ResultStdName             ] = listitem->dest();
            result[SearchConstants::ResultAddress             ] = listitem->address();
            result[SearchConstants::ResultPOIID               ] = listitem->poiID();
            result[SearchConstants::ResultX                   ] = listitem->mapX();
            result[SearchConstants::ResultY                   ] = listitem->mapY();
            result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeFavor;
            values.append(QVariant::fromValue(result));
        }


        QMetaObject::invokeMethod(mViewComponent, "__onResultFavor", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                Q_ARG(QVariant, QVariant::fromValue(startIndex)) );

      }
      break;
    case SearchConstants::ResponseDestSearch: {
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
            QList<RecentDestinationDtoPtr> list = data[SearchConstants::Result].value<QList<RecentDestinationDtoPtr>>();
            QVariantList values = QVariantList{};
            for (int i=0 ; i < list.size(); i++) {
                RecentDestinationDtoPtr listitem = list.at(i);
                QVariantList result;
                SearchConstants::resultInit(result);
                result[SearchConstants::ResultIndex               ] = listitem->id();
                result[SearchConstants::ResultName                ] = listitem->dest();
                result[SearchConstants::ResultStdName             ] = listitem->dest();
                result[SearchConstants::ResultAddress             ] = listitem->address();
                result[SearchConstants::ResultPOIID               ] = listitem->poiID();
                result[SearchConstants::ResultX                   ] = listitem->mapX();
                result[SearchConstants::ResultY                   ] = listitem->mapY();
                result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeRcntDest;
                values.append(QVariant::fromValue(result));
            }


            QMetaObject::invokeMethod(mViewComponent, "__onResultRcntDest", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)) );

        }
        break;

    case SearchConstants::TextSearchHasNoResult:
        // 통합 검색결과가 0일 때 SearchResultView에서 UpdateSearchResultCount 호출에 의해
        // 타겟에서 키보드가 올라오므로 keyboardDown property를 변경한다.
        // 19.05.17 ymhong.
        QQmlProperty::write(mViewComponent, "keyboardDown", false);
        QMetaObject::invokeMethod(mViewComponent, "resetKeyboardHeight");
        break;

    case SearchConstants::ResponseRecentSearch: {
            auto data = note->getBody().value<QList<RecentSearchDtoPtr> >();
            QVariantMap r;
            int i = 0;
            for ( RecentSearchDtoPtr dto : data ) {
                QVariantList v;
                uint arrivalTime = 0; // TODO : reqeust rp to arrival time from MNS
                v << dto->id() << dto->keyword();
                r.insert(QString::number(i),QVariant::fromValue(v));
                i++;
            }
            //T_DEBUG("Responsed to recent search: "+QString::number(i));
            QMetaObject::invokeMethod(mViewComponent,"__onResultRecentSearch",Q_ARG(QVariant,QVariant::fromValue(r)),Q_ARG(QVariant,i));
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Name)) {
                QString objName = data[CommonConstants::Name].toString();
                if (objName != ViewRegistry::objectName(ViewRegistry::TextSearchView)) {
                      if (!isVisible())
                		return false;
                }
            }

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
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString sect = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == SettingConstants::AddressDisplayMethod ) {
                int method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                QQmlProperty::write(mViewComponent, SearchConstants::AddressType, (method == SettingConstants::AddressDisplayMethodLandNumber ) ? SearchConstants::OldAddrType : SearchConstants::NewAddrType );
            }
        }
        break;

    case SettingConstants::DialogAccepted:
        {
            auto m = note->getBody().toMap();
            auto message = m[CommonConstants::Message].toString();
            auto message1 = tr("검색어를 입력 하세요.");
            auto message2 = tr("초성 검색은 2글자 이상<br>입력하여야 가능합니다.");
            if (message.contains(message1) || message.contains(message2)) {
                QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
            }

            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            if (caller != ViewRegistry::objectName(ViewRegistry::TextSearchView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::TextSearchView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, false));
        }
        break;
     case SearchConstants::ResponseNoResult: {
            QVariantMap args = note->getBody().value<QVariantMap>();
            QString objectName = args[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::TextSearchView))
                return false;
            QMetaObject::invokeMethod(mViewComponent, "__noResult");
        }
        break;

    case KeyboardConstants::PullDownKeyboard:
        if (isVisible()) {
            pulldownKeyboard();
        }
        break;

    case KeyboardConstants::PullUpKeyboard:
        if (isVisible()) {
            pullupKeyboard();
        }
        break;

    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case SearchConstants::QueueChanged: {
            int count = note->getBody().value<int>();
            QMetaObject::invokeMethod(mViewComponent, "__queueChanged", Q_ARG(QVariant, count));
        }
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName != ViewRegistry::objectName(ViewRegistry::TextSearchView))
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


    case SearchConstants::UpdateView: {
            qDebug() << "[Perf][UpdateView]before";
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::TextSearchView))
                return false;
            qDebug() << "[Perf][UpdateView]objectName: " << objectName;
            qDebug() << "[Perf][UpdateView]after";
            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::ReFocus == it.key() ||
                    CommonConstants::Visible == it.key()) {
                    continue;
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::Visible)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, QVariant::fromValue( data[CommonConstants::Visible]) );
            }
            if (data.contains(CommonConstants::ReFocus)) {
                QQmlProperty::write( mViewComponent, CommonConstants::ReFocus, QVariant::fromValue( data[CommonConstants::ReFocus]) );
            }
        }
        break;
    case MapConstants::ResponseDriveMode:
        {
            int driveMode = note->getBody().toInt();
            bool isRg = driveMode == MapViewMode::Navi;            
            if (isVisible() && mDestData.isValid()) {
                if (isRg) {
                    QVariantMap m = mDestData.toMap();
                    QString address = m[CommonConstants::Name].toString();
                    if (address.isEmpty())
                        address = m[CommonConstants::Address].toString();
                    showConfirmDialog(address);
                } else {
                    sendByDestination(mDestData);
                    QMetaObject::invokeMethod(mViewComponent, "closeAllView");
                }
            }
            mDestData.clear();
        }
        break;

    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:   {
            bool isOnline = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedOnlineStatus", Q_ARG(QVariant, isOnline));
        }
        break;
    case ApplicationConstants::UpdatedUserProfile:
        QMetaObject::invokeMethod(mViewComponent, "__updatedUserProfile");
        break;
    case ApplicationConstants::ChangedUserProfile:
        QMetaObject::invokeMethod(mViewComponent, "__changedUserProfile");
        break;
    case SearchConstants::ResponseLockStatus: {
            bool lock = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedSyncLock", Q_ARG(QVariant, lock));
        }
        break;
    default:
        return false;
    }
    return true;
}

void TextSearchViewMediator::sendToVia(const QVariant &data)
{
    QJSValue jsVal = data.value<QJSValue>();
    int x = jsVal.property( CommonConstants::X).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();
    QString name = jsVal.property( CommonConstants::Name).toString();
    QString address = jsVal.property( CommonConstants::Address).toString();

    QVariantMap body;
    body.clear();
    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[CommonConstants::Name] = name;
    body[CommonConstants::Address] = address;
    sendNotification(RouteConstants::AddVia, body);

    body.clear();
    body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteSettingView);
    body[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::SearchResultDetailView);

    sendNotification(Constants::CreateView, body);
    sendNotification(MapConstants::UpdateMapView);
}


bool TextSearchViewMediator::sendByDestination(const QVariant &data)
{
    auto m = data.toMap();
    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    auto name = m[CommonConstants::Name].toString();
    auto address = m[CommonConstants::Address].toString();
    auto poiId = m[SearchConstants::PoiID].toInt();
    auto navSeq = m[SearchConstants::NavSeq].toInt();
    auto rpFlag = m[SearchConstants::RpFlag].toInt();

    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});

    QVariantMap body;
    body[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, body);
    body.clear();

    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[CommonConstants::Name] = name;
    body[SearchConstants::PoiID] = poiId;
    body[SearchConstants::NavSeq] = navSeq;
    body[SearchConstants::RpFlag] = rpFlag;

    sendNotification(RouteConstants::SetArrival, body);

    // navi bookmark > select address
    QVariantMap body2;
    body2.insert( CommonConstants::Name, name);
    body2.insert( CommonConstants::Address, address);
    body2.insert( CommonConstants::X, x);
    body2.insert( CommonConstants::Y, y);
    sendNotification(NavibookmarkConstants::SelectNavibookAddress,body2);

    sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });

    return true;
}

void TextSearchViewMediator::updateSearchResultCount(const QString &text, int searchCount, bool show)
{
    QVariantMap m;
    mText = text;
    mSearchResultCount = searchCount;

    m[CommonConstants::Text] = text;
    m[CommonConstants::Count] = searchCount;
    m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::TextSearchView);
    if (show)
        m[CommonConstants::Show] = true;
    sendNotification(ZoneConstants::UpdateSearchResultCount, m);
}

void TextSearchViewMediator::pulldownKeyboard()
{
    QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
    mIsPulldown = true;

#ifdef BUILD_TARGET
    PlatformHelper::instance()->setZone1(SEARCH_TEXT_2, QVariantMap{{CommonConstants::Text, mText}});
#endif
}

void TextSearchViewMediator::pullupKeyboard()
{
    auto caller = ViewRegistry::objectName(ViewRegistry::TextSearchView);

    // 아래 UpdateSearchResultCount를 통해 zone1을 설정 하므로
    // 여기서는 키보드의 Owner만 등록한다.
    // 이전에 ShowKeyboard에 Owner를 전달해 처리했지만 그랬을 경우 타겟에서는
    // zone1에 NAV_SEARCH_BAR 와 SEARCH_TEXT_1이 연달아 표시되는 문제가 있다.
    sendNotification(KeyboardConstants::NotifyKeyboardOwner, caller);

    QVariantMap m;
    m[CommonConstants::Text] = mText;
    m[CommonConstants::Count] = mSearchResultCount;
    m[CommonConstants::Caller] = caller;
    m[CommonConstants::Show] = true;
    m[CommonConstants::PullUp] = true;
    sendNotification(ZoneConstants::UpdateSearchResultCount, m);

    QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");

    mIsPulldown = false;
}

void TextSearchViewMediator::queryInDriveMode()
{
    sendNotification(MapConstants::RequestDriveMode);
}

void TextSearchViewMediator::showConfirmDialog(const QString &address)
{
    auto mDestDataMap = mDestData.toMap();

    int x = mDestDataMap[CommonConstants::X].toInt();
    int y = mDestDataMap[CommonConstants::Y].toInt();

    QVariantMap m;

    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[SearchConstants::DestX] = x;
    m[SearchConstants::DestY] = y;
    m[CommonConstants::Address] = address;
    m[SearchConstants::PoiID] = mDestDataMap[SearchConstants::PoiID];
    m[SearchConstants::NavSeq] = mDestDataMap[SearchConstants::NavSeq];
    m[SearchConstants::RpFlag] = mDestDataMap[SearchConstants::RpFlag];
    m[SearchConstants::View] = QVariant::fromValue(mViewComponent);
    m[SearchConstants::OkCallback] = QStringLiteral("closeAllView");
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

bool TextSearchViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool TextSearchViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}

bool TextSearchViewMediator::isCallerViaSetting() const
{
    auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
    return caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView);
}

bool TextSearchViewMediator::isCallerBookmark() const
{
    auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
    return caller == ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
}
