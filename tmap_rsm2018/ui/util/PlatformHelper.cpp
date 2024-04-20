#include "PlatformHelper.h"

#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>
#include "../platforms/linux-gen3/layermanager.h"
#include "DebugUtils.h"
#include "CommonConstants.h"
#include "../environment.h"
#include "../platforms/linux-gen3/interface/HMIZone13/hmiZonetype.h"

#define TMAP_APP_ID 3

namespace SKT
{

PlatformHelper *PlatformHelper::instance()
{
    static PlatformHelper helper;

    return &helper;
}

void PlatformHelper::initialize(QObject *controller, QQmlEngine* engine)
{
    qDebug() << "#1 PlatformHelper::initialize(QObject *controller, QQmlEngine* engine)";

    Q_ASSERT(controller != Q_NULLPTR);
    Q_ASSERT(engine != Q_NULLPTR);

    qDebug() << "#2 PlatformHelper::initialize(QObject *controller, QQmlEngine* engine)";

    // Popup용 QQuickView 생성 및 레이어에 등록
    QMetaObject::invokeMethod(controller, "setupPlatformPopup");

    QVariant v;
    if (QMetaObject::invokeMethod(controller, "platformController", Q_RETURN_ARG(QVariant, v))) {
        qDebug() << "#3 PlatformHelper::initialize(QObject *controller, QQmlEngine* engine)";
        mPlatformController = v.value<QObject*>();
    }

    qDebug() << "#4 PlatformHelper::initialize(QObject *controller, QQmlEngine* engine)";
    mQmlEngine = engine;
    mTmapController = controller;
}

void PlatformHelper::showView(const QString &viewName, const QString &caller)
{
    qDebug() << "PlatformHelper::showView("+viewName+", caller:" + caller + ")";
#ifdef BUILD_TARGET
    QMetaObject::invokeMethod(mPlatformController, "showPopup", Qt::QueuedConnection, Q_ARG(QString, viewName));
#endif
}

void PlatformHelper::closeView(const QString &viewName)
{
    qDebug() << "PlatformHelper::closeView("+viewName+")";
#ifdef BUILD_TARGET
    QMetaObject::invokeMethod(mPlatformController, "hidePopup", Qt::QueuedConnection, Q_ARG(QString, viewName));
#endif
}

void PlatformHelper::beep(int id)
{
    QMetaObject::invokeMethod(mPlatformController, "beepForwading", Qt::QueuedConnection, Q_ARG(int, id));
}

QObject *PlatformHelper::popupRoot() const
{
    QObject* ret = Q_NULLPTR;

    // Monkey test를 위해 system popup을 이용하지 않는 경우
    // nullptr을 리턴한다.
    if (!mUseAppPopup) {
        auto applicationEngine = dynamic_cast<QQmlApplicationEngine*>(mQmlEngine);

        if (applicationEngine) {
            auto popup = applicationEngine->rootContext()->property("popupRoot");
            ret = popup.value<QObject*>();
        }

        qDebug() << "PlatformHelper::popupRoot() return " << (ret ? "not null" : "null");
    }

    return ret;
}

QObject *PlatformHelper::toastRoot() const
{
    auto applicationEngine = dynamic_cast<QQmlApplicationEngine*>(mQmlEngine);

    if (applicationEngine) {
        auto root = applicationEngine->rootContext()->property("toastRoot");
        return root.value<QObject*>();
    }

    return Q_NULLPTR;
}

QObject *PlatformHelper::popupWindow() const
{
    auto applicationEngine = dynamic_cast<QQmlApplicationEngine*>(mQmlEngine);

    if (applicationEngine) {
        auto root = applicationEngine->rootContext()->property("popupWindow");
        return root.value<QObject*>();
    }

    return Q_NULLPTR;
}

QObject *PlatformHelper::toastWindow() const
{
    auto applicationEngine = dynamic_cast<QQmlApplicationEngine*>(mQmlEngine);

    if (applicationEngine) {
        auto root = applicationEngine->rootContext()->property("toastWindow");
        return root.value<QObject*>();
    }

    return Q_NULLPTR;
}

void PlatformHelper::setZone1(int type, const QVariant &param)
{
    qDebug() << "PlatformHelper::setZone1(" << type << "...";
    QMetaObject::invokeMethod(mPlatformController, "setupZone1", Qt::QueuedConnection, Q_ARG(uint, type), Q_ARG(QVariant, param));

    mZone1Type = type;
    mZone1Param = param;
}

void PlatformHelper::setZone1SignalHandler(QObject *handler)
{
    QMetaObject::invokeMethod(mPlatformController, "setHmiZoneEventHandler", Qt::QueuedConnection, Q_ARG(QObject*,handler));
}

void PlatformHelper::setupKeyboard(const QVariant &param)
{
    auto m = param.toMap();

    qDebug() << "mShowkeyboard : " << mShowKeyboard;
    DebugUtils::printMap("setupKeyboard", m);

    auto show = m["show"].toBool();
    auto pullup = m["pullup"].toBool();
    auto isUpdate = m.contains("text") && m.contains("count");
    if (!isUpdate) {
        // 단순 ShowKeyboard noti.
        if (mShowKeyboard && show) {
            qDebug() << "[@---] skip showkeyboard";
            return;
        }
    }
    uchar type = show ? 0 : 1;
    auto text = m["text"].toString();
    auto count = m["count"].toInt();
    auto numberKeyboard = m["numberKeyboard"].toBool();
    auto dontUseEditBox = m[CommonConstants::DontUseEditBox].toBool();
    if (!show && !mShowKeyboard) {
        // 키보드가 표시중이 아닐 때에는 hide keyboard를 수행하지 않는다.
        // Zone3가 사라짐.
        if (isUpdate) {
            qDebug() << "#001";
            platformKeyboardCall("setZone1ForKeyboard", type, text, count);
        }
    } else {
        if (show != mShowKeyboard) {
            // setting zone3 only.
            if (type == 0/*show*/) {

                // 키보드 표시할 때 zone1에 editbox가 아니라면
                // KEYBOARD_FREEMODE_ZONE2를 사용해야 함.
                if (dontUseEditBox) {
                    if (numberKeyboard) {
                        type = 3; // zone2 숫자 키보드
                    } else {
                        type = 2; // zone2 일반 키보드
                    }
                }
            }
            platformKeyboardCall("setupKeyboard", type, text, count);
            if (show) {
                if (pullup) {
                    // TextSearchView에서 pullup을 누른 경우
                    type = 4;
                    qDebug() << "#002";
                    platformKeyboardCall("setZone1ForKeyboard", type, text, count);
                } else {
                    if (!dontUseEditBox) {
                        type = 4; // NAV_SEARCH_BAR -> SEARCH_TEXT_001
                        qDebug() << "#003";
                        if (!m.contains("text") && !m.contains("count")) {
                            count = -1;
                        }
                        platformKeyboardCall("setZone1ForKeyboard", type, text, count);
                    }
                }
            }

            if (!show && dontUseEditBox) {
                setSearchBarText("");
            }
        } else {
            if (!dontUseEditBox) {
                qDebug() << "#004";
                type = 4;
                platformKeyboardCall("setZone1ForKeyboard", type, text, count);
            }
        }
    }
    mShowKeyboard = show;
    mKeyboardParam = param;
    mZone1Param = param;
}

void PlatformHelper::setSearchBarText(const QString &text)
{
}

void PlatformHelper::setEditFieldText(const QString &text)
{
    QMetaObject::invokeMethod(mPlatformController, "setSearchBarText", Qt::QueuedConnection, Q_ARG(QString, text));
}

void PlatformHelper::setEditFieldUpdate(uchar position, const QString &text)
{
    QMetaObject::invokeMethod(mPlatformController, "setEditFieldUpdate", Q_ARG(uchar, TMAP_APP_ID),
                                                                         Q_ARG(uchar, position),
                                                                         Q_ARG(QString, text));
}

void PlatformHelper::setEditFieldTextForKeyboardKeyPressed(const QString &text)
{
    if (mZone1Type == SEARCH_TEXT_1) {
        setEditFieldText(text);
    }
}

void PlatformHelper::setAppIsAppear(bool b)
{
    auto switchToApp = !mIsAppAppear && b;

    if (switchToApp) {
        // Zone1에 텍스트 입력창이 빈상태로
        // 시스템 화면에서 언어 변경 후 돌아왔을 때
        // placeholder 텍스트 한/영전환 처리.
        // 19.04.09 ymhong.

        QString type;
        switch (mZone1Type) {
        case TITLE: type = "TITLE"; break;
        case NAV_SEARCH_BAR: type = "NAV_SEARCH_BAR"; break;
        case SEARCH_TEXT_1: type = "SEARCH_TEXT_1"; break;
        case SEARCH_TEXT_2: type = "SEARCH_TEXT_2"; break;
        }
        qDebug() << "PlatformHelper::setAppIsAppear(" << type << ")";
        DebugUtils::printMap("setAppIsAppear", mZone1Param.toMap());
    }
    mIsAppAppear = b;
}

void PlatformHelper::showVolume(bool show)
{
    if (show) {
        QMetaObject::invokeMethod(mPlatformController, "showVolume", Qt::QueuedConnection);
    }
}

void PlatformHelper::showPopupWindow(bool show)
{
    auto popupWindow = dynamic_cast<QWindow*>(PlatformHelper::instance()->popupWindow());
    if (popupWindow) {
        qDebug() << "PlatformHelper::showPopupWindow(" << show << ")";
        if (show) {
            popupWindow->show();
        } else {
            popupWindow->hide();
        }
    }
}

void PlatformHelper::switchToApp()
{
    qDebug() << "PlatformHelper::switchToApp()";
#ifdef BUILD_TARGET
    QMetaObject::invokeMethod(mPlatformController, "switchToApp", Qt::QueuedConnection);
#endif
}

void PlatformHelper::switchToSettingView()
{
    qDebug() << "PlatformHelper::switchToSettingView()";
#ifdef BUILD_TARGET
    QMetaObject::invokeMethod(mPlatformController, "switchToSettingView", Qt::QueuedConnection);
#endif
}

void PlatformHelper::switchToSystem()
{
    qDebug() << "PlatformHelper::switchToSystem()";

#ifdef BUILD_TARGET
    QMetaObject::invokeMethod(mPlatformController, "switchToSystem", Qt::QueuedConnection);
#endif
}

void PlatformHelper::setParentForSystemPopup(QVariantMap &m)
{
#ifdef BUILD_TARGET
    auto popupRoot = PlatformHelper::instance()->popupRoot();
    if (popupRoot) {
        m[CommonConstants::Parent] = QVariant::fromValue(popupRoot);
    } else {
        m[CommonConstants::ParentObjectName] = "popupWrap";
    }
#else
    m[CommonConstants::ParentObjectName] = "popupWrap";
#endif
}

void PlatformHelper::setUseAppPopup(bool b)
{
    mUseAppPopup = b;
}

void PlatformHelper::keyForwarding(uint key, int state)
{
    qDebug() << "PlatformHelper::keyForwarding(" << key << ")";
    QMetaObject::invokeMethod(mPlatformController, "keyForwarding", Qt::QueuedConnection, Q_ARG(uint, key), Q_ARG(uint, state));
}

void PlatformHelper::gadgetTouched(uint touchState, uint x, uint y, uint timeStamp, int instanceId)
{
    QMetaObject::invokeMethod(mTmapController,
                              "simulationGadgetTouch",
                              Q_ARG(uint, touchState),
                              Q_ARG(uint, x),
                              Q_ARG(uint, y),
                              Q_ARG(uint, timeStamp),
                              Q_ARG(int, instanceId));
}

void PlatformHelper::requestFreeTextSearch(const QString &text)
{
    mIsRgContinueEnable = false;
    QMetaObject::invokeMethod(mTmapController, "requestFreeTextSearch", Q_ARG(QString, text));
}

void PlatformHelper::requestAroundSearchParking()
{
    mIsRgContinueEnable = false;
    QMetaObject::invokeMethod(mTmapController, "requestAroundSearchParking");
}

void PlatformHelper::requestAroundSearchPatrol(int type)
{
    mIsRgContinueEnable = false;
    QMetaObject::invokeMethod(mTmapController, "requestAroundSearchPatrol", Q_ARG(int, type));
}

void PlatformHelper::disableRgContinue()
{
    mIsRgContinueEnable = false;
}

bool PlatformHelper::isRgContinueEnable() const
{
    return mIsRgContinueEnable;
}

void PlatformHelper::testMountMedia()
{
#ifdef QT_DEBUG
    QString path = Environment::getMapDataPath() + "../../usb";
    QMetaObject::invokeMethod(mPlatformController, "platformMediaMounted", Q_ARG(QString, path));
#endif
}

bool PlatformHelper::isKeyboardPulldown() const
{
    return mZone1Type == SEARCH_TEXT_2;
}

bool PlatformHelper::isKeyboardVisible() const
{
    return mShowKeyboard;
}

void PlatformHelper::appHasNoMap()
{
    QMetaObject::invokeMethod(mPlatformController, "appHasNoMap");
}

void PlatformHelper::encoderForwarding(int encCode, int encValue)
{
    QMetaObject::invokeMethod(mPlatformController, "encorderForwarding", Q_ARG(int, encCode), Q_ARG(int, encValue));
}

void PlatformHelper::platformKeyboardCall(const char *methodName, uchar type, const QString &text, int count)
{
    auto method = QString::fromLatin1(methodName);
    qDebug() << "platformKeyboardCall(" << method << ", " << (type==1 ?"hide": "show") << ", " << text << ", " << count << ")";
#ifdef BUILD_TARGET
    QMetaObject::invokeMethod(mPlatformController, methodName, Qt::QueuedConnection, Q_ARG(uchar, type), Q_ARG(QString, text), Q_ARG(int, count));

    const int SEARCH_BAR_FOR_INTEGRATED_SEARCH = 4;

    if (text.isEmpty() && type != SEARCH_BAR_FOR_INTEGRATED_SEARCH) {
        mZone1Type = NAV_SEARCH_BAR;
    } else {
        mZone1Type = SEARCH_TEXT_1;
    }
#endif
}

}
