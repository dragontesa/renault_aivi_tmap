#include "CommonDialogViewMediator.h"

#include <QQmlProperty>

#include "ApplicationConstants.h"
#include "HiddenSettingConstants.h"
#include "MapConstants.h"
#include "PlatformHelper.h"
#include "RouteConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include "ZoneConstants.h"
#include "NavigationControllerConstants.h"
#include "SystemPopupMediator.h"

namespace SKT
{

CommonDialogViewMediator::CommonDialogViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
#ifndef BUILD_TARGET
    QQmlProperty::write(mViewComponent, "modal", true);
#endif
}

CommonDialogViewMediator::~CommonDialogViewMediator()
{
    if (mNeedUpdateModalFalse) {
        sendUpdateModal(false);
    }
}

QList<int> CommonDialogViewMediator::listNotificationInterests()
{
    return QList<int> {
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::CswBackTriggered,
        ApplicationConstants::RequestCommonDialogExist,
        ApplicationConstants::CloseCommonDialogForSystemPopup,
        ApplicationConstants::CloseCommonDialogWithoutMapUpdate,
        NavigationControllerConstants::SpcxRouteGuidanceStarted,
        NavigationControllerConstants::NavigationDisappeared,
        ApplicationConstants::RequestCommonDialogAccepted,
        ApplicationConstants::RequestCommonDailogCanceled,
    };
}

bool CommonDialogViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated:
        {
            QMetaObject::invokeMethod(mViewComponent, "showDialog");
            sendUpdateModal(true);
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification(SettingConstants::RequestHiddenSetting, m);
            mTitle = QQmlProperty::read(mViewComponent, "title").toString();
            mMessage = QQmlProperty::read(mViewComponent, "message").toString();
        }
        break;

    case Accepted:
        onAccepted(data);
        break;

    case Canceled:
        onCanceled(data);
        break;

    case MenuClicked:
        onMenuClicked(data);
        break;

    case RegistCSW:
        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case CaptureScreen:
        sendNotification(HiddenSettingConstants::CaptureScreenshot);
        break;
    }
    return true;
}

bool CommonDialogViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case NavigationControllerConstants::NavigationDisappeared:
        {
            auto isMapUpdate = isMapUpdateView();
            if (!isMapUpdate) {
                QMetaObject::invokeMethod(mViewComponent, "close");
                sendUpdateModal(false);
                sendNotification(MapConstants::ResumeMapView);
            }
        }
        break;

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
                auto menus = QQmlProperty::read(mViewComponent, "menus").toList();
                auto state = note->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    if (menus.length() > 1) {
                        QMetaObject::invokeMethod(mViewComponent, "goBack");
                    } else {
                        QMetaObject::invokeMethod(mViewComponent, "processDefaultMenu");
                    }
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;

    case ApplicationConstants::RequestCommonDialogExist:
        sendNotification(ApplicationConstants::ReplyToCommonDialogExist);
        break;

    case ApplicationConstants::CloseCommonDialogForSystemPopup:
        onSystemPopupCreated();
        break;

    case ApplicationConstants::CloseCommonDialogWithoutMapUpdate:
        {
            auto isMapUpdate = isMapUpdateView();
            if (!isMapUpdate) {
                auto thisView = ViewRegistry::objectName(ViewRegistry::CommonDialogView);
                sendNotification(Constants::CloseView, QVariantMap{
                                     {CommonConstants::ObjectName, thisView}}
                                 );
            }
        }
        break;

    case NavigationControllerConstants::SpcxRouteGuidanceStarted:
        onSpcxRouteGuidanceStarted();
        break;
    case ApplicationConstants::RequestCommonDialogAccepted: {
            auto data = note->getBody().toMap();
            onAccepted(data);
        }
        break;
    case ApplicationConstants::RequestCommonDailogCanceled: {
            auto data = note->getBody().toMap();
            onCanceled(data);
        }
        break;
    }
    return true;
}

void CommonDialogViewMediator::sendUpdateModal(bool modal)
{
    sendNotification(ZoneConstants::UpdateModal, QVariantMap{{QStringLiteral("modal"), modal}});
    mNeedUpdateModalFalse = (modal == true);

    sendNotification(ApplicationConstants::EnableBlur, modal);//+blur//

    // system popup
    auto isSystemPopup = QQmlProperty::read(mViewComponent, "isSystemPopup").toBool();
    auto viewName = isSystemPopup ? MAPUPDATE_POPUP_VIEW_NAME : APP_POPUP_VIEW_NAME;

    if (modal) {
        PlatformHelper::instance()->showView(viewName, "CommonDialogViewMediator");
    } else {
        PlatformHelper::instance()->closeView(viewName);
    }
}

void CommonDialogViewMediator::onAccepted(const QVariant& v)
{
    QVariantMap m = v.toMap();
    auto title = QQmlProperty::read(mViewComponent, "title").toString();
    auto message = QQmlProperty::read(mViewComponent, "message").toString();
    m["title"] = title;
    m["message"] = message;
    sendUpdateModal(false);
    sendNotification(SettingConstants::DialogAccepted, m);

    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    if (caller == ViewRegistry::objectName(ViewRegistry::RouteCandidateView)) {
        sendNotification(MapConstants::ResumeMapView);
        sendNotification(RouteConstants::ErrorViewClosed);
    }

    auto isSystemPopup = QQmlProperty::read(mViewComponent, "isSystemPopup").toBool();
    if (isSystemPopup) {
        auto popupType = QQmlProperty::read(mViewComponent, "popupType").toInt();
        auto messageType = QQmlProperty::read(mViewComponent, "messageType").toInt();
        sendNotification(ApplicationConstants::SystemPopupAccepted, QVariantMap{
                             {"popupType", popupType},
                             {"messageType", messageType}
                         });
    }
}

void CommonDialogViewMediator::onCanceled(const QVariant& v)
{
    QVariantMap m = v.toMap();
    auto title = QQmlProperty::read(mViewComponent, "title").toString();
    auto message = QQmlProperty::read(mViewComponent, "message").toString();
    m["title"] = title;
    m["message"] = message;
    sendUpdateModal(false);
    sendNotification(SettingConstants::DialogCanceled, m);

    auto isSystemPopup = QQmlProperty::read(mViewComponent, "isSystemPopup").toBool();
    if (isSystemPopup) {
        auto popupType = QQmlProperty::read(mViewComponent, "popupType").toInt();
        sendNotification(ApplicationConstants::SystemPopupCanceled, QVariantMap{
                             {"popupType", popupType}
                         });
    }
}

void CommonDialogViewMediator::onMenuClicked(const QVariant &v)
{
    QVariantMap m = v.toMap();
    auto title = QQmlProperty::read(mViewComponent, "title").toString();
    auto message = QQmlProperty::read(mViewComponent, "message").toString();
    m["title"] = title;
    m["message"] = message;
    sendUpdateModal(false);
    sendNotification(SettingConstants::DialogMenuClicked, m);
}

void CommonDialogViewMediator::onSystemPopupCreated()
{
    auto isSystemPopup = QQmlProperty::read(mViewComponent, "isSystemPopup").toBool();
    if (!isSystemPopup) {
        auto menus = QQmlProperty::read(mViewComponent, "menus").toStringList();
        auto cancelText = QQmlProperty::read(mViewComponent, "cancelText").toString();
        auto arg = QQmlProperty::read(mViewComponent, "arg");

        // 시스템 팝업이 아니고
        // 1. 취소 버튼이 있다면 취소 동작을...
        // 2. 그렇지 않으면 확인 동작을 한다. (단순 알림창)
        if (menus.contains(cancelText)) {
            onCanceled(arg);
        } else {
            onAccepted(arg);
        }
    }
}

void CommonDialogViewMediator::onSpcxRouteGuidanceStarted()
{
    auto closeViewBySpcx = false;
    auto popupType = QQmlProperty::read(mViewComponent, "popupType").toInt();
    switch (popupType) {
    case SystemPopupMediator::ContinueRg:
    case SystemPopupMediator::Send2Car:
        closeViewBySpcx = true;
        break;
    }

    if (closeViewBySpcx) {
        //QMetaObject::invokeMethod(mViewComponent, "closeThisView"); //+spcx
    }
}

bool CommonDialogViewMediator::isMapUpdateView() const
{
    auto arg = QQmlProperty::read(mViewComponent, CommonConstants::Arg).toMap();
    auto caller = arg[CommonConstants::Caller].toString();
    auto isMapUpdate = mTitle == tr("지도 업데이트") || caller == ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView);

    return isMapUpdate;
}

}
