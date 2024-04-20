#ifndef PLATFORMHELPER_H
#define PLATFORMHELPER_H

#include "../platformcontroller.h"

#define APP_POPUP_VIEW_NAME         "NaviPopup"
#define MAPUPDATE_POPUP_VIEW_NAME   "MapUpdatePopup"

class QQmlEngine;
class QQuickView;

namespace SKT
{

class PlatformHelper
{
public:
    static PlatformHelper *instance();

    void initialize(QObject* platformController, QQmlEngine* engine);
    void showView(const QString& viewName, const QString &caller="");
    void closeView(const QString& viewName);
    void beep(int id);
    QObject* popupRoot() const;
    QObject* toastRoot() const;
    QObject *popupWindow() const;
    QObject* toastWindow() const;

    void setZone1(int type, const QVariant& param);
    void setZone1SignalHandler(QObject* handler);
    void setupKeyboard(const QVariant& param);
    void setSearchBarText(const QString& text); // deprecated
    void setEditFieldText(const QString &text);
    void setEditFieldUpdate(uchar position, const QString &text);
    void setEditFieldTextForKeyboardKeyPressed(const QString &text);
    void setAppIsAppear(bool);
    void showVolume(bool);
    void showPopupWindow(bool);
    void switchToApp();
    void switchToSettingView();
    void switchToSystem();
    void setParentForSystemPopup(QVariantMap &m);

    void setUseAppPopup(bool b);
    void keyForwarding(uint key, int state);

    // Gadget simulation.
    void gadgetTouched(uint touchState, uint x, uint y, uint timeStamp, int instanceId);
    void requestFreeTextSearch(const QString &text);
    void requestAroundSearchParking();
    void requestAroundSearchPatrol(int type);
    void disableRgContinue();

    bool isRgContinueEnable() const;
    void testMountMedia();
    bool isKeyboardPulldown() const;
    bool isKeyboardVisible() const;

    void appHasNoMap();
    void encoderForwarding(int encCode, int encValue);

private:
    void platformKeyboardCall(const char *methodName, uchar type, const QString& text, int count);

private:
    QObject *mPlatformController = nullptr;
    QObject *mTmapController = nullptr;
    QQmlEngine* mQmlEngine = nullptr;
    QQuickView* mPopupView = nullptr;
    QQuickView* mToastView = nullptr;
    bool mShowKeyboard = false;
    bool mIsAppAppear = false;
    int mZone1Type = -1;
    QVariant mZone1Param;
    QVariant mKeyboardParam;
    bool mUseAppPopup = false;
    bool mIsRgContinueEnable = true;
};

}

#endif // PLATFORMHELPER_H
