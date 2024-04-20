#include "ChangeDestinationViewMediator.h"

#include "ApplicationConstants.h"
#include "HiddenSettingConstants.h"
#include "SearchConstants.h"
#include "RouteConstants.h"
#include "ViewRegistry.h"
#include "RgConstants.h"
#include "PlatformHelper.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "NavibookmarkConstants.h"
#include "NavibookmarkProtocol.h"
#include "NavigationControllerConstants.h"

namespace SKT
{

ChangeDestinationViewMediator::ChangeDestinationViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
    , mNeedUpdateModalFalse(false)
{
#ifndef BUILD_TARGET
    QQmlProperty::write(mViewComponent, "modal", true);
#else
    sendUpdateModal(true);
#endif
}

ChangeDestinationViewMediator::~ChangeDestinationViewMediator()
{
    if (mNeedUpdateModalFalse) {
        sendUpdateModal(false);
    }
    sendNotification(ApplicationConstants::BlockSearchTabView, false);
    PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
}

QList<int> ChangeDestinationViewMediator::listNotificationInterests()
{
    return QList<int> {
        RouteConstants::ResponseRouteAsDestination,
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::CswBackTriggered,
        ApplicationConstants::SystemPopupCreated,
        ApplicationConstants::SystemPopupClosed,
        NavigationControllerConstants::SpcxRouteGuidanceStarted
    };
}

bool ChangeDestinationViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated:
        {
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification(SettingConstants::RequestHiddenSetting, m);
        }
        break;

    case Activated:
        if (!mPopupCreated) {
            PlatformHelper::instance()->showView(APP_POPUP_VIEW_NAME, "ChangeDestinationViewMediator");
            QMetaObject::invokeMethod(mViewComponent, "showAddress");
            mPopupCreated = true;
        }
        break;

    case UpdateModal:
        sendUpdateModal(true);
        break;

    case Accepted:
        onAccepted(data);
        break;

    case Canceled:
        {
            sendUpdateModal(false);

            auto view = QQmlProperty::read(mViewComponent, "view").value<QObject*>();
            auto cancelCallback = QQmlProperty::read(mViewComponent, "cancelCallback").toString();
            if (view && !cancelCallback.isEmpty()) {
                QMetaObject::invokeMethod(view, cancelCallback.toLatin1());
            }
        }
        break;

    case RegistCSW:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case CaptureScreen:
        sendNotification(HiddenSettingConstants::CaptureScreenshot);
        break;
    }

    return true;
}

bool ChangeDestinationViewMediator::handleNotification(const mvc::INotificationPtr &note)
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

    case RouteConstants::ResponseRouteAsDestination:
        if (note->getBody().toBool()) {
            // 통합 검색 화면내(최근 검색어, 최근 목적지, 즐겨찾기)에서 목적지 변경으로
            // 경로 안내를 할 경우 가젯 모드를 취소한다.
            sendNotification(SearchConstants::SetGadgetSearchMode, false);

            QVariantMap body2;
            body2.insert(CommonConstants::Type, NavibookmarkProtocol::TransactionMap);
            body2.insert(SearchConstants::SearchType, NavibookmarkProtocol::MapSearch);
            sendNotification(NavibookmarkConstants::NewNavibookSession, body2);

            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteCandidateView);
            sendNotification(Constants::CreateView, m);

            auto view = QQmlProperty::read(mViewComponent, "view").value<QObject*>();
            auto okCallback = QQmlProperty::read(mViewComponent, "okCallback").toString();
            if (view && !okCallback.isEmpty()) {
                QMetaObject::invokeMethod(view, okCallback.toLatin1());
            }
        }
        sendUpdateModal(false);
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

    case ApplicationConstants::SystemPopupCreated:
        QMetaObject::invokeMethod(mViewComponent, "close");
        break;

    case ApplicationConstants::SystemPopupClosed:
        break;

    case NavigationControllerConstants::SpcxRouteGuidanceStarted:
        onSpcxRouteGuidanceStarted();
        break;
    }
    return true;
}

void ChangeDestinationViewMediator::sendUpdateModal(bool modal)
{
    sendNotification(ApplicationConstants::EnableBlur, modal);//+blur//

    sendNotification(ZoneConstants::UpdateModal, QVariantMap{{QStringLiteral("modal"), modal}});
    mNeedUpdateModalFalse = (modal == true);
    if (modal) {
        if (!mUpdateModalTrueCalled) {

            mUpdateModalTrueCalled = true;
        }
    } else {

    }
}

void ChangeDestinationViewMediator::onAccepted(const QVariant& v)
{
    // 경로 요약 화면이 열려 있는 경우 닫는다.
    // RecentTileGridView 사양
    QVariantMap m;
    m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
    sendNotification(Constants::CloseView, m);

    // 목적지 변경시에는 SetDeparture처리를 RouteConstants::SetArrival에서 한다.
    // 19.02.28 ymhong
//    sendNotification(RouteConstants::SetDeparture, QVariantMap{{SearchConstants::Clear, true}});

    m = v.toMap();
    m["changeDestination"] = true;
    sendNotification(RouteConstants::SetArrival, m);
    sendNotification(RouteConstants::RequestRouteAsDestination);

    // 검색 결과, 검색 결과 상세에서 호출 된 경우
    // 검색어 초기화를 해주어야 함.
    // 19.10.22 ymhong
    sendNotification(ZoneConstants::UpdateSearchText, "");
}

void ChangeDestinationViewMediator::onSpcxRouteGuidanceStarted()
{
    //QMetaObject::invokeMethod(mViewComponent, "changeDestinationRejected"); //+spcx
}

}
