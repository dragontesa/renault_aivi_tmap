#include "BookmarkRegistViewMediator.h"
#include <QMetaObject>

#include "ApplicationConstants.h"
#include "Constants.h"
#include "HiddenSettingConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "mediator/app/MapViewMode.h"
#include "PlatformHelper.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include "ZoneConstants.h"
#include "NavigationControllerConstants.h"

namespace SKT {

BookmarkRegistViewMediator::BookmarkRegistViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
}

BookmarkRegistViewMediator::~BookmarkRegistViewMediator()
{
    PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
}

QList<int> BookmarkRegistViewMediator::listNotificationInterests()
{
    return QList<int>{
        ApplicationConstants::ReplyToViewList,
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::CswBackTriggered,
    };
}

bool BookmarkRegistViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated:
        {
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification(SettingConstants::RequestHiddenSetting, m);
        }
        break;

    case Activated: {
            sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));
            QVariantMap body;
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);

            if (!mPopupCreated) {
                PlatformHelper::instance()->showView(APP_POPUP_VIEW_NAME, "BookmarkRegistViewMediator");
                mPopupCreated = true;
            }
        }
        break;

    case Deactivated:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case CancelClicked:
        sendNotification(SearchConstants::CancelAddFavorite);
        closeView();
        break;

    case SearchItemClicked:
        {
            onFavoriteItemClicked(data);
            auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
            auto naviMenuView = ViewRegistry::objectName(ViewRegistry::NaviMenuView);
            if (caller == naviMenuView) {
                sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, naviMenuView}});
            }
        }
        break;

    case ModalChanged:
        updateModal(data);
        break;

    case CaptureScreen:
        sendNotification(HiddenSettingConstants::CaptureScreenshot);
        break;

    default:
        return false;
    }

    return true;
}

bool BookmarkRegistViewMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case ApplicationConstants::ReplyToViewList:
        {
            auto data = noti->getBody().toMap();
            if (!data.contains(CommonConstants::ObjectName)) {
                break;
            }
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            QString requestObjectName = data[CommonConstants::ObjectName].value<QString>();
            if (requestObjectName == ViewRegistry::objectName(ViewRegistry::SimulationView)  ||
                requestObjectName == ViewRegistry::objectName(ViewRegistry::RouteCandidateView) ||
                requestObjectName == objectName) {
                parseViewList(noti->getBody());
            }
        }
        break;

    case SettingConstants::ResponseHiddenSetting:
        {
            auto m = noti->getBody().toMap();
            auto section = m[CommonConstants::Section].toString();
            if (section == HiddenSettingConstants::HiddenSetting) {
                auto settings = m[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
                for (auto setting : settings) {
                    auto key = setting->key().toString();
                    if (key == HiddenSettingConstants::UseScreenCapture) {
                        auto enabled = setting->value().toBool();
                        QQmlProperty::write(mViewComponent, "captureEnable", enabled);
                        break;
                    }
                }
            }
        }
        break;

    case ZoneConstants::CswBackTriggered:
        {
            auto activated = QQmlProperty::read(mViewComponent, "active").toBool();
            if (activated) {
                auto state = noti->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    QMetaObject::invokeMethod(mViewComponent, "goBack");
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;
    }
    return false;
}

void BookmarkRegistViewMediator::onFavoriteItemClicked(const QVariant &data)
{
    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();

    auto bookmarkType = data.toInt();

    switch (bookmarkType) {
    case 0: // unity search
        {
            QVariantMap m;

            m.clear();
            m[CommonConstants::Url        ] = ViewRegistry::url(ViewRegistry::TextSearchView);
            m[CommonConstants::Caller     ] = caller;
            m[CommonConstants::Owner      ] = owner;
            m[CommonConstants::Visible    ] = SearchConstants::AlwaysShow;

            //m[CommonConstants::Z          ] = CommonConstants::MAX_INT_VALUE;
            m[CommonConstants::ReFocus    ] = true;
            m[CommonConstants::ObjectName ] = ViewRegistry::objectName(ViewRegistry::TextSearchView);
            sendNotification(SearchConstants::UpdateView, m);

            sendNotification(ZoneConstants::UpdateSearchText, "");
            sendNotification(SearchConstants::ClearTextSearchZone1);

            sendNotification(Constants::CreateView, m);

            closeView();
        }
        break;

    case 1: // recent destination
        {
            // 최근 목적지 화면 작성 필요.
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RecentDestinationView);
            m[CommonConstants::Caller] = caller;
            m[CommonConstants::Owner] = owner;
            sendNotification(Constants::CreateView, m);

            closeView();
        }
        break;

    case 2: // around
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchAroundView);
            m[CommonConstants::Caller] = caller;
            m[CommonConstants::Owner] = owner;
            sendNotification(Constants::CreateView, m);

            closeView();
        }
        break;

    case 3: // addreess
        {
            // 화면은 작성되어 있음.
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::AddressSearchView);
            m[CommonConstants::Caller] = caller;
            m[CommonConstants::Owner] = owner;
            sendNotification(Constants::CreateView, m);

            closeView();
        }
        break;

    case 4: // up jong
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::IndustrySearchView);
            m[CommonConstants::Caller] = caller;
            m[CommonConstants::Owner] = owner;
            sendNotification(Constants::CreateView, m);

            closeView();
        }
        break;

    case 5: // map
        {
            QVariantMap body;
            body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::MapSearchView);
            body[CommonConstants::Caller] = caller;
            body[CommonConstants::Owner] = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
            sendNotification(Constants::CreateView, body);
            closeView();

            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            sendNotification(MapConstants::ShowCarvata, false);//+
        }
        break;

    }//switch
    closeBookmark(bookmarkType);
}

void BookmarkRegistViewMediator::parseViewList(const QVariant &v)
{
    auto m = v.toMap();
    mViewList = m[ApplicationConstants::ViewIdList].value<QList<int>>();
}

void BookmarkRegistViewMediator::closeView()
{
    QMetaObject::invokeMethod(mViewComponent, "close");

    updateModal(QVariantMap{{CommonConstants::Modal, false}});
}

void BookmarkRegistViewMediator::closeView(const QString &viewName)
{
    QVariantMap m;
    m[CommonConstants::ObjectName] = viewName;
    sendNotification(Constants::CloseView, m);

    updateModal(QVariantMap{{CommonConstants::Modal, false}});
}

void BookmarkRegistViewMediator::closeBookmark(int searchType)
{
    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
    bool checkSearchTabView = false;
    bool checkBookmarkView = false;

    switch (searchType) {
    case 0: // integrated search
    case 1: // recent destination
        break;
    case 2: // around search
    case 3: // address search
    case 4: // industry search
    case 5: // map search
        checkSearchTabView = (caller == ViewRegistry::objectName(ViewRegistry::BookmarkPageView) ||
                              caller == ViewRegistry::objectName(ViewRegistry::SearchTabView));
        break;
    }
    checkBookmarkView = mViewList.contains(ViewRegistry::BookmarkView);

    if (checkSearchTabView)
        closeView(ViewRegistry::objectName(ViewRegistry::SearchTabView));
    if (checkBookmarkView)
        closeView(ViewRegistry::objectName(ViewRegistry::BookmarkView));
}

void BookmarkRegistViewMediator::updateModal(const QVariant &v)
{
    auto m = v.toMap();
    auto modal = m[CommonConstants::Modal].toBool();
    sendNotification(ApplicationConstants::EnableBlur, modal);//+blur//

    sendNotification(ZoneConstants::UpdateModal, v);
}

} //SKT
