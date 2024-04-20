#include "MapSearchViewMediator.h"
#include "ApplicationConstants.h"
#include "Constants.h"
#include "MapConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "ZoneConstants.h"
#include "mediator/app/MapViewMode.h"
#include "ViewRegistry.h"
#include "CommonConstants.h"
#include "NavigationControllerConstants.h"
#include "NavibookmarkConstants.h"

//2018.09.13 - cjlee
#include "../common/searchrequest.h"
#include "SearchConstants.h"
#include "UtilConstants.h"
#include "NavigationControllerConstants.h"
#include "SoftkeyMenu.h"
#include "PlatformHelper.h"
#include "AddressConstants.h"
#include "BookmarkHelper.h"

namespace SKT {

MapSearchViewMediator::MapSearchViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{

}

QList<int> MapSearchViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        MapConstants::ReturnClicked,
        MapConstants::ResponseMapScale,
        MapConstants::ResponseMapAreaName,
        MapConstants::ResponseMapPosition,

        //[#온라인 주소 표출] 2018.09.13 cjlee
        MapConstants::RendererTouched,
        SearchConstants::ResponseOnlineSearch,

        NavigationControllerConstants::ChangeLanguage,

        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,

        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        ApplicationConstants::ChangedGadgetState,
        ApplicationConstants::ReplyToGadgetState,
        AddressConstants::ResponseAddress,
    };
    return interests;
}

bool MapSearchViewMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case AddressConstants::ResponseAddress:
        switch (mSelectedMenu) {
        case SetAsDeparture:
        case SetAsWaypoint:
        case SetAsArrival:
            addVia(noti->getBody().toString());
            break;

        case AddToBookmark:
            addToBookmark(noti->getBody().toString());
            break;
        }
        break;

    case ApplicationConstants::ChangedGadgetState:
    case ApplicationConstants::ReplyToGadgetState:
        {
            auto m = noti->getBody().toMap();
            auto isGadget = m["visible"].toBool();

            QQmlProperty::write(mViewComponent, "isGadget", isGadget);
        }
        break;
    case NavigationControllerConstants::ChangeLanguage:
        sendNotification(ZoneConstants::SelectZone1Title, tr("지도 검색"));
        QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        break;

    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:
        procSoftkey(noti->getBody());
        break;

    case MapConstants::ReturnClicked:
        sendNotification(MapConstants::ResetMapPosition, noti->getBody());
        break;

    case MapConstants::ResponseMapScale:
        {
            auto level = noti->getBody().toInt();
            if (level) {
                QMetaObject::invokeMethod(mViewComponent, "updateMapScale", Q_ARG(QVariant, level));
            }
        }
        break;

    case RgConstants::ChangedRouteGuidanceState:
    case RgConstants::ReplyToRouteGuidanceState:
        {
            auto m = noti->getBody().toMap();
            mIsRg = m["isRg"].toBool();
        }
        break;

    case MapConstants::ResponseMapPosition:
        {
            mDestPos = noti->getBody().toPoint();            
            auto address = QQmlProperty::read(mViewComponent, "address").toString();

            if (mIsRg) {
                showChangeDestinationDialog(mDestPos, address);
            } else {
                requestRoute(mDestPos, address);
            }
        }
        break;

    case MapConstants::ResponseMapAreaName:
        mMapAreaName = noti->getBody().toString();
        break;

    //[#온라인 주소 표출] 2018.09.13 cjlee
    case MapConstants::RendererTouched:
        QMetaObject::invokeMethod(mViewComponent, "__onMapTouch");
        break;

    case SearchConstants::ResponseOnlineSearch:{
            QVariantMap data = noti->getBody().value<QVariantMap>();
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

    case NavigationControllerConstants::ChangeOnlineStatus:
    case NavigationControllerConstants::ResponseOnlineStatus:
        {
            auto isOnline = noti->getBody().toBool();
            QQmlProperty::write(mViewComponent, "isOnline", isOnline);
            if (isOnline) {
                // 온라인으로 변경 시 현재 맵 위치에 대한 주소가 없을 수 있으므로 무조건 요청
                // 19.07.17 ymhong
                requestAddress();
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

void MapSearchViewMediator::closeView()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void MapSearchViewMediator::procSoftkey(const QVariant &data)
{
    QVariantMap m = data.toMap();
    if (m[CommonConstants::Name].toString() != ViewRegistry::objectName(ViewRegistry::MapSearchView))
        return;

    QString function = m[ZoneConstants::Function].toString();
    QString caller = QQmlProperty::read(mViewComponent,CommonConstants::Caller).toString();
    QString owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();

    if (function == ZoneConstants::Back) {
        // 즐겨찾기 추가를 위해 SearchTabView 에서 온 경우 close된 상태.
        if (caller != ViewRegistry::objectName(ViewRegistry::BookmarkPageView)) {
            if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                auto url = ViewRegistry::url(ViewRegistry::NaviMenuView);
                sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});
            } else {
                sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName,caller}});
            }
        } else {
            sendNotification(SearchConstants::ReturnToFavorite);
        }
    } else if (function == ZoneConstants::Menu) {
        sendNotification(MapConstants::RequestMapAreaName); // update mMapAreaName.

        if (mMapAreaName.isEmpty())
            mMapAreaName = QQmlProperty::read(mViewComponent, "address").toString();

        if (mMapAreaName.isEmpty()) {
            // do nothing.

            return;
        } else {
            QString menu = m[ZoneConstants::Menu].toString();
            auto index = -1;
            if (m.contains("index")) {
                index = m["index"].toInt();
                menu.clear();
            }
            auto isWaypointMenu = index == SetAsDeparture ||
                                  index == SetAsWaypoint ||
                                  index == SetAsArrival;

            QVariantMap m;
            m["mapView"] = QVariant::fromValue(mRenderer);
            m[CommonConstants::Address] = "";// mMapAreaName;

            mSelectedMenu = index;

            if (isWaypointMenu || menu == tr("경유지로") || menu == tr("출발지로") || menu == tr("목적지로")) {
#if (0)
                sendNotification(Constants::ShowView,QVariantMap{{CommonConstants::ObjectName, QQmlProperty::read(mViewComponent,CommonConstants::Caller)}});
                m[CommonConstants::Type] = "via";
                sendNotification(MapConstants::RequestMapPosition, m);
#else
                enableMapMove(false);

                auto pos = mapCenterPos();
                mDestPos = pos;

                sendNotification(AddressConstants::RequestAddress, QVariantMap{
                                     {CommonConstants::X, pos.x()},
                                     {CommonConstants::Y, pos.y()}
                                 });
                return;
#endif
            } else if (index == AddToBookmark || menu == tr("즐겨찾기 추가")) {
#if (0)
                QString type = CommonConstants::Favorite;
                if (owner.contains("/home"))
                    type += "/home";
                else if (owner.contains("/office"))
                    type += "/office";

                m[CommonConstants::Type] = type;
                m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::MapSearchView);
                m[CommonConstants::Owner] = caller;

                sendNotification(MapConstants::RequestMapPosition, m);
#else
                enableMapMove(false);

                auto pos = mapCenterPos();
                mDestPos = pos;

                sendNotification(AddressConstants::RequestAddress, QVariantMap{
                                     {CommonConstants::X, pos.x()},
                                     {CommonConstants::Y, pos.y()}
                                 });
#endif

                // TMCRSM-792.
                // don't close view.
                return;
            } else if (index == RequestRoute) {
                sendNotification(MapConstants::RequestMapPosition, QVariantMap{
                                     {CommonConstants::Type, "center"}
                                 });
                return;
            }
        }
    }
    closeView();
}

void MapSearchViewMediator::requestRoute(const QPoint &pos, const QString &address)
{
    sendNotification(RouteConstants::SetDeparture, QVariantMap{
                         {SearchConstants::Clear, true}
                     });

    sendNotification(RouteConstants::SetArrival, QVariantMap{
                         {CommonConstants::X, pos.x()},
                         {CommonConstants::Y, pos.y()},
                         {CommonConstants::Name, address}
                     });

    sendNotification(NavibookmarkConstants::SelectNavibookAddress, QVariantMap{
                         {CommonConstants::X, pos.x()},
                         {CommonConstants::Y, pos.y()},
                         {CommonConstants::Name, address},
                         {CommonConstants::Address, address}
                     });

    sendNotification(Constants::CreateView, QVariantMap{
                         {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)}
                     });

    closeView();
}

void MapSearchViewMediator::requestAddress()
{
    QVariantMap body;
    body.insert( SearchConstants::SearchType, SearchRequestBase::SearchAddress);
    sendNotification(SearchConstants::RequestSearch, body);
}

void MapSearchViewMediator::showChangeDestinationDialog(const QPoint &pos, const QString &address)
{
    auto m = QVariantMap{
        {CommonConstants::Url, ViewRegistry::url(ViewRegistry::ChangeDestinationView)},
        {QStringLiteral("destX"), pos.x()},
        {QStringLiteral("destY"), pos.y()},
        {CommonConstants::Address, address},
        {QStringLiteral("view"), QVariant::fromValue(mViewComponent)},
        {QStringLiteral("okCallback"), QStringLiteral("closeView")}
    };

    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

void MapSearchViewMediator::addVia(const QString &address)
{
    if (address.isEmpty()) {
        enableMapMove(true);
    } else {
        sendNotification(Constants::ShowView,QVariantMap{{CommonConstants::ObjectName, QQmlProperty::read(mViewComponent,CommonConstants::Caller)}});
        sendNotification(MapConstants::RequestMapPosition, QVariantMap{
                             {CommonConstants::Type, "via"},
                             {CommonConstants::X, mDestPos.x()},
                             {CommonConstants::X, mDestPos.y()},
                         });
        closeView();
    }
}

void MapSearchViewMediator::addToBookmark(const QString &address)
{
    if (address.isEmpty()) {
        enableMapMove(true);
    } else {
        QString type = CommonConstants::Favorite;
        QString caller = QQmlProperty::read(mViewComponent,CommonConstants::Caller).toString();
        QString owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();

        if (owner.contains("/home"))
            type += "/home";
        else if (owner.contains("/office"))
            type += "/office";

        QVariantMap m;
        m["mapView"] = QVariant::fromValue(mRenderer);
        m[CommonConstants::Address] = "";// mMapAreaName;
        m[CommonConstants::Type] = type;
        m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::MapSearchView);
        m[CommonConstants::Owner] = caller;
        sendNotification(MapConstants::RequestMapPosition, m);
    }
}

void MapSearchViewMediator::enableMapMove(bool enable)
{
    QMetaObject::invokeMethod(mViewComponent, "enableMapMove", Q_ARG(QVariant, enable));
}

QPoint MapSearchViewMediator::mapCenterPos() const
{
    auto x = QQmlProperty::read(mViewComponent, "curX").toInt();
    auto y = QQmlProperty::read(mViewComponent, "curY").toInt();

    return QPoint{x, y};
}

bool MapSearchViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
            auto m = data.toMap();
            mRenderer = m["mapView"].value<QObject*>();
            sendNotification(MapConstants::UpdateMapViewPadding);
            sendNotification(NavigationControllerConstants::QueryOnlineStatus);
            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            sendNotification(MapConstants::RequestMapScale, QVariantMap{
                                 {QStringLiteral("mapView"), QVariant::fromValue(mRenderer)}
                             });
            sendNotification(ApplicationConstants::RequestGadgetState);
        }
        break;

    case Activated: {

            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);

            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode, MapViewMode::MapControl}});
            // 지도모드를 MapControl로 변경 시 Cavata 표시됨.
            sendNotification(MapConstants::ShowCarvata, false);

            sendNotification(ZoneConstants::SelectZone1Title, tr("지도 검색"));

            sendNotification(MapConstants::EnableCallout, false);
            sendNotification(MapConstants::ShowRoute, false);
        }
        break;

    case SelectSoftkeyIndexMenu: {
        QVariantMap m = data.toMap();
        auto index = m["index"].toInt();
        auto menu = m["menu"].toString();
        QVariantList menu2;
        menu2.append(QVariant::fromValue(SoftkeyMenu{index, menu}));
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QVariantMap{
                             {"menus2", menu2}
                         });
        break;
    }

    case Deactivated:
        sendNotification(MapConstants::EnableCallout, true);
        sendNotification(ZoneConstants::SelectSoftkeyMenuMode, QStringList{});
        break;

    case MapScaleReturnButtonClicked:
        sendNotification(MapConstants::ReturnClicked, data);
        break;

    //[#온라인 주소 표출]
    case RequestSearch:
        requestAddress();
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case SelectCsw:
        sendNotification(NavigationControllerConstants::SelectCSW, data);
        break;

    case MoveMap:
        {
            auto m = data.toMap();
            auto x = m["x"].toInt();
            auto y = m["y"].toInt();
            auto screenSize = 50;
            m["x"] = x * screenSize;
            m["y"] = y * screenSize;

            sendNotification(MapConstants::MoveMapByScreenPos, m);
        }
        break;

    case MoveMapByPos:
        {
            sendNotification(MapConstants::MoveMapByWorldPos, data);
        }
        break;

    default:
        return false;
    }

    return true;
}

} // SKT
