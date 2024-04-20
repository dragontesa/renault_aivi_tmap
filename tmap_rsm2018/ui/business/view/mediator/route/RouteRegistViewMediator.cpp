#include "RouteRegistViewMediator.h"

#include <QMetaObject>
#include <QQmlProperty>

#include "ApplicationConstants.h"
#include "Constants.h"
#include "HiddenSettingConstants.h"
#include "MapConstants.h"
#include "PlatformHelper.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "ZoneConstants.h"
#include "RouteConstants.h"
#include "NavigationControllerConstants.h"
#include "mediator/app/MapViewMode.h"
#include "ViewRegistry.h"

namespace SKT {

RouteRegistViewMediator::RouteRegistViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
}

RouteRegistViewMediator::~RouteRegistViewMediator()
{
}

QList<int> RouteRegistViewMediator::listNotificationInterests()
{
    return QList<int> {
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::CswBackTriggered,
    };
}

bool RouteRegistViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated:
        {
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification(SettingConstants::RequestHiddenSetting, m);
        }
        break;

    case CancelClicked:
        // 취소시 기본값(GPS 0x0)으로 되돌린다.
        if (isStartPos()) {
            sendNotification(RouteConstants::SetStartCode, RouteConstants::EXPLORER_GPS_START);
        }
        closeView();
        break;

    case SearchItemClicked:
        onViaItemClicked(data);
        break;

    case ModalChanged:
        updateModal(data);
        break;

    case RegistCSW:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case CaptureScreen:
        sendNotification(HiddenSettingConstants::CaptureScreenshot);
        break;

    default:
        return false;
    }

    return true;
}

bool RouteRegistViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case SettingConstants::ResponseHiddenSetting:
        {
            auto m = note->getBody().toMap();
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
                auto state = note->getBody().toInt();
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

void RouteRegistViewMediator::closeView()
{
    QMetaObject::invokeMethod(mViewComponent, "close");

    updateModal(QVariantMap{{CommonConstants::Modal, false}});
}

void RouteRegistViewMediator::onViaItemClicked(const QVariant &data)
{
    QString caller = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
    auto type = data.toMap()["index"].toInt();

    if (type != 6) {
        // 경유지로 돌아왔을 때 MapView의 크기의 변경이 있어야 drawMBRAll이 잘 적용 되는 듯 함.
        // 경유지 추가 각 화면 이동 전 MapView의 크기를 초기화 시킴
        // 17.09.21 ymhong.
        sendNotification(MapConstants::UpdateMapViewPadding);
        sendNotification(MapConstants::UpdateMapView, 0);
    }

    // 출발지 탐색 구분 코드 설정
    if (isStartPos()) {
        checkStartCode(type);
    }

    switch (type) {
    case 0:
        { // unity search
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
        }
        break;

    case 1: // recent destination
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RecentDestinationView);
            m[CommonConstants::Caller] = caller;
            sendNotification(Constants::CreateView, m);
        }
        break;

    case 2: // book mark
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::BookmarkView);
            m[CommonConstants::Caller] = caller;
            sendNotification(Constants::CreateView, m);
#if (0) // 굳이 없어도 BookmarkPageView에서 Mediated될 때 호출 함. 19.07.22 ymhong
            m.clear();
            m[CommonConstants::Max] = 20;
            m[CommonConstants::Offset] = 0;
            sendNotification(SearchConstants::RequestFavorites, m);
#endif
        }
        break;

    case 3: // around
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchAroundView);
            m[CommonConstants::Caller] = caller;
            sendNotification(Constants::CreateView, m);
        }
        break;

    case 4:
        { // addreess
            QVariantMap body;
            body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::AddressSearchView);
            body[CommonConstants::Caller] = caller;//"AddressSearchView";
            sendNotification(Constants::CreateView, body);
        }
        break;

    case 5: // industry search 업종 검색
        {
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::IndustrySearchView);
            m[CommonConstants::Caller] = caller;
            sendNotification(Constants::CreateView, m);
        }
        break;

    case 6:
        { // map
            auto title = QQmlProperty::read(mViewComponent, "titleText").toString();
            QString menuText;
            int type;
            if (title == tr("출발지 등록")) {
                menuText = tr("출발지로");
                type = 0;
            } else if (title == tr("경유지 등록")) {
                menuText = tr("경유지로");
                type = 1;
            } else {
                menuText = tr("목적지로");
                type = 2;
            }

            QVariantMap body;
            body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::MapSearchView);
            body[CommonConstants::Caller] = caller;
            body["menuText"] = menuText;
            body[CommonConstants::Type] = type;
            sendNotification(Constants::CreateView, body);
        }
        break;

    }//switch
    hideRouteSettingView();
    closeView();
}

void RouteRegistViewMediator::hideRouteSettingView()
{
    QString routeSettingView = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
    sendNotification(Constants::HideView, QVariantMap{{CommonConstants::ObjectName, routeSettingView}});
}

void RouteRegistViewMediator::updateModal(const QVariant &v)
{
    auto m = v.toMap();
    auto modal = m[CommonConstants::Modal].toBool();
    sendNotification(ApplicationConstants::EnableBlur, modal);//+blur//

    if (modal) {
        PlatformHelper::instance()->showView(APP_POPUP_VIEW_NAME, "RouteRegistViewMediator");
    } else {
        PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
    }

    sendNotification(ZoneConstants::UpdateModal, v);
}

void RouteRegistViewMediator::checkStartCode(int type)
{
    RouteConstants::StartSearchCode startCode;
    switch (type) {
    case 0: // 통합 검색
    case 3: // 주변 검색
    case 5: // 업종 검색
        startCode = RouteConstants::EXPLORER_NAME_START;
        break;
    case 1: // 최근길
        startCode = RouteConstants::EXPLORER_RECENT_START;
        break;
    case 2: // 즐겨찾기
        startCode = RouteConstants::EXPLORER_BOOKMARK_START;
        break;
    case 4: // 주소 검색
        startCode = RouteConstants::EXPLORER_ADDRESS_START;
        break;
    case 6: // 지도 검색
        startCode = RouteConstants::EXPLORER_MAP_START;
        break;
    }
    sendNotification(RouteConstants::SetStartCode, startCode);
}

bool RouteRegistViewMediator::isStartPos() const
{
    auto viaIndex = QQmlProperty::read(mViewComponent, "visIndex").toInt();

    return 0 == viaIndex;
}

} //SKT
