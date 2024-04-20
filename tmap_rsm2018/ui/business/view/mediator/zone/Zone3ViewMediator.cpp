#include "Zone3ViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "ZoneConstants.h"
#include "NavigationControllerConstants.h"
#include "satellitedataptr.h"
#include "SettingConstants.h"
#include "Csw.h"
#include "CswManager.h"
#include "KeyboardConstants.h"
#include "../rg/RgUtils.h"
#include "ApplicationConstants.h"
#include "HiddenSettingConstants.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"
#include "ViewRegistry.h"
#include "LangConstants.h"
#include "QmlAppHelper.h"
#include "SoftkeyMenu.h"
#ifdef BUILD_TARGET
#include "PlatformHelper.h"
#endif

//#define DEBUG_ZONE3_MENU

namespace SKT {

Zone3ViewMediator::Zone3ViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
    , mBlockSoftkey(false)
    , mShowKeyboard(false)
{
    mRgDataCacheTimer.setInterval(2000);
    connect(&mRgDataCacheTimer, &QTimer::timeout, this, &Zone3ViewMediator::updateSoftkeyRouteGuideUsingCache);
}

QList<int> Zone3ViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
        ZoneConstants::RegistSoftkey,
        ZoneConstants::EditSoftkeyMenu,
        ZoneConstants::EditSoftkeyIndexMenu,
        ZoneConstants::SelectSoftkeyMenuMode,
        ZoneConstants::SelectSoftkeyIndexMenuMode,
        ZoneConstants::SelectSoftkeyMenuWithTabMode,
        ZoneConstants::SelectSoftkeyIndexMenuWithTabMode,
        ZoneConstants::SelectSoftkeyNaviMode,
        ZoneConstants::SelectSoftkeyNormalDriveMode,
        ZoneConstants::SelectSoftkeyToggleMode,
        ZoneConstants::SelectSoftkeyToggleIndexMode,
        ZoneConstants::SelectSoftkeySimulMode,
        RgConstants::StopRouteGuidance,
        ZoneConstants::UpdateModal,
        ZoneConstants::UpdateSoftkeyAddress,
        ZoneConstants::UpdateSoftkeyRouteGuide,
        ZoneConstants::UpdatePoi,
        ZoneConstants::CswBackTriggered,
        ZoneConstants::CswOptionTriggered,
        KeyboardConstants::ShowKeyboard,
        KeyboardConstants::HideKeyboard,
        NavigationControllerConstants::ChangeTimeFormat,
        RgConstants::UpdateGpsTime,
        ZoneConstants::BlockNaviMenu,
        ZoneConstants::BlockSoftkeyMenu,
        ZoneConstants::ShowZone3,
        ZoneConstants::HideZone3,
        ZoneConstants::DoSoftkeyOption,
        ZoneConstants::DoSoftkeyIndexMenuOption,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ChangeToApp,
        ApplicationConstants::ChangeToWidget,
    };

    return interests;
}

bool Zone3ViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
        sendNotification(ZoneConstants::RegistZone3, QVariant::fromValue(mViewComponent));
        sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));
#ifdef BUILD_TARGET
        QQmlProperty::write(mViewComponent, "isTarget", true);
#endif
        auto lang = QmlAppHelper::instance()->language();
        QQmlProperty::write(mViewComponent, "lang", lang);
        break;
    }

    case BackClicked:
        if (!mBlockSoftkey)
            sendNotification(ZoneConstants::DoSoftKeyFunction,
                             QVariantMap{{"function","back"},{"name",currentViewName(data)}});
        break;

    case NaviClicked:
        if (!mBlockSoftkey)
            sendNotification(ZoneConstants::DoSoftKeyFunction,
                             QVariantMap{{"function","navi"},{"name",currentViewName(data)}});
        break;

    case OptionClicked:
        if (!mBlockSoftkey)
            sendNotification(ZoneConstants::DoSoftKeyFunction,
                             QVariantMap{{"function","option"},{"name",currentViewName(data)}});
        break;

    case OpenOptionMenuView:
    case OpenOptionMenuView2:
        if (!mBlockSoftkey) {
            auto m = data.toMap();
            auto menu = m["menu"];
            auto indexMode = event == OpenOptionMenuView2;
            auto currentViewName = QQmlProperty::read(mViewComponent, "currentViewName").toString();

            m = QVariantMap{
                {CommonConstants::Url, ViewRegistry::url(ViewRegistry::OptionMenuView)},
                {CommonConstants::Menus, menu},
                {"indexMode", indexMode},
                {"currentViewName", currentViewName}
            };
            QObject *popupRoot = nullptr;
#if defined(BUILD_TARGET)
            popupRoot = PlatformHelper::instance()->popupRoot();
#endif
            if (popupRoot) {
                m[CommonConstants::Parent] = QVariant::fromValue(popupRoot);
            } else {
                m[CommonConstants::ParentObjectName] = "popupWrap";
            }
            sendNotification(Constants::CreateView, m);
        }
        break;

    case MenuClicked:
        if (!mBlockSoftkey)
            sendNotification(ZoneConstants::DoSoftKeyFunction,
                             QVariantMap{{"function","menu"},{"name",currentViewName(data)},{"menu",menuName(data)}});
        break;

    case Menu2Clicked:
        if (!mBlockSoftkey) {
            auto m = data.toMap();
            m["function"] = "menu";
            sendNotification(ZoneConstants::DoSoftkeyIndexMenuFunction, m);
        }
        break;

    case OptionMenuClicked:
        sendNotification(ZoneConstants::DoSoftKeyFunction,
                         QVariantMap{{"function","menu"},{"name",currentViewName(data)},{"menu",menuName(data)}});
        break;
    case OptionMenu2Clicked:
        {
            auto m = data.toMap();
            m["function"] = "menu";
            sendNotification(ZoneConstants::DoSoftkeyIndexMenuFunction, m);
        }
        break;

    case MenuCleared:
    case RemoveSoftkeySection:
        {
            QVariantMap m;
            m[CommonConstants::Section] = Csw::Section_Softkey;
            sendNotification(NavigationControllerConstants::RemoveCSW, m);
        }
        break;

    case MenuCreated:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case FaderClicked:
        sendNotification(ZoneConstants::FaderClicked);
        break;

    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;

    default:
        return false;
    }
    return true;
}

bool Zone3ViewMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case ZoneConstants::RegistSoftkey:
        QMetaObject::invokeMethod(mViewComponent, "registSoftkey", Q_ARG(QVariant, noti->getBody()));
        break;

    case NavigationControllerConstants::ChangeLanguage:
        {
            auto m = noti->getBody().toMap();
            auto lang = m[LangConstants::Lang].toString();

            QQmlProperty::write(mViewComponent, "lang", lang);
        }
        break;

    case ZoneConstants::EditSoftkeyMenu:
        {
            auto m = noti->getBody().toMap();
            auto index = m["index"].toInt();
            auto menu = m["menu"].toString();
            QMetaObject::invokeMethod(mViewComponent, "editSoftkeyMenu", Q_ARG(QVariant, index), Q_ARG(QVariant, menu));
            sendNotification(NavigationControllerConstants::SelectDefaultSoftkeyCSW);
        }
        break;

    case ZoneConstants::EditSoftkeyIndexMenu:
        {
            auto m = noti->getBody().toMap();
            auto index = m["index"].toInt();
            auto menu = m["menu"];
            QMetaObject::invokeMethod(mViewComponent, "editSoftkeyMenu2", Q_ARG(QVariant, index), Q_ARG(QVariant, menu));
            sendNotification(NavigationControllerConstants::SelectDefaultSoftkeyCSW);
        }
        break;

    case ZoneConstants::SelectSoftkeyMenuMode:
        {
            sendNotification(NavigationControllerConstants::RemoveCSW, QVariantMap{{CommonConstants::Section, Csw::Section_Softkey}});
            QStringList menus;
            QStringList options;
            auto m = noti->getBody().toMap();
            if (m.isEmpty()) {
                menus = noti->getBody().toStringList();
                qDebug() << "menus : " << menus;
            } else {
                menus = m["menus"].toStringList();
                options = m["optionMenu"].toStringList();
            }

#ifdef DEBUG_ZONE3_MENU
            QString source = noti->getSource()->metaObject()->className();
            qDebug() << "[ZONE3] SelectSoftkeyMenuMode. source: " << source << ", menus: " << menus << "options: " << options;
#endif
            QMetaObject::invokeMethod(mViewComponent, "setSoftkeyMenus", Q_ARG(QVariant, menus), Q_ARG(QVariant, options));//menus);
            sendNotification(NavigationControllerConstants::SelectDefaultSoftkeyCSW);
        }
        break;

    case ZoneConstants::SelectSoftkeyIndexMenuMode:
        {
            sendNotification(NavigationControllerConstants::RemoveCSW, QVariantMap{{CommonConstants::Section, Csw::Section_Softkey}});
            auto m = noti->getBody().toMap();

            QVariantList menus2;
            QVariantList options2;
            QString defaultMenu;

#ifdef DEBUG_ZONE3_MENU
            QString source = noti->getSource()->metaObject()->className();
            qDebug() << "[ZONE3] SelectSoftkeyIndexMenuMode. source: " << source << ", menus: " << menus2 << "options: " << options2;
#endif
            if (m.contains("menus2")) {
                menus2 = m["menus2"].toList();
                options2 = m["optionMenu2"].toList();
                defaultMenu = m["defaultMenu"].toString();
            } else {
                int id;
                QString menu;
                QStringList menus;
                QStringList options;
                if (m.isEmpty()) {
                    menus = noti->getBody().toStringList();
                } else {
                    menus = m["menus"].toStringList();
                    options = m["optionMenu"].toStringList();
                    defaultMenu = m["defaultMenu"].toString();
                }

                for (auto i = 0; i<menus.length(); i++) {
                    id = i;
                    menu = menus[i];
                    if (menu != "noOption") {
                        menus2.append(QVariant::fromValue(SoftkeyMenu{id, menu}));
                    } else {
                        qDebug() << "skip noOption";
                    }
                }

                const int kOptionMenuBaseIndex = 100;

                for (auto i = 0; i<options.length(); i++) {
                    id = kOptionMenuBaseIndex + i;
                    menu = options[i];
                    options2.append(QVariant::fromValue(SoftkeyMenu{id, menu}));
                }
            }
            QMetaObject::invokeMethod(mViewComponent, "setSoftkeyMenus2", Q_ARG(QVariant, menus2), Q_ARG(QVariant, options2), Q_ARG(QVariant, defaultMenu));
            sendNotification(NavigationControllerConstants::SelectDefaultSoftkeyCSW);
        }
        break;
    case ZoneConstants::SelectSoftkeyMenuWithTabMode:
    case ZoneConstants::SelectSoftkeyIndexMenuWithTabMode:
        {
            auto m = noti->getBody().toMap();
            auto select = m["select"];
            auto menus = m["menu"];
            auto arg0 = Q_ARG(QVariant, menus);
            auto arg1 = Q_ARG(QVariant, select);

            if (noti->getId() == ZoneConstants::SelectSoftkeyMenuWithTabMode) {
                QMetaObject::invokeMethod(mViewComponent, "setSoftkeyMenusWithTab", arg0, arg1);
            } else {
                QMetaObject::invokeMethod(mViewComponent, "setSoftkeyMenusWithTab2", arg0, arg1);
            }
            //sendNotification(NavigationControllerConstants::SelectDefaultSoftkeyCSW);
        }
        break;

    case ZoneConstants::SelectSoftkeyNaviMode:
        QMetaObject::invokeMethod(mViewComponent, "setSoftkeyNavi");
#ifdef DEBUG_ZONE3_MENU
        {
            QString source = noti->getSource()->metaObject()->className();
            qDebug() << "[ZONE3] SelectSoftkeyNaviMode.";
        }
#endif
        break;

    case ZoneConstants::BlockNaviMenu:
        {
            auto block = noti->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "blockNaviButton", Q_ARG(QVariant, block));
        }
        break;

    case ZoneConstants::BlockSoftkeyMenu:
        {
            auto block = noti->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "blockMenuButton", Q_ARG(QVariant, block));
        }
        break;

    case ZoneConstants::SelectSoftkeyToggleMode:
    case ZoneConstants::SelectSoftkeyToggleIndexMode:
        {
            auto m = noti->getBody().toMap();
            auto menu = m[QStringLiteral("menu")].toStringList();
            auto select = m[QStringLiteral("select")].toInt();
            if (noti->getId() == ZoneConstants::SelectSoftkeyToggleMode) {
                QMetaObject::invokeMethod(mViewComponent, "setSoftkeyToggle", Q_ARG(QVariant, menu), Q_ARG(QVariant, select));
            } else {
                QMetaObject::invokeMethod(mViewComponent, "setSoftkeyToggle2", Q_ARG(QVariant, menu), Q_ARG(QVariant, select));
            }
            sendNotification(NavigationControllerConstants::SelectDefaultSoftkeyCSW);
        }
        break;

    case ZoneConstants::SelectSoftkeySimulMode:
        QMetaObject::invokeMethod(mViewComponent, "setSoftkeySimulation");
        break;

    case RgConstants::StopRouteGuidance:
        QMetaObject::invokeMethod(mViewComponent, "stopNavi");
        break;

    case ZoneConstants::SelectSoftkeyNormalDriveMode:
        QMetaObject::invokeMethod(mViewComponent, "setSoftkeyNormalDrive");
        sendNotification(NavigationControllerConstants::RemoveCSW, QVariantMap{{CommonConstants::Section, Csw::Section_Softkey}});
        break;

    case ZoneConstants::UpdateSoftkeyAddress:
        QMetaObject::invokeMethod(mViewComponent, "setSoftkeyAddress", Q_ARG(QVariant, noti->getBody()));
        break;

    case ZoneConstants::UpdateSoftkeyRouteGuide:
        {
            auto m = noti->getBody().toMap();
            m["poi"] = mPoi;
            auto remainDistance = m["remainDistance"].toInt();
            m["remainDistanceText"] = RgUtils::formatDistance(remainDistance);

            updateSoftkeyRouteGuide(m);

            mRgDataCache = m;
            mRgDataCacheTimer.start();
        }
        break;

    case ZoneConstants::UpdatePoi:
        mPoi = noti->getBody().toString();
        break;

    case ZoneConstants::UpdateModal:
        {
            auto m = noti->getBody().toMap();
            mBlockSoftkey = m["modal"].toBool();

            blockSoftkey(mBlockSoftkey || mShowKeyboard);
        }
        break;

    case KeyboardConstants::ShowKeyboard:
        mShowKeyboard = true;
        blockSoftkey(mBlockSoftkey || mShowKeyboard);
        break;

    case KeyboardConstants::HideKeyboard:
        mShowKeyboard = false;
        blockSoftkey(mBlockSoftkey || mShowKeyboard);
        break;

    case ZoneConstants::CswBackTriggered:
        if (!mBlockSoftkey) {
            auto currentViewName = QQmlProperty::read(mViewComponent, "currentViewName").toString();
            if (currentViewName == ViewRegistry::objectName(ViewRegistry::MapView)) {

            } else {
                auto state = noti->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    sendNotification(ZoneConstants::DoSoftKeyFunction,
                                     QVariantMap{{"function", "back"},{"name", currentViewName}});
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;

    case ZoneConstants::CswOptionTriggered:
        if (!mBlockSoftkey) {
            QMetaObject::invokeMethod(mViewComponent, "execOption");
        }
        break;

    case NavigationControllerConstants::ChangeTimeFormat:
        {
            auto m = noti->getBody().toMap();
            auto format = m["format"].toString();
            QQmlProperty::write(mViewComponent, "timeFormat", format);
        }
        break;

    case ZoneConstants::ShowZone3:
        showZone3(noti->getBody());
        break;

    case ZoneConstants::HideZone3:
        hideZone3(noti->getBody());
        break;

    case RgConstants::UpdateGpsTime:
        parseGpsTime(noti->getBody());
        break;

    case ZoneConstants::DoSoftkeyOption:
        QMetaObject::invokeMethod(mViewComponent, "sendSoftkeyFunction", Q_ARG(QVariant, noti->getBody()));
        break;

    case ZoneConstants::DoSoftkeyIndexMenuOption:
        QMetaObject::invokeMethod(mViewComponent, "sendSoftkeyFunctionIndex", Q_ARG(QVariant, noti->getBody()));
        break;
    case ApplicationConstants::ChangeToApp:
        QQmlProperty::write(mViewComponent, "visible", true);
        break;
    case ApplicationConstants::ChangeToWidget:
        qDebug() << "ChangeToWidget";
        QQmlProperty::write(mViewComponent, "visible", false);
        break;
    }

#ifdef BUILD_TARGET
#if (0) // 타겟에서 zone3 메뉴 설정 추적용 디버그 코드. 19.06.11 ymhong
    QString softkeyMenu;
    switch (noti->getId()) {
    case ZoneConstants::EditSoftkeyMenu: softkeyMenu = "EditSoftkeyMenu"; break;
    case ZoneConstants::EditSoftkeyIndexMenu: softkeyMenu = "EditSoftkeyIndexMenu"; break;
    case ZoneConstants::SelectSoftkeyMenuMode: softkeyMenu = "SelectSoftkeyMenuMode"; break;
    case ZoneConstants::SelectSoftkeyIndexMenuMode: softkeyMenu = "SelectSoftkeyIndexMenuMode"; break;
    case ZoneConstants::SelectSoftkeyMenuWithTabMode: softkeyMenu = "SelectSoftkeyMenuWithTabMode"; break;
    case ZoneConstants::SelectSoftkeyIndexMenuWithTabMode: softkeyMenu = "SelectSoftkeyIndexMenuWithTabMode"; break;
    case ZoneConstants::SelectSoftkeyNaviMode: softkeyMenu = "SelectSoftkeyNaviMode"; break;
    case ZoneConstants::SelectSoftkeyToggleMode: softkeyMenu = "SelectSoftkeyToggleMode"; break;
    case ZoneConstants::SelectSoftkeyToggleIndexMode: softkeyMenu = "SelectSoftkeyToggleIndexMode"; break;
    case ZoneConstants::SelectSoftkeySimulMode: softkeyMenu = "SelectSoftkeySimulMode"; break;
    case ZoneConstants::SelectSoftkeyNormalDriveMode: softkeyMenu = "SelectSoftkeyNormalDriveMode"; break;
    }
    if (softkeyMenu.length()) {
        QString caller = noti->getSource()->metaObject()->className();
        qDebug() << "[ZONE3]" << softkeyMenu << ", caller: " << caller;
    }
#endif
#endif

    return false;
}

void Zone3ViewMediator::updateSoftkeyRouteGuide(const QVariantMap &m)
{
    QMetaObject::invokeMethod(mViewComponent, "setSoftkeyRouteGuide", Q_ARG(QVariant, m));
}

void Zone3ViewMediator::updateSoftkeyRouteGuideUsingCache()
{
    updateSoftkeyRouteGuide(mRgDataCache);
}

QString Zone3ViewMediator::currentViewName(const QVariant &data)
{
    QVariantMap m = data.toMap();
    return m["name"].toString();
}

QString Zone3ViewMediator::menuName(const QVariant &data)
{
    QVariantMap m = data.toMap();
    return m["menu"].toString();
}

void Zone3ViewMediator::blockSoftkey(bool block)
{
    QMetaObject::invokeMethod(mViewComponent, "blockSoftkey", Q_ARG(QVariant, block));
}

void Zone3ViewMediator::parseGpsTime(const QVariant &v)
{
    auto gpsTime = v.value<QDateTime>();
    QQmlProperty::write(mViewComponent, "gpsTime", gpsTime);
    auto time = gpsTime.time();
    auto h = time.hour();
    auto m = time.minute();
    auto s = time.second();
    QQmlProperty::write(mViewComponent, "gpsHour", h);
    QQmlProperty::write(mViewComponent, "gpsMin", m);
    QQmlProperty::write(mViewComponent, "gpsSec", s);
}

void Zone3ViewMediator::showZone3(const QVariant &v)
{
    auto showType = v.toInt();
    auto isAnimation = showType == ZoneConstants::Animation;

    if (isAnimation) {
        QQmlProperty::write(mViewComponent, "state", "show");
    } else {
        QQmlProperty::write(mViewComponent, "visible", true);
    }
}

void Zone3ViewMediator::hideZone3(const QVariant &v)
{
    auto showType = v.toInt();
    auto isAnimation = showType == ZoneConstants::Animation;

    if (isAnimation) {
        QQmlProperty::write(mViewComponent, "state", "hide");
    } else {
        QQmlProperty::write(mViewComponent, "visible", false);
    }
}

}//SKT
