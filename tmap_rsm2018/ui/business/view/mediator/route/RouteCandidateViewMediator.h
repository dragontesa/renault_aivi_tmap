#ifndef ROUTECANDIDATEVIEWMEDIATOR_H
#define ROUTECANDIDATEVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RouteCandidateListModel.h"
#include "RpStatus.h"

#include <QTimer>

namespace SKT {

class RouteCandidateListModel;

class RouteCandidateViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Activated,
        Deactivated,
        Mediated,
        RouteSelected,
        RouteViaSettingClicked,
        VisibleChanged,
        ViewSizeChanged,
        RequestCancelCountdown,
        ReturnClicked,
        RequestRoute,
        RetranslateUi,
        SetupSoftkeyMenu
    };
    enum MenuK {
        MenuAlternatives = 0,
        MenuWaypoints,
        MenuStart,
        OptionRouteSummary,
        OptionDrivingSimulation
    };

    Q_INVOKABLE RouteCandidateViewMediator( QObject* viewComponent = nullptr );
    virtual ~RouteCandidateViewMediator();

    QList<int> listNotificationInterests();

    bool activated() const;
    void setActivated(bool activated);

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool applyRouteSetting(const QVariant &data);
    void onViewSizeChanged();
    void openErrorView(int errorCode);
    void openRouteSummaryView();
    void openSimulationView();
    void onUpdateRouteSummary(const QVariantMap &m);
    void updateRouteSummary(const QVariant &body, bool byContinue, bool applyTimer=true);
    void requestRoute();
    void requestRouteSingle();
    void requestRouteSingle(const QVariant& v);
    void requestRouteMulti();
    void requestRouteByVr();
    void setupRouteOptions(const QVariantMap &);
    void requestLocalRouteSummary(int option, bool byVr = false);
    void requestServerRouteSummary(bool byVr = false);
    void reset();
    void restoreTvas();
    void procSoftkey(const QVariant &body);
    void goBack();
    void close();
    void cancelCountdownMenu();
    void updateStartRouteGuidanceMenu(bool);
    void updateOtherPassMenu(bool);
    void updateMapViewPadding();
    void updateModel(const QString& from, const QString& to);
    void updateRoadInfo(const QString& from, const QString& to);
    QVariant copyModel() const;

    void blockSoftkey(const QString &reason = QString());
    void unblockSoftkey();
    void setupStartGuideMenu();
    void postErrorViewClosed(const QVariant& v);

    void setupSoftkeyMenu(const QStringList& menus, bool enableOption=true);

    void addDummyData();
    QVariant makeRouteTypeSettings();
    void showSwitchToLocalToast();

    Q_INVOKABLE void drawRouteAll();
    Q_INVOKABLE void rescanTvas();

    int mRouteIndex;
    int mSelectedRouteIndex;
    int mFee;
    int mRemainTime;
    int mDistance;
    bool mInitialized;
    bool mUpdateCompleted;
    bool mDisableSoftkeyMenu;
    bool mIsOnline;
    bool mIsSoftkeyCreated;
    bool mIsActivated;
    bool mIsLocalRequested;
    bool mIsServerRequested;
    bool mIsServerReceived;
    bool mBlockSoftkey;
    bool mIsCountdownCanceled;
    bool mUpdateMapViewPaddingForce;
    bool mWillBeDeleted = false;
    bool mBlockActivate = false;
    bool mIsContinued = false;
    bool mForceOtherPass = false;
    bool mIsUrgentMode = false;
    RouteCandidateListModelPtr model;
    QList<int> mRouteOptions;
    QString mBlockSoftkeyReason;
    RpStatus mRpStatus;
    bool mBlockSetTvas = false;
    bool mShowTimer = false;
    // 언어 변경 시 이전에 설정했던 소프트키 메뉴를 재설정하기 위해 백업
    QVariantMap mSoftkeyMenu;
    QVariantMap mSoftkeyEditMenu;
    QTimer mDrawTimer;
    QTimer mTvasRescanTimer;

    // Update된 summary (경유지 설정 화면으로 이동했는 데 RP가 끝났을 때...)
    QVariantMap mRouteSummaryVariantMap;
    // 모의 주행 진입 조건을 위한 현재 속도
    int mCurrentSpeed = 0;

    bool mIsGadgetMode = false;
    bool mChannelServerPolicyDone = false;
    bool mChannelServerPolicyFinished = false;
    QString mRescanType;
    int mRequestedIndex = -1;
    int mRescanCount = 0;
};

}

#endif // ROUTECANDIDATEVIEWMEDIATOR_H
