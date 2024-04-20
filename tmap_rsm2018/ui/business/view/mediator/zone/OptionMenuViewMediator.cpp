#include "OptionMenuViewMediator.h"

#include "ApplicationConstants.h"
#include "ZoneConstants.h"
#include "HiddenSettingConstants.h"
#include "NavigationControllerConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include "PlatformHelper.h"
#include "ViewRegistry.h"

#define ZONE4_HARDKEY_HEIGHT  92

namespace SKT
{

OptionMenuViewMediator::OptionMenuViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{

}

QList<int> OptionMenuViewMediator::listNotificationInterests()
{
    return QList<int> {
        SettingConstants::ResponseHiddenSetting,
        ZoneConstants::CswBackTriggered,
        NavigationControllerConstants::NavigationDisappeared
    };
}

bool OptionMenuViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
#ifdef BUILD_TARGET
        if (PlatformHelper::instance()->popupRoot() == nullptr) {
            // Monkey test를 위해 popupRoot가 null인 경우는 아래 property를 설정.
            // 옵션 메뉴 위치가 이상해 짐.
            QQmlProperty::write(mViewComponent, "hardKeyMargin", ZONE4_HARDKEY_HEIGHT);
        }
#endif
            QVariantMap m;
            m[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification(SettingConstants::RequestHiddenSetting, m);
        }
        break;

    case RegistCSW:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case Activated: {
        sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal", true}});
        sendNotification(ApplicationConstants::EnableBlur, true);
        PlatformHelper::instance()->showView(APP_POPUP_VIEW_NAME, "OptionMenuViewMediator");
        break;
    }

    case Deactivated:
        break;

    case SelectMenu:
    case SelectMenuIndex:
        sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal", false}});
        sendNotification(ApplicationConstants::EnableBlur, false);
        if (event == SelectMenu) {
            sendNotification(ZoneConstants::DoSoftkeyOption, data);
        } else {
            sendNotification(ZoneConstants::DoSoftkeyIndexMenuOption, data);
        }
        PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
        break;

    case ReleaseMenu:
    case ReleaseMenuIndex:
        // special case.
        // option메뉴에서 떼자마자 처리가 필요한 경우.(예:경로상세화면) -> performance issue
        // 19.06.21 ymhong.
        if (event == ReleaseMenu) {
            sendNotification(ZoneConstants::DoSoftkeyOptionReleased, data);
        } else {
            sendNotification(ZoneConstants::DoSoftkeyIndexMenuOptionReleased, data);
        }
        break;

    case CaptureScreen:
        sendNotification(HiddenSettingConstants::CaptureScreenshot);
        break;
    }
    return true;
}

bool OptionMenuViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case NavigationControllerConstants::NavigationDisappeared:
        // 후방 카메라 화면 표출 후 복귀 하면 화면이 Blur먹은 상태로 남음.
        // 19.12.20 ymhong
        sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal", false}});
        sendNotification(ApplicationConstants::EnableBlur, false);
        sendNotification(Constants::CloseView, QVariantMap{
                             {CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::OptionMenuView)}
                         });
        // TMCRSM-4309. 옵션 메뉴가 뜬 상태에서 홈 메뉴로 나갔다 재 진입하면 ZONE1이 blur됨.
        PlatformHelper::instance()->closeView(APP_POPUP_VIEW_NAME);
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
                        //QQmlProperty::write(mViewComponent, "captureEnable", enabled);
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
    return true;
}

}
