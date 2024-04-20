#include "TestBookmarkPageViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ViewRegistry.h"
#include "FavoriteDto.h"
#include "NavigationPropertyProtocol.h"
#include "rprequest.h"
#include "NavigationPropertyConstants.h"
#include "mediator/app/MapViewMode.h"

using namespace SKT;

TestBookmarkPageViewMediator::TestBookmarkPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    // 17.10.24 hskim
    mEditType = -1;
    mSelectedFavoriteItem = -1;

    mIsEditMode = false;
    mIsActivate = false;
}

QList<int> TestBookmarkPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::DeleteFavorite,
        SearchConstants::ResponseSearch,
        SearchConstants::ResponseFavorites,
        SearchConstants::ResponseIsFavorite,
        ZoneConstants::DoSoftKeyFunction,
        KeyboardConstants::ProcessKeyboard,
        MapConstants::ResponseDriveMode,
        SearchConstants::ApplyTabPageActivation
    };
    return notes;
}

QString TestBookmarkPageViewMediator::deleteMenuMessage(int count) const
{
    return count ? QString("삭제(%1)").arg(count) : "삭제:disabled";
}

QString TestBookmarkPageViewMediator::deleteAllMenuMessage() const
{
    QVariant v;
    QMetaObject::invokeMethod(mViewComponent, "favoriteListCount", Qt::DirectConnection, Q_RETURN_ARG(QVariant, v));
    auto count = v.toInt();

    return count ? "전체삭제" : "전체삭제:disabled";
}

void TestBookmarkPageViewMediator::showEditFavoriteAliasDialog(const QVariant &data)
{
    QMetaObject::invokeMethod(mViewComponent, "showEditFavoriteAliasDialog", Q_ARG(QVariant, data));
}

void TestBookmarkPageViewMediator::updateEditFavoriteMenu(int checked)
{
#if (0)
    QVariantMap m;
    m[CommonConstants::Index] = 1;
    m[ZoneConstants::Menu] = deleteMenuMessage(checked);

    sendNotification(Constants::EditSoftkeyMenu, m);
#else
    QStringList l { deleteAllMenuMessage(), deleteMenuMessage(checked) };
    sendNotification(ZoneConstants::SelectSoftkeyMenuMode, l);
#endif
}

void TestBookmarkPageViewMediator::recoverMenuFromEdit()
{
    QMetaObject::invokeMethod(mViewComponent, "recoverMenuFromEdit");
    mIsEditMode = false;
}

void TestBookmarkPageViewMediator::deleteSelectedFavoriteItems(const QVariant &data)
{
#if (0)
    sendNotification(SearchConstants::DeleteFavorite, QVariantMap{{"list", data}});
#else
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
#endif

    recoverMenuFromEdit();
}

void TestBookmarkPageViewMediator::deleteAllFavoriteItems()
{
    sendNotification(SearchConstants::ClearFavorites);

    QMetaObject::invokeMethod(mViewComponent, "clearFavoriteListModel");

    // 더 이상 편집 데이터가 없으므로 이전 메뉴로 복귀.
    recoverMenuFromEdit();

    QVariantMap m;
    auto pos = new TS_ROUTE_POS;
    memset(pos, 0, sizeof(TS_ROUTE_POS));

    m["propertyValue"] = QVariant::fromValue(RoutePosPtr(pos));

    m["type"] = NavigationPropertyProtocol::TmapWorkLocation;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);

    m["type"] = NavigationPropertyProtocol::TmapHomeLocation;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
}

void TestBookmarkPageViewMediator::editFavoriteAlias(const QVariant &data)
{
    QMetaObject::invokeMethod(mViewComponent, "editFavoriteAlias", Q_ARG(QVariant, data));

    auto m = data.toMap();

    QVariantMap args;
    args[SearchConstants::DestType] = 0; //TODO
    args[SearchConstants::Id] = m[SearchConstants::Id].toInt();
    args[CommonConstants::Alias] = m[CommonConstants::Name].toString();
    sendNotification(SearchConstants::UpdateFavorite, args);

    m.clear();
    m[CommonConstants::Position] = ToastsConstants::ToastUp;
    m[CommonConstants::Message] = tr("명칭이 변경되었습니다.");
    sendNotification(ToastsConstants::ShowToast, m);
}

void TestBookmarkPageViewMediator::showChangeDestinationConfirmDialog(const QString &address)
{
    auto x = mStartRouteGuidanceMap[CommonConstants::X].toInt();
    auto y = mStartRouteGuidanceMap[CommonConstants::Y].toInt();

    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    m[QStringLiteral("okCallback")] = QStringLiteral("closeView");
    sendNotification(Constants::CreateView, m);
}

void TestBookmarkPageViewMediator::startRouteGuidance()
{
    auto m = mStartRouteGuidanceMap;
    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    auto name = m[CommonConstants::Name].toString();

    if (x > 0 && y > 0) {
        QVariantMap body;
        body[SearchConstants::Clear] = true;
        sendNotification(RouteConstants::SetDeparture, body);
        body.clear();

        body[CommonConstants::X] = x;
        body[CommonConstants::Y] = y;
        body[CommonConstants::Name] = name;

        sendNotification(RouteConstants::SetArrival, body);

        sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
        close();
    }
}

void TestBookmarkPageViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

bool TestBookmarkPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Activated:
        mIsActivate = true;
        mIsEditMode = false;
        break;
    case Deactivated:
        mIsActivate = false;
        break;
    case SelectSoftkey:
        {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyMenuMode, data);
        }
        break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case DeleteSelectedFavoriteItems:
        deleteSelectedFavoriteItems(data);
        break;
    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;
    case BackwardClicked:
//        if (!data.isNull()) {
//            QString caller = data.toString();
//            if (!caller.isEmpty()) {
//                T_DEBUGV("SearchTabViewMediator::onEvent(BackwardClicked) : %s", data.toString().toStdString().c_str());
//                QString caller = QQmlProperty::read(mViewComponent,CommonConstants::Caller).toString();
//                if (caller == ViewRegistry::objectName(ViewRegistry::AddressSearchView))
//                    caller = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
//                sendNotification(Constants::ShowView,QVariantMap{{CommonConstants::ObjectName,caller}});
//            }
//        }
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput);
        break;
    case ActivatedForNavi:
        {
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
        }
        break;
    case CloseView:
        // Deactivate 이벤트가 발생하지 않으므로 CSW처리는 이곳에서 함
        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(Constants::CloseView, data);
        break;
    case EditFavoriteClicked:
        {
            mIsEditMode = true;
            mEditType = 2; // favorite

            auto m = data.toMap();
            mSelectedFavoriteItem = m[SearchConstants::Checked].toInt();

            QStringList l;
            l << deleteAllMenuMessage() << deleteMenuMessage(mSelectedFavoriteItem);

            sendNotification(ZoneConstants::SelectSoftkeyMenuMode, l);
        }
        break;
    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case AddVia:
        {
            sendNotification(RouteConstants::AddVia, data);
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;
    case QueryInDriveMode:
        mStartRouteGuidanceMap = data.toMap();
        sendNotification(MapConstants::RequestDriveMode);
        break;
    case EditFavoriteAliasClicked:
        // 명칭 변경 다이얼로그를 띄움
        showEditFavoriteAliasDialog(data);
        break;
    case EditFavoriteAlias:
        // 명칭 변경 다이얼로그에서 확인을 누르면 반영.
        editFavoriteAlias(data);
        // 즐겨찾기 UI에 명칭변경이 반영 되도록.
        sendNotification(SearchConstants::UpdateBookmarkPageView);

        recoverMenuFromEdit();
        break;
    case EditFavoriteDone:
        // 키보드 편집 중 뒤로가기를 눌렀을 때 편집 모드 종료를 설정.
        // Softkey back을 두번 눌러야 종료되는 문제 때문.
        mIsEditMode = false;
        break;
    case DeleteAllFavoriteItems:
        deleteAllFavoriteItems();
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;
    case FavoriteDetailClicked:
        {
            sendNotification(NavigationControllerConstants::ResetCSW);

            auto m = data.toMap();
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchResultDetailView);
            m[CommonConstants::Owner] = ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
            m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::SearchTabView);
            sendNotification(Constants::CreateView, m);

            QQmlProperty::write( mViewComponent, QStringLiteral("visible"), false );
        }
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
        //T_DEBUGV( "RequestFavorite" );
        sendNotification(SearchConstants::RequestFavorites, data);
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal, data.toMap());
        break;

    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;
    }

    return false;
}

bool TestBookmarkPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ApplyTabPageActivation:
        {
            auto m = note->getBody().toMap();
            auto page = m[SearchConstants::Page].toInt();
            T_DEBUGV( "ApplyTabPageActivation #1 page: %d", page );
            QQmlProperty::write(mViewComponent, CommonConstants::Visible, page == 2);
        }
        break;
    case SearchConstants::DeleteFavorite:
        {
            auto m = note->getBody().toMap();
            if (m.contains("x") && m.contains("y")) {
                auto x = m["x"].toInt();
                auto y = m["y"].toInt();
                QVariant id;
                QMetaObject::invokeMethod(mViewComponent, "deleteFavoriteByPos", Q_RETURN_ARG(QVariant, id), Q_ARG(QVariant, x), Q_ARG(QVariant, y));
            }
        }
        break;

    case SearchConstants::ResponseSearch:
        break;

    case SearchConstants::ResponseFavorites:
        {
            //T_DEBUGV( "ResponseFavorites" );

            // 17.10.24 hskim comment the below checking caller routines because it has no difference regardeless of whether it is used in search tab view or as a standalone
            //auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
            //if (caller == "RouteSettingView") {
                auto data = note->getBody().value<QList<FavoriteDtoPtr> >();
                int i = 0;
                QVariantList l;
                for ( FavoriteDtoPtr dto : data )
                {
                    QVariantMap m;
                    m[SearchConstants::Id] = dto->id();
                    m[CommonConstants::Address] = dto->address();
                    m[CommonConstants::Name] = dto->dest();
                    m[CommonConstants::Alias] = dto->alias();
                    m[CommonConstants::X] = dto->mapX().toInt();
                    m[CommonConstants::Y] = dto->mapY().toInt();
                    m[SearchConstants::DestType] = dto->destType();

                    l.append(m);
                    i++;
                }

                QMetaObject::invokeMethod(mViewComponent, "pushFavoriteList", Q_ARG(QVariant, l), Q_ARG(QVariant, l.count()));
            //}
        }
        break;

    case KeyboardConstants::ProcessKeyboard: {
         auto args = note->getBody().value<QVariantMap>();
         // 17.10.25 hskim check owner
         auto owner = args[CommonConstants::Owner].toString();
         auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
         auto bookmarkPageView = ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
         if (owner != objectName && owner != bookmarkPageView)
             return false;
         auto key = args[CommonConstants::Key];
         auto text = args[CommonConstants::Text];
         QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
     }
     break;

    case ZoneConstants::DoSoftKeyFunction:{
            T_DEBUGV( "DoSoftKeyFunction Back #0" );

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

                T_DEBUGV( "DoSoftKeyFunction Back #1 mIsEditMode: %d mEditType: %d", mIsEditMode, mEditType );

                if (ZoneConstants::Back == objFunction && mIsEditMode) {
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
    case MapConstants::ResponseDriveMode:
        {
            if (mIsActivate) {
                auto driveMode = note->getBody().toInt();
                if (driveMode == MapViewMode::Navi) {
                    auto address = mStartRouteGuidanceMap[CommonConstants::Name].toString();
                    if (address.isEmpty())
                        address = mStartRouteGuidanceMap[CommonConstants::Address].toString();

                    showChangeDestinationConfirmDialog(address);
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
    default:
        return false;
    }
    return true;
}
