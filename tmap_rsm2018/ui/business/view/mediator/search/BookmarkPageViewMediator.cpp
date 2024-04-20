#include "BookmarkPageViewMediator.h"

#include <QQmlProperty>
#include <QTimer>

#include "Constants.h"
#include "ApplicationConstants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "LangConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "SearchConstants.h"
#include "RouteConstants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "ViewRegistry.h"
#include "FavoriteDto.h"
#include "NavigationPropertyProtocol.h"
#include "ProfileManager.h"
#include "rprequest.h"
#include "NavigationPropertyConstants.h"
#include "mediator/app/MapViewMode.h"
#include "PlatformHelper.h"
#include "SoftkeyMenu.h"

using namespace SKT;

BookmarkPageViewMediator::BookmarkPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    // 17.10.24 hskim
    mEditType = -1;
    mSelectedFavoriteItem = 0;

    mIsEditMode = false;
    mIsActivate = false;
}

QList<int> BookmarkPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseSearch,
        SearchConstants::ResponseFavorites,
        SearchConstants::ResponseIsFavorite,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::ChangedActiveTabPage,
        SearchConstants::UpdateBookmarkPageView,
        SearchConstants::InvokeClearFavorite,
        SearchConstants::InvokeEditFavoriteAlias,
        SearchConstants::InvokeUncheckAllFavorite,
        SearchConstants::ReplyToGadgetSearchMode,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState
    };
    return notes;
}

QString BookmarkPageViewMediator::deleteMenuMessage(int count) const
{
    auto menu = tr("삭제");
    if (count)
        menu += QString(" (%1)").arg(count);
    else
        menu += " (0):disabled";

    return menu;
}

QString BookmarkPageViewMediator::deleteAllMenuMessage() const
{
    QVariant v;
    QMetaObject::invokeMethod(mViewComponent, "favoriteListCount", Qt::DirectConnection, Q_RETURN_ARG(QVariant, v));
    auto count = v.toInt();

    return count ? tr("전체삭제") : tr("전체삭제") + ":disabled";
}

void BookmarkPageViewMediator::showDeleteAllFavoriteDialog()
{
    QVariantMap m;
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::BookmarkPageEditView);
    m[QStringLiteral("type")] = "deleteAll";
    sendNotification(Constants::CreateView, m);
}

void BookmarkPageViewMediator::showEditFavoriteAliasDialog(const QVariant &data)
{
    QVariantMap m = data.toMap();
    auto name = m[CommonConstants::Name].toString();
    auto id = m[SearchConstants::Id].toInt();

    m.clear();

    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::BookmarkPageEditView);
    m[QStringLiteral("type")] = "editAlias";
    m[CommonConstants::Name] = name;
    m[SearchConstants::Id] = id;
    sendNotification(Constants::CreateView, m);
}

void BookmarkPageViewMediator::updateEditFavoriteMenu(int checked)
{
    QStringList l { deleteAllMenuMessage(), deleteMenuMessage(checked), "noOption" };

    QVariantList vMenus;
    vMenus.append(QVariant::fromValue(SoftkeyMenu{ MenuDeleteAll, l[0]}));
    vMenus.append(QVariant::fromValue(SoftkeyMenu{ MenuDeleteSelected, l[1]}));

    QVariant v;
    QMetaObject::invokeMethod(mViewComponent, "favoriteListCountWithoutHomeAndOffice", Qt::DirectConnection, Q_RETURN_ARG(QVariant, v));

    QVariantMap m;
    m["menus2"] = vMenus;
    m["optionMenu2"] = QVariantList {};
    sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, m);
}

void BookmarkPageViewMediator::recoverMenuFromEdit()
{
    QMetaObject::invokeMethod(mViewComponent, "recoverMenuFromEdit");
    // mIsEditMode = false; // should not call here. move to the original role scope. 2018.2.6 lckxx
}

void BookmarkPageViewMediator::deleteSelectedFavoriteItems(const QVariant &data)
{
    QVariantList l;
    auto vlist = data.toList();
    int homeoffice = 0;
    for (auto v : vlist) {
        auto m = v.toMap();
        auto type = m["type"].toInt();
        auto seq = m["seq"].toInt();
        l.push_back(seq);


        if (type) {
            homeoffice = type;
            QVariantMap m;
            auto pos = new TS_ROUTE_POS;
            memset(pos, 0, sizeof(TS_ROUTE_POS));

            m["propertyValue"] = QVariant::fromValue(RoutePosPtr(pos));

            if (type == 1) {
                sendNotification(NavigationPropertyProtocol::TmapWorkLocation, m);
            } else if (type == 2) {
                sendNotification(NavigationPropertyProtocol::TmapHomeLocation, m);
            }
        }
    }
    sendNotification(SearchConstants::DeleteFavorite, QVariantMap{{"list", l}, {SearchConstants::DestType, homeoffice}});

    // recoverMenuFromEdit(); // should not call here. move to the original role scope. 2018.2.6 lckxx
}

void BookmarkPageViewMediator::deleteAllFavoriteItems()
{
    sendNotification(SearchConstants::ClearFavorites);

    QMetaObject::invokeMethod(mViewComponent, "resetFavoriteList");

    // 더 이상 편집 데이터가 없으므로 이전 메뉴로 복귀.
    recoverMenuFromEdit();

    // fixed double updatiing  TMCRSM-2412, TMCRSM-2413, Note! Navi Property 업데이트는 뷰에서 하지 않도록 한다 2019-03-11
//    QVariantMap m;
//    auto pos = new TS_ROUTE_POS;
//    memset(pos, 0, sizeof(TS_ROUTE_POS));

//    m["propertyValue"] = QVariant::fromValue(RoutePosPtr(pos));

//    m["type"] = NavigationPropertyProtocol::TmapWorkLocation;
//    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);

//    m["type"] = NavigationPropertyProtocol::TmapHomeLocation;
//    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
}

void BookmarkPageViewMediator::editFavoriteAlias(const QVariant &data)
{
    auto m = data.toMap();
    auto id = m[SearchConstants::Id].toInt();
    auto name = m[CommonConstants::Name].toString();
    auto message = tr("명칭이 변경되었습니다.");
    if (!name.isEmpty()) {
        QMetaObject::invokeMethod(mViewComponent, "editFavoriteAlias", Q_ARG(QVariant, data));

        QVariantMap args;
        args[SearchConstants::DestType] = 0; //TODO
        args[SearchConstants::Id] = id;
        args[CommonConstants::Alias] = name;
        sendNotification(SearchConstants::UpdateFavorite, args);

        QMetaObject::invokeMethod(mViewComponent, "updateItemAlias", Q_ARG(QVariant, id), Q_ARG(QVariant, name));
    } else {
        //TMCRSM-667
        message = tr("명칭을 입력하세요.");
    }

    m.clear();
    m[CommonConstants::Position] = ToastsConstants::ToastUp;
    m[CommonConstants::Message] = message;
    sendNotification(ToastsConstants::ShowToast, m);
}

void BookmarkPageViewMediator::showChangeDestinationConfirmDialog(const QString &name, const QString &address)
{
    auto x = mStartRouteGuidanceMap[CommonConstants::X].toInt();
    auto y = mStartRouteGuidanceMap[CommonConstants::Y].toInt();
    auto poiId = mStartRouteGuidanceMap[SearchConstants::PoiID].toInt();
    auto navSeq = mStartRouteGuidanceMap[SearchConstants::NavSeq].toInt();
    auto rpFlag = mStartRouteGuidanceMap[SearchConstants::RpFlag].toInt();

    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("name")] = name;
    m[SearchConstants::PoiID] = poiId;
    m[SearchConstants::NavSeq] = navSeq;
    m[SearchConstants::RpFlag] = rpFlag;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    m[QStringLiteral("okCallback")] = QStringLiteral("closeView");
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

void BookmarkPageViewMediator::startRouteGuidance()
{
    auto m = mStartRouteGuidanceMap;
    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    auto name = m[CommonConstants::Name].toString();
    auto address = m[CommonConstants::Address].toString();
    auto type = m[CommonConstants::Type].toInt(); //1:회사, 2:집

    if (x > 0 && y > 0) {
        sendNotification(SearchConstants::SetGadgetSearchMode, false);

        QVariantMap body;
        body[SearchConstants::Clear] = true;
        sendNotification(RouteConstants::SetDeparture, body);
        body.clear();

        body[CommonConstants::X] = x;
        body[CommonConstants::Y] = y;
        body[CommonConstants::Name] = name;
        body[CommonConstants::Address] = address;
        body[SearchConstants::PoiID] = m[SearchConstants::PoiID];
        body[SearchConstants::NavSeq] = m[SearchConstants::NavSeq];
        body[SearchConstants::RpFlag] = m[SearchConstants::RpFlag];

        RouteConstants::DestSearchCode destSearchCode = RouteConstants::EXPLORER_BOOKMARK_DES;
        switch (type) {
        case 1: destSearchCode = RouteConstants::EXPLORER_OFFICE_DES; break;
        case 2: destSearchCode = RouteConstants::EXPLORER_HOME_DES; break;
        }
        body[SearchConstants::DestType] = destSearchCode;

        sendNotification(RouteConstants::SetArrival, body);

        sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
        close();
    }
}

void BookmarkPageViewMediator::close()
{
    auto isViaSetting = QQmlProperty::read(mViewComponent, "isViaSetting").toBool();
    if (mIsEditMode || isViaSetting) {
        // if this instance is for 'Bookmark Edit', it must not call 'closeView' in order to not destroy SearchTabView
        QMetaObject::invokeMethod(mViewComponent, "close");
    }
    else {
        QMetaObject::invokeMethod(mViewComponent, "closeView");
    }
}

void BookmarkPageViewMediator::updateBookmarkPageView()
{
    QMetaObject::invokeMethod(mViewComponent, "requestViewData");
}

void BookmarkPageViewMediator::updateSoftkey()
{
    QVariant v;
    QStringList menus;

    QString caller = QQmlProperty::read(mViewComponent, "caller").toString();
    auto isViaSetting = caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView);
    QMetaObject::invokeMethod(mViewComponent, "favoriteListCount", Qt::DirectConnection, Q_RETURN_ARG(QVariant, v));
    auto count = v.toInt();

    if (isViaSetting) {
        // TMCRSM-787
        // 즐겨찾기를 경유지로 추가시에는 소프트키메뉴가 표시되지 않도록 처리.
    } else {
        if (count) {
            menus = QStringList { tr("즐겨찾기 추가"), tr("즐겨찾기 편집") };
        } else {
            menus = QStringList { tr("즐겨찾기 추가"), tr("즐겨찾기 편집") + ":disabled" };
        }
        QMetaObject::invokeMethod(mViewComponent, "favoriteListCountWithoutHomeAndOffice", Qt::DirectConnection, Q_RETURN_ARG(QVariant, v));
        count = v.toInt();
        if (0 == count) {
            menus.append(QStringLiteral("noOption"));
        }
    }

    auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
    sendNotification(ZoneConstants::RegistSoftkey, objectName);

    QVariantList vMenus;
    if (!isViaSetting) {
        vMenus.append(QVariant::fromValue(SoftkeyMenu{MenuAddBookmark, menus[0]}));
        vMenus.append(QVariant::fromValue(SoftkeyMenu{MenuEditBookmark, menus[1]}));
    }

    QVariantList vOptions = setupOptionMenu2(count);

    QVariantMap m;
    m["menus2"] = vMenus;
    m["optionMenu2"] = vOptions;
    sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, m);
}

QVariantList BookmarkPageViewMediator::setupOptionMenu2(int count)
{
    QVariantList ret;

    if (count > 0) {
        auto optionMenu = QQmlProperty::read(mViewComponent, "optionMenu").toString();
        auto sortByName = QQmlProperty::read(mViewComponent, "sortByName").toBool();
        auto optionMenuId = sortByName ? OptionSortByName : OptionSortByOrder;

        ret.append(QVariant::fromValue(SoftkeyMenu{optionMenuId, optionMenu}));
    }

    return ret;
}

bool BookmarkPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        // update mIsRg
        sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        break;

    case RetranslateUi:
        if (mIsActivate) {
            if (mIsEditMode) {
                updateEditFavoriteMenu(mSelectedFavoriteItem);
            } else {
                updateSoftkey();
            }
        }
        break;

    case Activated: {
            // 언어 확인
            sendNotification( NavigationControllerConstants::RequestController, QVariantMap{
                                  {"type", NavigationControllerProtocol::GetLang }
                              } );

            mIsActivate = true;
            mIsEditMode = false;
            mIsRegistMode = false;
            auto m = data.toMap();
            if (m.contains(QStringLiteral("editMode")))
                mIsEditMode = m[QStringLiteral("editMode")].toBool();

            if (!mIsEditMode) {
                sendNotification(SearchConstants::ChangedTabPageViewActivation,
                    QVariantMap{{QStringLiteral("viewId"),ViewRegistry::BookmarkPageView},
                    {QStringLiteral("active"),true}});

                updateBookmarkPageView();

                sendNotification(SearchConstants::ChangeSearchEditMode, false);
            }
            else {
                mEditType = 2; // favorite

                auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
                sendNotification(ZoneConstants::RegistSoftkey, objectName);

                mSelectedFavoriteItem = m[SearchConstants::Checked].toInt();

                updateEditFavoriteMenu(mSelectedFavoriteItem);
            }
            sendNotification(RgConstants::BlockReroute, true);
        }
        break;

    case Deactivated:
        mIsActivate = false;
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::BookmarkPageView},
                {QStringLiteral("active"),false}});

        //sendNotification(ZoneConstants::SelectSoftkeyMenuMode, QStringList{});
        break;

    case UpdateSoftkey:
        updateSoftkey();
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case DeleteSelectedFavoriteItems:
        deleteSelectedFavoriteItems(data);
        if (mIsEditMode) //  close edit view - standalon version 2018.2.6
            close();
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::BookmarkRegistView)) {
                PlatformHelper::instance()->setParentForSystemPopup(m);
                mIsRegistMode = true;
            }
            sendNotification(Constants::CreateView, m);
        }
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput,data);
        break;
    case ActivatedForNavi:
        {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyNaviMode);

            sendNotification(SearchConstants::RequestGadgetSearchMode);
            if (mIsGadgetSearchMode) {
                PlatformHelper::instance()->switchToSystem();
                sendNotification(SearchConstants::SetGadgetSearchMode, false);
            }
        }
        break;
    case CloseView:
        // Deactivate 이벤트가 발생하지 않으므로 CSW처리는 이곳에서 함
        sendNotification(NavigationControllerConstants::ResetCSW);
        if (mIsGadgetSearchMode) {
            sendNotification(NavigationControllerConstants::ReserveSystemSwitchJob, QVariantMap{
                                 {CommonConstants::Command, Constants::CloseView},
                                 {CommonConstants::Arg, data}
                             });
        } else {
            sendNotification(Constants::CloseView, data);
        }
        break;
    case EditFavoriteClicked:
        {
            auto m = data.toMap();
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::BookmarkView);
            m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::BookmarkView);
            m[QStringLiteral("editMode")] = true;
            sendNotification(Constants::CreateView, m);
        }
        break;
    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case AddVia:
        {
            sendNotification(RouteConstants::AddVia, data);
            close();

            sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteSettingView)}});
        }
        break;
    case FavoriteItemSelectedTwice:
        if (!mIsEditMode) {
            // TMCRSM-119. 편집 모드에서 아이템을 두번 터치했을 때 경로 안내가 되지 않도록 수정
            // 17.11.30 ymhong
            mStartRouteGuidanceMap = data.toMap();

            if (mIsActivate) {
                auto block = mIsRg;
                sendNotification(ApplicationConstants::BlockSearchTabView, block);

                if (mIsRg) {
                    auto name = mStartRouteGuidanceMap[CommonConstants::Name].toString();
                    auto address = mStartRouteGuidanceMap[CommonConstants::Address].toString();

                    showChangeDestinationConfirmDialog(name, address);
                } else {
                    startRouteGuidance();
                    QMetaObject::invokeMethod(mViewComponent, "closeView");

                    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
                    if (owner == ViewRegistry::objectName(ViewRegistry::SearchResultView)) {
                        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, owner}});
                    }
                }
            }
        }
        break;
    case EditFavoriteAliasClicked:
        // 명칭 변경 다이얼로그를 띄움
        showEditFavoriteAliasDialog(data);
        break;

    case EditFavoriteDone:
        // 키보드 편집 중 뒤로가기를 눌렀을 때 편집 모드 종료를 설정.
        // Softkey back을 두번 눌러야 종료되는 문제 때문.
        mIsEditMode = false;
        break;

    case FavoriteDetailClicked:
        {
            if (mIsRegistMode) {
                return false;
            }
            sendNotification(NavigationControllerConstants::ResetCSW);

            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            auto isViaSetting = QQmlProperty::read(mViewComponent, "isViaSetting").toBool();

            QVariantMap m;
            if (!isViaSetting) {
                m[CommonConstants::ObjectName ] = ViewRegistry::objectName(ViewRegistry::SearchTabView);
                sendNotification(Constants::CloseView, m);
            }

            m = data.toMap();
            m[CommonConstants::Url       ] = ViewRegistry::url(ViewRegistry::SearchResultDetailView);
            //m[CommonConstants::Z         ] = CommonConstants::MAX_INT_VALUE;
            m[CommonConstants::ReFocus   ] = true;
            m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::SearchResultDetailView);
            m[SearchConstants::SearchType] = CommonConstants::Name;
            m[CommonConstants::Visible   ] = SearchConstants::AlwaysShow;//HideToShow;
            if (isViaSetting) {
                m[CommonConstants::Caller] = caller;
                m[CommonConstants::Owner ] = ViewRegistry::objectName(ViewRegistry::BookmarkView);

                sendNotification(SearchConstants::UpdateView, m);
                sendNotification(Constants::CreateView, m);

                m.clear();
                m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::BookmarkView);
                sendNotification(Constants::HideView, m);
            } else {
                m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::SearchTabView);
                m[CommonConstants::Owner] = ViewRegistry::objectName(ViewRegistry::BookmarkPageView);

                sendNotification(SearchConstants::UpdateView, m);
                sendNotification(Constants::CreateView, m);
            }
        }
        break;

    case FavoriteItemPressed:
        sendNotification(ApplicationConstants::BlockSearchTabView, true);
        break;

    case FavoriteListScrolled:
    case FavoriteItemReleased:
        sendNotification(ApplicationConstants::BlockSearchTabView, false);
        break;

    case FavoriteItemChecked:
        ++mSelectedFavoriteItem;
        updateEditFavoriteMenu(mSelectedFavoriteItem);
        break;

    case FavoriteItemUnchecked:
        --mSelectedFavoriteItem;
        updateEditFavoriteMenu(mSelectedFavoriteItem);
        break;

    case RequestFavorite:
        mIsRequested = true;
        sendNotification(SearchConstants::RequestFavorites, data);
        break;

    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;

    case DeleteAllClicked:
        showDeleteAllFavoriteDialog();
        break;

    case ShowMaxFavoriteDialog:
        {
            QVariantMap m;
            m[QStringLiteral("itemType")] = QStringLiteral("favorite");
            m[QStringLiteral("code")] = 1;
            sendNotification(SearchConstants::AddItemFailure, m);
        }
        break;

    case BackToViaSetting:
        backToViaSetting();
        break;
    case HideZone3:
        //sendNotification(ZoneConstants::HideZone3, ZoneConstants::Animation);
        break;
    case ShowZone3:
        //sendNotification(ZoneConstants::ShowZone3, ZoneConstants::Animation);
        break;
    case FocusCsw:
        sendNotification(NavigationControllerConstants::SelectCSWByObjectName, data);
        break;
    }

    return false;
}

bool BookmarkPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case NavigationControllerConstants::ChangeLanguage:
        if (mIsActivate) {
            auto m = note->getBody().toMap();
            auto lang = m[LangConstants::Lang].toString();
            QQmlProperty::write( mViewComponent, "lang", QVariant::fromValue(lang) );
            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        }
        break;

    case NavigationControllerConstants::ResponseController: {
            auto m = note->getBody().toMap();
            if (m.contains(CommonConstants::Lang)) {
                auto value = m[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "lang", QVariant::fromValue(value) );
            }
        }
        break;

    case SearchConstants::ChangedActiveTabPage:
        {
            auto m = note->getBody().toMap();
            auto page = m[SearchConstants::Page].toInt();
            T_DEBUGV( "ApplyTabPageActivation #1 page: %d", page );
            QQmlProperty::write(mViewComponent, CommonConstants::Visible, page == 2);
        }
        break;

    case SearchConstants::InvokeClearFavorite:
        if (mIsEditMode) {
            // close edit view - standalon version 2018.2.6
            // 즐겨 찾기 편집 창
            close();
        } else {
            // 즐겨 찾기 화면
            deleteAllFavoriteItems();
            updateBookmarkPageView();
            updateSoftkey();
        }
        mIsEditMode = false;
        break;

    case SearchConstants::InvokeEditFavoriteAlias:
        // 명칭 변경 다이얼로그에서 확인을 누르면 반영.
        if (mIsEditMode)
            editFavoriteAlias(note->getBody());
        // 즐겨찾기 UI에 명칭변경이 반영 되도록.
        //updateBookmarkPageView();
        //updateSoftkey();

//        recoverMenuFromEdit();
//        if (mIsEditMode) //  close edit view - standalon version 2018.2.6
//            close();
//        mIsEditMode = false;
        break;

    case SearchConstants::InvokeUncheckAllFavorite:
        recoverMenuFromEdit();
        if (mIsEditMode) //  close edit view - standalon version 2018.2.6
            close();
        mIsEditMode = false;
        break;

    case SearchConstants::ResponseSearch:
        break;

    case SearchConstants::ResponseFavorites:
        {
            if (mIsRequested) {
                auto data = note->getBody().value<QList<FavoriteDtoPtr> >();
                int i = 0;
                QVariantList l;
                QVariantMap m;
                for ( FavoriteDtoPtr dto : data )
                {
                    m.clear();

                    m[SearchConstants::Id] = dto->id();
                    m[CommonConstants::Address] = dto->address();
                    m[CommonConstants::Name] = dto->dest();
                    m[CommonConstants::Alias] = dto->alias();
                    m[CommonConstants::X] = dto->mapX().toInt();
                    m[CommonConstants::Y] = dto->mapY().toInt();
                    m[SearchConstants::DestType] = dto->destType();
                    m[SearchConstants::PoiID]  = dto->poiID();
                    m[SearchConstants::NavSeq] = dto->navSeq();
                    m[SearchConstants::RpFlag] = dto->rpFlag();
                    m[SearchConstants::OpiPoleCate] = dto->opiPoleCate();

                    l.append(m);
                    i++;
                }

#ifdef QT_DEBUG
#if (0)
                // 즐겨찾기 DB 가득 채우기.
                static bool makeFullDB = false;
                if (!makeFullDB) {
                    for ( ; i<TMAP_UI_FAVORITE_MAX; ++i) {
                        m[SearchConstants::Dest] = QString("test-%1").arg(i);
                        m[CommonConstants::X] = 0x10000+i;
                        m[CommonConstants::Y] = 0x10000+i;
                        m[SearchConstants::DestType] = 0;
                        m[SearchConstants::PoiID] = 0x10000+i;

                        sendNotification(SearchConstants::AddFavorite, m);

                        l.append(m);
                    }
                    makeFullDB = true;
                }
#endif
#endif

                QMetaObject::invokeMethod(mViewComponent, "pushFavoriteList", Q_ARG(QVariant, l), Q_ARG(QVariant, l.count()));

                updateSoftkey();

                mIsRequested = false;
            }
        }
        break;

    case SearchConstants::ReplyToGadgetSearchMode:
        mIsGadgetSearchMode = note->getBody().toBool();
        break;

    case ZoneConstants::DoSoftkeyIndexMenuFunction:
        {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            auto m = note->getBody().toMap();
            auto name = m[CommonConstants::Name].toString();
            if (objectName != name) {
                return false;
            }

            auto index = m["index"].toInt();
            switch (index) {
            case MenuAddBookmark:
            case MenuEditBookmark:
            case MenuDeleteAll:
            case MenuDeleteSelected:
            case OptionSortByName:
            case OptionSortByOrder:
                QMetaObject::invokeMethod(mViewComponent, "onSoftkeyIndexMenuFunction", Q_ARG(QVariant, index));
                if (index == MenuEditBookmark) {
                    sendNotification(SearchConstants::ChangeSearchEditMode, true);
                }
                break;
            default:
                qDebug() << "onSoftkeyIndexMenuFunction(" << index << ") : invalid value.";
                break;
            }
        }
        break;

    case ZoneConstants::DoSoftKeyFunction:{
            T_DEBUGV( "DoSoftKeyFunction Back #0" );
// Donot follow this notification if the view is hidden(prevent it might be bug), by lckxx 2017.11.14
        if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;
        
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            auto data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Name)) {
                auto objName = data[CommonConstants::Name].toString();
                if (objName != objectName)
                    return false;
            }

            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu) )
                objMenu = data[ZoneConstants::Menu].toString();

            // 17.10.27 hskim up to this line, it is the common part of old search tab view and recent desti...
            // here it is divided into two branches.

            auto isViaSettingMode = QQmlProperty::read( mViewComponent, QStringLiteral("isViaSettingMode") ).toBool();

            // 17.10.27 hskim old search tab view branch
            if ( !isViaSettingMode ) {
                // Edit 모드에서 뒤로가기 눌렀을 때의 처리.

                auto isBack = objFunction == QStringLiteral("back");

                if (isBack && mIsEditMode) {
                    mIsEditMode = false;                    
                    switch (mEditType) {
                    case 0:
                        break;
                    case 1:
                        break;
                    case 2:
                        T_DEBUGV( "DoSoftKeyFunction Back #2" );
                        recoverMenuFromEdit();
                        break;
                    }                    
                    QMetaObject::invokeMethod(mViewComponent, "close");
                } else {
                    T_DEBUGV( "DoSoftKeyFunction Back #3" );
                    QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)) );
                }
            }
            // 17.10.27 hskim old recent desti... branch
            else {
                QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)) );
            }
        }
        break;
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            auto m = note->getBody().toMap();
            mIsRg = m["isRg"].toBool();
        }
        break;

    case SearchConstants::UpdateBookmarkPageView:
        updateBookmarkPageView();
        updateSoftkey();
        break;

    default:
        return false;
    }
    return true;
}

void BookmarkPageViewMediator::backToViaSetting()
{
    QVariantMap m;
    m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
    sendNotification(Constants::ShowView, m);

    close();
}
