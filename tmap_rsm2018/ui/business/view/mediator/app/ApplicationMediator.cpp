/*!
    \class ApplicationMediator
    \brief View 생성/소멸/활성화와 공통 UI를 다룬다( Zone, Keyboard, Toast )

    ApplicationMediator에서도 마찬가지로 다른 view의 메소드를 직접 호출하거나(invoke) 또는 프로퍼티를 read/write 하지 않도록 부탁 드립니다
    용도 이외 작업은 적절한 다른 위치에서 해주시기 부탁 드립니다.
    불필요한 메소드나 멤버변수 추가하지 않도록 부탁 드립니다.

    구조가 유지될 수 있도록 협조 부탁 드리며 개선이 필요한 것은 hskim에게 요청주시기 바랍니다
*/

#include "ApplicationMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "SimulatorConstants.h"
#include "KeyboardConstants.h"
#include "Facade.h"
#include "RouteSummaryVo.h"
#include <QVariant>
#include "mediator/search/RecentTileGridViewMediator.h"
#include "mediator/app/MapViewMode.h"
#include "RouteCandidateListModel.h"
#include "mediator/keyboard/KeyboardViewMediator.h"
#include "mediator/search/SearchTabViewMediator.h"
#include "mediator/search/AddressSearchViewMediator.h"
#include "mediator/search/RegionSearchViewMediator.h"
#include "mediator/keyboard/KeyboardViewMediator.h"
#include "mediator/map/SimulationViewMediator.h"
#include "ViewRegistry.h"
#include <QQuickItem>
#include <QQuickWindow>
#include <QDesktopWidget>
#include <QScreen>
#include "SettingDto.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"
#include "KeyboardConstants.h"
#include "ToastsConstants.h"
#include "RouteConstants.h"
#include "SimulatorConstants.h"
#include "UtilConstants.h"
#include "CommonConstants.h"
#include "QmlAppHelper.h"
#include "ApplicationConstants.h"
#include "uiconstants.h"
#include "NavigationControllerConstants.h"
#include "DebugUtils.h"
#include <QtWidgets/QStyle>
#include "HiddenSettingConstants.h"
#include "PlatformHelper.h"
#include "QmlOffscreenRenderer.h"
#include "ResourceResolver.h"
#include "ImageProvider.h"
#include "mapinclusterlayout.h"
#include "MultiSenseConstants.h"
#include "NavigationControllerProtocol.h"
#include "NavigationUpdateManagerConstants.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "navigationdata.h"
#include "../../../../../update/packageinfo.h"
#include "../common/navigationcontroller.h"
#include "GadgetOffscreenRenderer.h"
#include "SearchConstants.h"

using namespace SKT;

ApplicationMediator::ApplicationMediator( QObject* viewComponent, QQmlEngine* qmlEngine, bool hasMapData, QVariantMap gadgetSignal )
    : AbstractViewMediator( viewComponent ),
    mQmlEngine( qmlEngine ),
    mState( ApplicationConstants::StateNone )
{
    QObject::connect( &mViewTree, &ViewTree::visibilityChanged, this, &ApplicationMediator::onViewTreeVisibilityChanged );

    mHasMapData = hasMapData;
    mLastGadgetSignal = gadgetSignal;

    auto window = qobject_cast<QQuickWindow*>(mViewComponent);

#ifdef BUILD_TARGET
    window->setFlags(Qt::FramelessWindowHint);
#endif

#ifdef BUILD_TARGET
//    connect(&mTestTimer, &QTimer::timeout, this, [&](){
//        if(QFile::exists("/var/opt/bosch/navdata/log/screenshot/ui_test")) {
//            createView(
//                QVariantMap{
//                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::TestView)},
//                    {QStringLiteral("parentObjectName"),"testWrap"}
//                });
//        }
//    });
//    mTestTimer.setInterval(100);
//    mTestTimer.start();
#endif

#ifdef QT_DEBUG
    window->installEventFilter( this );
//    window->setGeometry(
//        QStyle::alignedRect(
//            Qt::LeftToRight,
//            Qt::AlignCenter,
//            window->size(),
//            window->screen()->availableGeometry()
//        )
//    );
#endif

    initGadgetHanlder();
}

ApplicationMediator::~ApplicationMediator()
{
    if ( mGadgetRenderer ) {
        mGadgetRenderer->stop();
        delete mGadgetRenderer;
        mGadgetRenderer = nullptr;
    }

    if ( mMapInClusterRenderer ) {
        delete mMapInClusterRenderer;
    }

    if ( mGadgetDebugWindow ) {
        mGadgetDebugWindow->close();
        mGadgetDebugWindow->deleteLater();
        mGadgetDebugWindow = nullptr;
    }
}

void ApplicationMediator::__sendEvent( int event, const QVariant& data )
{
    onEvent( event, data );
}

bool ApplicationMediator::eventFilter(QObject *target, QEvent *event )
{
    auto ctrlKey = QApplication::keyboardModifiers() & Qt::ControlModifier;
    if (event->type() == QEvent::MouseButtonRelease) {
        auto me = static_cast<QMouseEvent*>(event);

        QRect hitRect( 0, 140, 100, 100 );
#ifdef QT_DEBUG
        if ( hitRect.contains( me->pos() ) ) {
            createView(
                QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::TestView)},
                    {QStringLiteral("parentObjectName"),"testWrap"}
                });
        }
#endif
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        auto me = static_cast<QMouseEvent*>(event);
//        T_DEBUG( QString("MouseButtonPress: (%1,%2)").arg( me->pos().x() ).arg( me->pos().y() ) );
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        auto me = static_cast<QMouseEvent*>(event);
//        T_DEBUG( QString("MouseButtonRelease: (%1,%2)").arg( me->pos().x() ).arg( me->pos().y() ) );
    }
    else if (event->type() == QEvent::MouseMove) {
        auto me = static_cast<QMouseEvent*>(event);
//        T_DEBUG( QString("MouseMove: (%1,%2)").arg( me->pos().x() ).arg( me->pos().y() ) );
    }

    return QObject::eventFilter(target, event);
}

QList<int> ApplicationMediator::listNotificationInterests()
{
    static QList<int> notes = {
        Constants::CreateView,
        Constants::ShowView,
        Constants::HideView,
        Constants::CloseView,
        ZoneConstants::ShowRecentTileGridView,
        ZoneConstants::IgnoreRecentTileGridView,
        ZoneConstants::ShowSoftKeyBackground,
        SimulatorConstants::UpdateSimulator,
        ZoneConstants::FocusZone1TextInput,
        KeyboardConstants::ShowKeyboard,
        KeyboardConstants::HideKeyboard,
        KeyboardConstants::RegistCommonKeyboardInput,
        KeyboardConstants::RegistKeyboardInput,
        ZoneConstants::RegistZone1,
        ZoneConstants::RegistZone3,
        ZoneConstants::UpdateModal,
        MapConstants::ChangeMapViewMode,
        MapConstants::RestoreMapViewMode,
        MapConstants::NotifyMapViewMode,
        ToastsConstants::ShowToast,
        MapConstants::RegistGlobalMapViewRenderer,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        SettingConstants::ResponseHiddenSetting,
        HiddenSettingConstants::ChangeTheme,
        PreferenceConstants::ApplyPreference,
        PreferenceConstants::ChangedPreference,
        RgConstants::CloseNaviMenuAll,
        RgConstants::StartRouteGuidanceContinuously,
        RgConstants::ShowRouteGuidanceContinuously,
        MapConstants::RequestDriveMode,
        UtilConstants::ShowProgressIndicator,
        UtilConstants::HideProgressIndicator,
        ApplicationConstants::ChangeState,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
        ApplicationConstants::EnableBlur,
        ApplicationConstants::ResetNavi,        
        NavigationControllerConstants::GoToStandby,
        NavigationControllerConstants::GoToRunning,
        RouteConstants::RestoreRoute,
        ApplicationConstants::RequestViewList,
        HiddenSettingConstants::ChangeVsmDebugSettingUi,
        MapConstants::StartMapInCluster,
        MapConstants::StopMapInCluster,
        MapConstants::ChangeMapInClusterLayout,
        ApplicationConstants::ShowGadget,
        ApplicationConstants::RequestGadgetState,
        MultiSenseConstants::RequestColorState,
        MultiSenseConstants::ChangedColorState,
        NavigationControllerConstants::FirstNavigationAppeared,
        NavigationControllerConstants::NavigationAppeared,
        NavigationControllerConstants::NavigationDisappeared,
        SettingConstants::RequestHiddenSettingByReset,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        NavigationControllerConstants::ResponseController,
        NavigationControllerConstants::NavigationAppeared,
        NavigationControllerConstants::ContextSwitchRequested,
        ApplicationConstants::UpdateImageProvider,
        ApplicationConstants::FocusToWidget
    };

    return notes;
}

bool ApplicationMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {

    case ComponentCompleted:
        {
            qDebug() << "[Perf][Init]after displaying application window";
            handleStateChange( mHasMapData ? ApplicationConstants::StateWithMapData : ApplicationConstants::StateNoMapData );

            auto gadgetIsValid = mLastGadgetSignal["isValid"].toBool();
            auto gadgetType = mLastGadgetSignal["gadgetType"].toInt();
            auto gadgetActivate = mLastGadgetSignal["gadgetActivate"].toBool();
            qDebug() << "[Perf][Init]last gadget signal isValid: " << gadgetIsValid << " type: " << gadgetType << " activate: " << gadgetActivate;

            if ( gadgetIsValid ) {
                QVariantMap params;
                params[QStringLiteral("type")] = gadgetType;
                params[QStringLiteral("show")] = gadgetActivate;
                //params[QStringLiteral("asView")] = true;
                sendNotification( ApplicationConstants::ShowGadget, params );
            }

            if (mHasMapData == ApplicationConstants::StateWithMapData) {
                // search - Performance improvements
                QTimer::singleShot(30, this, [=](){
                    createView(
                        QVariantMap{
                            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::TextSearchView)},
                            {QStringLiteral("parent"),QVariant::fromValue(mContentRoot)},
                            {CommonConstants::Visible,QVariant::fromValue(false)}
                        });
                    createView(
                        QVariantMap{
                            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SearchResultView)},
                            {QStringLiteral("parent"),QVariant::fromValue(mContentRoot)},
                            {CommonConstants::Visible,QVariant::fromValue(false)}
                        });
                    createView(
                        QVariantMap{
                            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SearchResultDetailView)},
                            {QStringLiteral("parent"),QVariant::fromValue(mContentRoot)},
                            {CommonConstants::Visible,QVariant::fromValue(false)}
                        });
                });
            }
        }
        break;
    case CaptureScreenshot: {
            if (mUseScreenShot)
                sendNotification(HiddenSettingConstants::CaptureScreenshot);
        }
        break;
    case RegisterMapInClusterWindow: {
            //mMapInClusterWindow = data.value<QQuickWindow*>();
        }
        break;
    case TestGadgetPressed: {
            auto m = data.toMap();
            auto type = (UIConstants::GadgetType)m["type"].toInt();
            auto pressed = m["pressed"].toBool();
            sendNotification(ApplicationConstants::FocusToWidget, QVariantMap{
                                 {CommonConstants::FocusIndex, type},
                                 {CommonConstants::Focus, pressed}
                             });
        }
        break;
    default:
        return false;
    }

    return true;
}

bool ApplicationMediator::handleNotification( const mvc::INotificationPtr& note )
{
    auto noteId = note->getId();
    switch ( noteId ) {

/*
    // Map Data
    case MapUpdateCompleted: {
            if ( mHasMapData ) {
                QQmlProperty::write( mLoader, QStringLiteral("source"), mWithMapDataComp );
            }
        }
        break;
*/

    // Common

    case Constants::CreateView: {
            auto args = note->getBody().value<QVariantMap>();
            createView( args );
        }
        break;
    case Constants::ShowView: {
            showView( note->getBody().value<QVariantMap>() );
        }
        break;
    case Constants::HideView: {
            hideView( note->getBody().value<QVariantMap>() );
        }
        break;
    case Constants::CloseView: {
            closeView( note->getBody().value<QVariantMap>() );
        }
        break;
    // Toast

    case ToastsConstants::ShowToast: {
            handleToast( note );
        }
        break;

    // Softkey

    // Keyboard
    case KeyboardConstants::RegistCommonKeyboardInput: {
            auto args = note->getBody().value<QVariantMap>();
            mCommonKeyboardInput = args[QStringLiteral("keyboardInput")].value<QObject*>();
        }
        break;
    case KeyboardConstants::RegistKeyboardInput: {
            auto args = note->getBody().value<QVariantMap>();
            auto value = args[QStringLiteral("keyboardInput")];
            if ( !value.isNull() ) {
                mKeyboardInput = value.value<QObject*>();
            }
            else {
                mKeyboardInput = mCommonKeyboardInput;
            }
        }
        break;

    case KeyboardConstants::ShowKeyboard: {
#ifdef BUILD_TARGET
        auto m = note->getBody().toMap();
        //auto create = m["create"].toBool();
        auto hasView = mViewIdList.contains(ViewRegistry::KeyboardView);
        auto create = !hasView;
        if (create) {
            // KeyboardViewMediator를 미리 생성해 놓는다.
            // ProcessSystemKeyboard를 처리하기 위해...
            // 생성 시점은 Zone1View가 Mediated 될 때.
            QVariantMap args;
            args[CommonConstants::Url] = ViewRegistry::url( ViewRegistry::KeyboardView );
            args[CommonConstants::Visible] = false;
            args[QStringLiteral("parent")] = QVariant::fromValue(mContentRootWithoutSoftkey);

            //타겟에서 아래 항목 접근 시 crash 남
            //18.07.27 ymhong
            //args[QStringLiteral("inputField")] = QVariant::fromValue( mKeyboardInput );
            createView(args );
        } else {
            m[QStringLiteral("parent")] = QVariant::fromValue(mContentRootWithoutSoftkey);
            //타겟에서 아래 항목 접근 시 crash 남
            //18.07.27 ymhong
            //m[QStringLiteral("inputField")] = QVariant::fromValue( mKeyboardInput );
            sendNotification(KeyboardConstants::UpdateKeyboard, m);
        }

        if (create) {
            qDebug() << "KeyboardConstants::ShowKeyboard( created )";
            return true;
        }

        qDebug() << "PlatformHelper::instance()->setupKeyboard(m) from KeyboardConstants::ShowKeyboard, " << note->getSource()->metaObject()->className();;

        auto hasKey = m.contains(CommonConstants::DontUseEditBox);
        auto dontUseEditBox = m[CommonConstants::DontUseEditBox].toBool();
        auto numberKeyboard = m["numberKeyboard"].toBool();
        m.clear();
        m["show"] = true;
        m["numberKeyboard"] = numberKeyboard;
        if (hasKey) {
            m[CommonConstants::DontUseEditBox] = dontUseEditBox;
        }

        PlatformHelper::instance()->setupKeyboard(m);

        // for Monkey test
        {
            auto m2 = note->getBody().toMap();
            m2[CommonConstants::Url] = ViewRegistry::url( ViewRegistry::KeyboardView );
            m2[CommonConstants::Visible] = true;
            m2[QStringLiteral("parent")] = QVariant::fromValue(mContentRootWithoutSoftkey);
            m2[QStringLiteral("inputField")] = QVariant::fromValue( mKeyboardInput );
            sendNotification(KeyboardConstants::ShowKeyboardForMonkeyTest, m2);
        }
#else
            auto args = note->getBody().value<QVariantMap>();
            args[CommonConstants::Url] = ViewRegistry::url( ViewRegistry::KeyboardView );
            args[CommonConstants::Visible] = true;
            args[QStringLiteral("parent")] = QVariant::fromValue(mContentRootWithoutSoftkey);
            args[QStringLiteral("inputField")] = QVariant::fromValue( mKeyboardInput );
            createView(args );
            sendNotification(KeyboardConstants::UpdateKeyboard, args);
#endif
            hideView( QVariantMap{
                {CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::Zone3View)}
            } );
        }
        break;
    case KeyboardConstants::HideKeyboard: {
        auto args = note->getBody().value<QVariantMap>();
#ifdef BUILD_TARGET
        qDebug() << "PlatformHelper::instance()->setupKeyboard(m) from KeyboardConstants::HideKeyboard, " << note->getSource()->metaObject()->className();

        QVariantMap m;
        m["show"] = false;
        if (args.contains(CommonConstants::DontUseEditBox)) {
            m[CommonConstants::DontUseEditBox] = args[CommonConstants::DontUseEditBox];
        }
        PlatformHelper::instance()->setupKeyboard(m);
        args["show"] = false;
#endif
        args[CommonConstants::Visible] = false;
        sendNotification(KeyboardConstants::UpdateKeyboard, args);
            showView( QVariantMap{
               { CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::Zone3View)}
            } );
        }
        break;

    // MapViewMode

    case MapConstants::RegistGlobalMapViewRenderer: {
        //MMV 18.4.18
            //T_DEBUG( QString("RegistMapViewRenderer") );

            // 화면마다 MapViewQuick 인스턴스를 갖는 방식으로 변경될 때까지만 유지

            auto args = note->getBody().value<QVariantMap>();
            auto mapView = args[QStringLiteral("mapView")].value<QObject*>();
            mQmlEngine->rootContext()->setContextProperty( "mapView", mapView );

            sendNotification( MapConstants::AttachMapViewOnVsmNavi, QVariant::fromValue(args) );
            sendNotification( MapConstants::SetMapViewAsGlobal, QVariant::fromValue(args) );

            // request Map Auto Level Speed View mode
            QVariantMap autoLevelSetting;
            autoLevelSetting[CommonConstants::Section] = SettingConstants::MapSubSpeedResponse;
            autoLevelSetting[CommonConstants::Key] = SettingConstants::Distance;
            sendNotification( SettingConstants::RequestSetting, autoLevelSetting );

            sendNotification(SettingConstants::RequestSetting, QVariantMap{
                                 {CommonConstants::Section, SettingConstants::MapSubSpeedResponse},
                                 {CommonConstants::Key, SettingConstants::SpeedResponseMapUse}
                             });

            // request hidden setting to apply map, gps on startup
            QVariantMap reqHiddenSetting;
            reqHiddenSetting[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification( SettingConstants::RequestHiddenSetting, QVariant::fromValue(reqHiddenSetting));
            reqHiddenSetting[CommonConstants::Section] = HiddenSettingConstants::HiddenVsmSetting;
            sendNotification( SettingConstants::RequestHiddenSetting, QVariant::fromValue(reqHiddenSetting));

            // apply map init configuration from it stored
            sendNotification(HiddenSettingConstants::ApplyVsmLastDebugSetting);

            // request monkey test
            sendNotification( PreferenceConstants::RequestPreference,
                QVariantMap{
                    { CommonConstants::Caller, QStringLiteral("ApplicationMediator") },
                    { QStringLiteral("preferencesName"), PreferenceConstants::Test },
                    { CommonConstants::Section, QStringLiteral("MonkeyTest") }
                } );

            QVariantMap body;
            body.insert( "type", NavigationControllerProtocol::ApplyFontScale );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;

    case MapConstants::RequestDriveMode:
    case MapConstants::ChangeMapViewMode:
    case MapConstants::RestoreMapViewMode:
    case MapConstants::NotifyMapViewMode: {
            handleMapViewMode( note );
        }
        break;

    // Route Guidance

    case RgConstants::StartRouteGuidanceContinuously: {
            mIsRgContInProgress = true;
        }
        break;

    // preference
    case PreferenceConstants::ChangedPreference:
        {
            auto result = note->getBody().value<QVariantMap>();
            if ( result[QStringLiteral("preferencesName")].toString() == PreferenceConstants::Test ) {
                if ( result[CommonConstants::Section] == QStringLiteral("MonkeyTest") ) {
                    mEnableMonkeyTest = result[CommonConstants::Value].toBool();
                    T_DEBUG( QString("ChangedPreference #1 %1").arg(mEnableMonkeyTest) );
                }
            }
        }
        break;

    case PreferenceConstants::ApplyPreference:
        {
            auto result = note->getBody().value<QVariantMap>();
            auto req = result[QStringLiteral("request")].value<QVariantMap>();
            if ( req[QStringLiteral("preferencesName")].toString() == PreferenceConstants::Test ) {
                if ( req[CommonConstants::Section] == QStringLiteral("MonkeyTest") ) {
                    auto resp = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                    mEnableMonkeyTest = resp["enable"].value().toBool();
                    T_DEBUG( QString("ApplyPreference #1 %1").arg(mEnableMonkeyTest) );
                }
            }
            if ( req[CommonConstants::Caller].toString() == QStringLiteral("ApplicationMediator") ) {
                // 경로 이어 가기

                // 처음 앱 화면에 들어오기 전
                // 홈스크린에서 Patrol, Parking 위젯을 터치하면 주변 검색 화면이 뜨게 된다.
                // 이후 앱 화면으로 들어 오면서 경로 이어 하기 팝업이 뜨면서
                // 화면이 꼬이기 때문에 위젯 터치로 주변 검색을 하게 되면 경로 이어 하기를 하지 않도록 한다.
                // 19.01.22 ymhong.
                if (PlatformHelper::instance()->isRgContinueEnable()) {
                    auto resp = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                    auto isNormallyTerminated = resp["isNormallyTerminated"].value().toBool();
                    bool ok = false;
                    auto selectedRouteIndex = resp["selectedRouteIndex"].value().toInt( &ok );
                    if ( ok && !isNormallyTerminated ) {
                        if ( selectedRouteIndex > -1 ) {
                            mIsRgContInProgress = true;
                        }
                    }
                }
            }
        }
        break;

    // setting

    case SettingConstants::ApplySetting:
        {
            //T_DEBUG( "<B>ApplicationMediator ApplySetting</B>" );
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if (mHasMapData) {
                if ( sect == SettingConstants::MapMain && key == SettingConstants::RecentDestinations ) {
                    mShowRecentDestinations = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    //T_DEBUG( QString( "<B>RecentDestinations #2 '%1'</B>" ).arg( mShowRecentDestinations ) );
#if (0)
                    // TMCRSM-3042
                    // 최근 목적지 타일 뷰는 앱 실행 시점이 아닌
                    // 네비앱이 처음 forground 되는 시점에 생성 되도록 코드를 옮김
                    // 19.04.23 ymhong

                    if ( mShowRecentDestinations ) {
                        if ( !mIsRgContInProgress && PlatformHelper::instance()->isRgContinueEnable()) {
                            createView(
                                    QVariantMap{
                                {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RecentTileGridView)}
                            });
                            mIsRgContInProgress = false;
                        }
                    }
#endif
                }
            }
            if ( sect == SettingConstants::MapSubSpeedResponse && key == SettingConstants::Distance) {
                    mAutoLevelMode = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
            }
        }
        break;

    case HiddenSettingConstants::ChangeVsmDebugSettingUi:
           handleHiddenSetting( note );
       break;
    case SettingConstants::ResponseHiddenSetting: {
        auto body = note->getBody().toMap();
            auto owner = reinterpret_cast<void*>(body[CommonConstants::Owner].toULongLong());
            if (owner != this) break;

            handleHiddenSetting( note );
        }
        break;
    case HiddenSettingConstants::ChangeTheme:
           handleHiddenSetting( note );
       break;
    // Zone1

    // Frequently used

    case ZoneConstants::UpdateModal:
        {
            auto m = note->getBody().toMap();
            mIsModal = m["modal"].toBool();
        }
        break;

    case ZoneConstants::IgnoreRecentTileGridView:
        mIgnoreRecentTileGridView = true;
        break;

    case ZoneConstants::ShowRecentTileGridView: {
            if ( mCreateViewStatus.contains( ViewRegistry::url(ViewRegistry::RecentTileGridView) ) ) {
                closeView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::RecentTileGridView)}});
            }
            else {
                if (mIsModal) {
                    // ErrorView 가 떠 있는 상태면 최근 목적지 그리드뷰를 띄우지 않음.
                } else {
                    auto forceShowRecentTileGridView = false;
#ifdef QT_DEBUG
                    auto source = note->getSource();
                    QString caller = source->metaObject()->className();
                    if (caller.contains(ViewRegistry::objectName(ViewRegistry::HardKeyView))) {
                        forceShowRecentTileGridView = true;
                    }
#endif
                    if ( !mIgnoreRecentTileGridView && (mShowRecentDestinations || forceShowRecentTileGridView)) {
                        bool isSettingMapUpdateView = mCreateViewStatus.contains( ViewRegistry::url(ViewRegistry::SettingMapUpdateView));
                        auto showRecentTileGridView = !isSettingMapUpdateView && !mIsRgContInProgress && PlatformHelper::instance()->isRgContinueEnable();

                        if (showRecentTileGridView || forceShowRecentTileGridView) {
                            qDebug() << "[OTP] ShowTileGridView. -> CloseRouteCandidateView.";

                            sendNotification(SimulatorConstants::CloseSimulationView);
                            sendNotification(RouteConstants::CloseRouteCandidateView);
                            sendNotification(RouteConstants::CloseRouteSummaryView);

                            sendNotification(KeyboardConstants::HideKeyboard);

                            createView(
                                    QVariantMap{
                                {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RecentTileGridView)}
                            });
                            mIsRgContInProgress = false;
                            qDebug() << "RecentTileGridView created. ZoneConstants::ShowRecentTileGridView.";
                        }
                    }
                }
            }
        }
        break;

    case RgConstants::CloseNaviMenuAll:
        {
            QStringList views;
            auto index = ViewRegistry::staticMetaObject.indexOfEnumerator( "ViewK" );
            auto enumerator = ViewRegistry::staticMetaObject.enumerator( index );
            for ( int i = 0, count = enumerator.keyCount(); i < count; i++ ) {
                if ( ViewRegistry::hasTraits(i, ViewRegistry::Setting) ||
                     ViewRegistry::hasTraits(i, ViewRegistry::Search)  ||
                     ViewRegistry::hasTraits(i, ViewRegistry::Menu)) {
                      views.append(ViewRegistry::objectName(i));
                }
            }

            for (QString view: views) {
                sendNotification(Constants::CloseView,QVariantMap{{CommonConstants::ObjectName, view}});
            }
        }
        break;

    // Unsorted
    case RgConstants::ShowRouteGuidanceContinuously:
        {
            if (PlatformHelper::instance()->isRgContinueEnable()) {
                // 최근 목적지 표시?
                QVariantMap body;
                body[CommonConstants::Section] = SettingConstants::MapMain;
                body[CommonConstants::Key] = SettingConstants::RecentDestinations;
                sendNotification( SettingConstants::RequestSetting, body );

                // 경로 이어 가기
                sendNotification( PreferenceConstants::RequestPreference, QVariantMap{
                    { CommonConstants::Caller, QStringLiteral("ApplicationMediator") },
                    { SettingConstants::PreferencesName, PreferenceConstants::Rg },
                    { CommonConstants::Section, QStringLiteral("Termination") }
                } );
            }

            // 지난 경로 데이터(for NDDS)
            sendNotification( PreferenceConstants::RequestPreference, QVariantMap{
                {SettingConstants::PreferencesName, PreferenceConstants::Rg},
                {CommonConstants::Section, RgConstants::PastPos}
            });
        }
        break;

    case NavigationControllerConstants::FirstNavigationAppeared:
        {
            // 지도 데이타 업데이트 여부 조회
            QVariantMap data;
            data.insert( CommonConstants::Type, NavigationUpdateManagerConstants::GetStatusUpdateCommitted );
            sendNotification( NavigationUpdateManagerConstants::Get, data );

            qDebug() << "[OTP] FirstNavigationAppeared";
        }
        break;

    case NavigationControllerConstants::NavigationAppeared:
        {
            qDebug() << "[GadgetSwitch]NavigationAppeared";
            QQmlProperty::write(mViewComponent, "isNaviVisible", true);

            //stopGadget();
        }
        break;

    case NavigationControllerConstants::ContextSwitchRequested:
        {
            qDebug() << "[GadgetSwitch]ContextSwitchRequest";
            auto m = note->getBody().toMap();
            auto request = m[QStringLiteral("request")].toInt();
            if ( request == NavigationController::MAP_MAIN || request == NavigationController::NAVIGATION_SERVICE ) {
                stopGadget();
            }
        }
        break;

    case NavigationControllerConstants::NavigationDisappeared:
        {
            qDebug() << "[GadgetSwitch]NavigationDisappeared";
            QQmlProperty::write(mViewComponent, "isNaviVisible", false);
        }
        break;

    // Util
    case UtilConstants::ShowProgressIndicator: {
            auto m = note->getBody().toMap();
            auto isReroute = m["reroute"].toBool();

            createView(
                QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::ProgressIndicatorView)},
                    {QStringLiteral("parentObjectName"),"progressWrap"},
                    {QStringLiteral("isReroute"), isReroute}
                });

            // 경로 안내중에는 상태가 바뀌지 말아야 한다.
            // 19.06.07 ymhong.
            if ( !mIsRg ) {
                QVariantMap data;
                data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                data["propertyValue"] = TMAPNAVISTATUS_BUSY;
                sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
            }
        }
        break;
    case UtilConstants::HideProgressIndicator: {
            closeView(
                QVariantMap{
                    {CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::ProgressIndicatorView)}
                });

            // 경로 안내중에는 상태가 바뀌지 말아야 한다.
            // 19.06.07 ymhong.
            if (!mIsRg) {
                QVariantMap data;
                data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                data["propertyValue"] = TMAPNAVISTATUS_ANDO;
                sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
            }
        }
        break;

    case ApplicationConstants::ChangeState:{
            QVariantMap m = note->getBody().toMap();
            auto state = m[QStringLiteral("state")].toInt();
            handleStateChange( (ApplicationConstants::StateK)state );
        }
        break;

    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile: {
            handleUserProfileChange( note );
        }
        break;

    case ApplicationConstants::EnableBlur:
        QQmlProperty::write(mViewComponent, "enableBlur", note->getBody());
        break;

    case ApplicationConstants::ResetNavi:
        resetNavi(note->getBody().toMap());
        break;

    case NavigationControllerConstants::GoToStandby: {
            QQmlProperty::write( mViewComponent, QStringLiteral("interaction"), false );
        }
        break;

    case NavigationControllerConstants::GoToRunning: {
            QQmlProperty::write( mViewComponent, QStringLiteral("interaction"), true );
        }
        break;

    case RouteConstants::RestoreRoute:
        {
            auto m = note->getBody().toMap();
            if (m.contains("restore") && m["restore"].toBool()) {
                mForceNaviMode = true;
            }
        }
        break;

    case ApplicationConstants::RequestViewList: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            data[ApplicationConstants::ViewIdList] = QVariant::fromValue( mViewIdList );

            for ( auto viewId : mViewIdList ) {
                auto objName = ViewRegistry::objectName( viewId );
                auto view = mViewTree.findViewByObjectName( objName );
                if ( view == nullptr ) {
                    T_DEBUG( QString( "Detected a mismatch for %1 between the view list and tree" ).arg( objName ) );
                }
                else {
                    if ( !mViewDataMap.contains( viewId ) ) {
                        T_DEBUG( QString( "Detected a mismatch for %1 between the view list and data map" ).arg( objName ) );
                    }
                    else {
                        auto& data = mViewDataMap[viewId];
                        auto visible = QQmlProperty::read( view, ApplicationConstants::Visible ).toBool();
                        auto active = QQmlProperty::read( view, ApplicationConstants::Active ).toBool();
                        data[ApplicationConstants::Visible] = visible;
                        data[ApplicationConstants::Active] = active;
                    }
                }
            }

            data[ApplicationConstants::ViewDataMap] = QVariant::fromValue( mViewDataMap );
            sendNotification( ApplicationConstants::ReplyToViewList, data );
        }
        break;
    case MultiSenseConstants::RequestColorState:
        {
            sendNotification(MultiSenseConstants::ReplyToColorState, QmlAppHelper::instance()->multiSenseColorState() );
        }
        break;
    case MapConstants::StartMapInCluster:
    case MapConstants::StopMapInCluster:
    case MapConstants::ChangeMapInClusterLayout: {
            handleMapInCluster( note );
        }
        break;
    case ApplicationConstants::RequestGadgetState:
    case ApplicationConstants::ShowGadget:
    case ApplicationConstants::FocusToWidget: {
            handleWidget( note );
        }
        break;
    //환경설정 초기화 및 공장초기화할 때 호출
    case SettingConstants::RequestHiddenSettingByReset: {
            // request hidden setting to apply map, gps on startup
            QVariantMap reqHiddenSetting;
            reqHiddenSetting[CommonConstants::Section] = HiddenSettingConstants::HiddenSetting;
            sendNotification( SettingConstants::RequestHiddenSetting, QVariant::fromValue(reqHiddenSetting));
            reqHiddenSetting[CommonConstants::Section] = HiddenSettingConstants::HiddenVsmSetting;
            sendNotification( SettingConstants::RequestHiddenSetting, QVariant::fromValue(reqHiddenSetting));
        }
        break;

    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState: {
            QVariantMap m = note->getBody().toMap();
            mIsRg = m["isRg"].toBool();


            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::ApplyRgStatus);
            body.insert( CommonConstants::Value, mIsRg);
            sendNotification( NavigationUpdateManagerConstants::Apply, body );
        }
        break;

     case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::NoMapData)) {
                int err = data[CommonConstants::NoMapData].toInt();
                QVariantMap body;
                body.insert(CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView));
                switch(err) {
                    case PackageContainer::E_ERROR_FILEOPEN:
                    case PackageContainer::E_ERROR_FILEMPTY:
                    case PackageContainer::E_ERROR_JSONULL:
                    case PackageContainer::E_ERROR_INVALIDVERSION:
                    case PackageContainer::E_ERROR_MESSAGE:
                    case PackageContainer::E_ERROR_SIGNATURE:
                    case PackageContainer::E_ERROR_SHA256:
                        qDebug() << "#1 Create SettingMapUpdateView.<<NoMapData";
                        body.insert(CommonConstants::Caller, CommonConstants::NoMapData);
                        qDebug() << "#2 Create SettingMapUpdateView.<<NoMapData";
                        break;
                    case PackageContainer::E_ERROR_VERIFYSIG:
                    case PackageContainer::E_ERROR_PKGINFO:
                    case PackageContainer::E_ERROR_SUCCESS:
                    default:
                        qDebug() << "#1 Create SettingMapUpdateView.<<IncompatibleMapData";
                        body.insert(CommonConstants::Caller, CommonConstants::IncompatibleMapData);
                        qDebug() << "#2 Create SettingMapUpdateView.<<IncompatibleMapData";
                        break;
                }
                createView(body);
            }
        }
        break;
    case ApplicationConstants::UpdateImageProvider: {
            auto m = note->getBody().toMap();
            auto type = m[CommonConstants::Type].toInt();
            if ( type == ApplicationConstants::OnlineCrossImage ) {
                qDebug() << "[Cross]ApplicationMediator UpdateImageProvider OnlineCrossImage";
                auto image = m[CommonConstants::Image].value<QImage>();
                auto provider = dynamic_cast<ImageProvider*>(mQmlEngine->imageProvider(QStringLiteral("ImageProvider")));
                provider->updateImage( (ApplicationConstants::ImageType)type, image );
            }
        }
        break;

    default:
        return false;
    }

    return true;
}

void ApplicationMediator::resetNavi(const QVariantMap &m)
{
    if (mViewIdList.contains(ViewRegistry::SimulationView)) {
        sendNotification(SimulatorConstants::StopSimulator, QVariantMap{
                             {"cancelRoute", true}
                         });
    }
    if (mViewIdList.contains(ViewRegistry::RouteCandidateView)) {
        // 경로 상세 화면 상태에서 뷰들을 닫으면 출발지, 도착지 마커가 남아 있는 상태.
        // 이의 처리를 위해 아래를 호출 한다.
        // 홈 메뉴에서 Patrol, Parking 가젯 터치 시 맵 화면에 마커들이 남아 있음.
        // 19.03.20 ymhong.
        sendNotification(RouteConstants::CancelRoute);
    }

    closeUserProfileDependentViews();

    // 프로파일 전환 후 Zone3가 안보이는 경우가 있음.
    // 타겟 키보드 감추어야 함.
    // Blur Off 해야 함.
    // 모의 주행 멈추어야 함.
    // 모의 주행중 팝업을 띄웠을 때 사라지지 않음.
    sendNotification(KeyboardConstants::HideKeyboard);
    sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal",false}}); // keyboard가 감쳐져야 제대로 적용.
    sendNotification(ZoneConstants::ShowZone3, ZoneConstants::Immediately);
    sendNotification(ZoneConstants::BlockSoftkeyMenu, false);
    sendNotification(ZoneConstants::BlockNaviMenu, false);
    sendNotification(ApplicationConstants::EnableBlur, false);
    sendNotification(ApplicationConstants::BlockSearchTabView, false);

    // 전체 창을 닫으면 통합검색창도 닫기므로 통합검색가젯 모드도 리셋한다.
    // 19.05.30 ymhong
    sendNotification(SearchConstants::SetGadgetSearchMode, false);

    // TMCRSM-3742
    // 경로 안내 중 경유지 추가를 하고 있을 때 ResetNavi가 호출 되면
    // 경유지 설정 화면이 닫히므로 추가중이던 경유지 정보를 리셋한다.
    // 19.09.05 ymhong
    auto restoreVia = true;
    if (m.contains("restoreVia")) {
        restoreVia = m["restoreVia"].toBool();
    }
    if (mIsRg && restoreVia) {
        sendNotification(RouteConstants::RestoreViaData);
    }
}

void ApplicationMediator::closeUserProfileDependentViews()
{
    // 팝업 닫음
    QVariantMap body;
    body[QStringLiteral("release")] = ViewRegistry::Popup;
    body[QStringLiteral("hold")] = ViewRegistry::Dialog;
    sendNotification( Constants::CloseView, body );
    sendNotification(ApplicationConstants::CloseCommonDialogWithoutMapUpdate);

    closeView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::MapControlView)}});
    closeView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::MapModeOptionView)}});
    closeView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::SimulationView)}});

    mBlankWrap = findChild( "blankWrap" );
    mZoneWrap = findChild( CommonConstants::ZoneWrap );
    mContentWrap = findChild( CommonConstants::ContentWrap );
    mContentRoot = findChild( CommonConstants::ContentRoot );
    mContentRootWithoutSoftkey = findChild( CommonConstants::ContentRootWithoutSoftkey );
    mMapViewWrap = findChild(CommonConstants::MapViewWrap );
    mToastWrap = findChild( CommonConstants::ToastWrap );
    mAlertWrap = findChild( CommonConstants::AlertWrap );
    mProgressWrap = findChild( CommonConstants::ProgressWrap );
    mWidgetWrap = findChild( CommonConstants::WidgetWrap );
}
