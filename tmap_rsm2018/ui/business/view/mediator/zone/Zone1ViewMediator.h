#ifndef ZONE1VIEWMEDIATOR_H
#define ZONE1VIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class Zone1ViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        TextInputFocusActivated,
        Mediated,
        RegistCommonKeyboardInput,
        RegistKeyboardInput,
        RegistCsw,
        ToggleGuidView,
        RequestNomalize,
        PullDownKeyboard,
        PullUpKeyboard
    };

    Q_INVOKABLE Zone1ViewMediator( QObject* viewComponent = nullptr );

    QList<int> listNotificationInterests();

public Q_SLOTS:
    void onActiveNotificationType(uint activeNotification);
    void onButtonPressed(uchar appId, uint buttonId, uchar buttonState);
    void onKeyboardInputFinished(uchar, const QString &searchbarText);
    void onKeyboardKeyPressed(uchar, const QString &searchbarText, const QString &keyPressed, bool isCtrl, bool IsSpeedLockActive);
    void onPullupButtonPressed(uchar, uchar buttonId, uchar buttonState);
    void onTogglePressed(uchar appId, uint buttonId, uchar buttonState, bool toggleValue);
    void onPopupStatus(uint surfaceId, uint popupStatus);

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    void updateMapViewPadding(const QVariant&);
    void updateViaInfo(const QVariantMap&);
    void onTextInputFocusActivated(const QVariant&);
    void updateZone1(const QVariant&);
    void pulldownKeyboard();
    void pullupKeyboard();

private:
    QString mKeyboardOwner;
    QString mSearchBarText;
    bool mGuidViewVisible = false;
    bool mIsAppForground = false;
    int zone1Type;
    QList<int> mViewList;
    QMap<int,QVariantMap> mViewListData;
};

}//SKT

#endif // ZONE1VIEWMEDIATOR_H
