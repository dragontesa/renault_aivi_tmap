#ifndef APPLICATIONMEDIATOR_H
#define APPLICATIONMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "ViewTree.h"
#include "mediator/app/MapViewMode.h"
#include "ApplicationConstants.h"
#include <QQuickWindow>
#include "QmlOffscreenRenderer.h"
#include <QOpenGLFramebufferObject>
#include <QOpenGLExtraFunctions>
#include "IconGadgetProvider.h"
#include "uiconstants.h"

namespace SKT {

class GadgetOffscreenRenderer;

class ApplicationMediator : public AbstractViewMediator
{
    Q_OBJECT
public:
    enum EventK {
		ComponentCompleted = 0,
        CaptureScreenshot,
        RegisterMapInClusterWindow,
        TestGadgetPressed
    };
    Q_ENUM(EventK)

    Q_INVOKABLE ApplicationMediator( QObject* viewComponent, QQmlEngine* qmlEngine, bool hasMapData, QVariantMap gadgetSignal );
    ~ApplicationMediator();

    QList<int> listNotificationInterests();

public slots:
    Q_INVOKABLE void __sendEvent( int event, const QVariant& data );

protected:
    bool eventFilter(QObject *target, QEvent *event );
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private slots:  
    void onViewTreeVisibilityChanged();    
    void onViewVisibleChanged( const QVariant& view );
    void onViewDestruction( const QVariant& view, const QVariant& data );
    void onUserProfileChangeFinished();
    void onGadgetWindowClosing();
    void onGadgetOffscreenRendererStopped();
    void onGadgetOffscreenRendererWindowHasRendered();

private:
    void initGadgetHanlder();

    void handleStateChange( ApplicationConstants::StateK newState );
    void handleUserProfileChange( const mvc::INotificationPtr& note );
    void handleToast( const mvc::INotificationPtr& note );
    void handleWidget( const mvc::INotificationPtr& note );
    void handleMapViewMode( const mvc::INotificationPtr& note );
    void handleHiddenSetting( const mvc::INotificationPtr& note );
    void handleMapInCluster( const mvc::INotificationPtr& note );
    void stopGadget();
    void changeToApp();

    void createView( const QVariantMap& args );
    void showView( const QVariantMap& args );
    void hideView( const QVariantMap& args );    
    void closeView( const QVariantMap& args );

    void resetNavi(const QVariantMap &m);
    void closeUserProfileDependentViews();

    QQmlEngine* mQmlEngine = nullptr;
    ViewTree mViewTree;
    QObject* mLastActiveView = nullptr;
    QObject* mBlankWrap = nullptr;
    QObject* mZoneWrap = nullptr;
    QObject* mContentWrap = nullptr;
    QObject* mContentRoot = nullptr;
    QObject* mContentRootWithoutSoftkey = nullptr;
    QObject* mProgressWrap = nullptr;
    QObject* mAlertWrap = nullptr;
    QObject* mWidgetWrap = nullptr;
    QObject* mMapViewWrap = nullptr;
    enum CreateViewStatusK {
        Creating,
        Created
    };
    QMap<QString,CreateViewStatusK> mCreateViewStatus;
	QList<int> mViewIdList;
    QMap<int,QVariantMap> mViewDataMap;	
    QObject* mCommonKeyboardInput = nullptr;
    QObject* mKeyboardInput = nullptr;
    MapViewMode mMapViewMode;
    QObject* mToastWrap = nullptr;
    int mShowRecentDestinations = 0;
    bool mIgnoreRecentTileGridView = false;
    bool mIsRgContInProgress = false;
    bool mIsModal = false;
    bool mHasMapData = false;
    bool mForceNaviMode = false;
    bool mUseScreenShot = false;
    bool mEnableMonkeyTest = false;
    bool mIsRg = false;
    int  mAutoLevelMode = 0;    
    ApplicationConstants::StateK mState;
    QObject* mToast = nullptr;
    QmlOffscreenRenderer* mMapInClusterRenderer = nullptr;
    QQuickWindow* mClusterWindow = nullptr;
    QVariantMap mLastGadgetSignal;
    GadgetOffscreenRenderer* mGadgetRenderer = nullptr;
    QQuickWindow* mGadgetDebugWindow = nullptr;
    bool mIsGadgetVisible = false;
    IconGadgetProviderPtr mIconGadgetProvider;
    QList<UIConstants::GadgetType> mActivatedGadgets;
    QTimer mTestTimer;
};

}

#endif // APPLICATIONMEDIATOR_H
