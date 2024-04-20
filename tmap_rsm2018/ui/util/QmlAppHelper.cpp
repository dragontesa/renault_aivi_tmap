#include "QmlAppHelper.h"
#include <QDesktopWidget>
#include <QFontDatabase>

#include "TSettings.h"
#include "ImageProvider.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "HiddenSettingConstants.h"
#include "SearchConstants.h"
#include "Facade.h"
#include "RgDataVo.h"
#include "RgProtocol.h"
#include "ResourceResolver.h"

#include "Facade.h"
#include "ViewRegistry.h"
#include "Ring.h"
#include "Csw.h"

#include "mediator/app/ApplicationMediator.h"
#include "mediator/app/MapViewMode.h"
#include "mediator/app/CommonDialogViewMediator.h"

#include "mediator/zone/HardKeyViewMediator.h"
#include "mediator/zone/Zone1ViewMediator.h"
#include "mediator/zone/Zone3ViewMediator.h"
#include "mediator/zone/ZoneApiTestViewMediator.h"
#include "mediator/zone/OptionMenuViewMediator.h"

#include "mediator/keyboard/KeyboardViewMediator.h"
#include "mediator/search/RegionSearchViewMediator.h"


#include "mediator/menu/NaviMenuViewMediator.h"


#include "mediator/dbg/ErrorViewMediator.h"
#include "mediator/dbg/TestViewMediator.h"
#include "mediator/dbg/MonkeyTestViewMediator.h"
#include "mediator/dbg/TestMultiMapViewMediator.h"

#include "mediator/map/MapViewMediator.h"
#include "mediator/map/SimulationViewMediator.h"
#include "mediator/map/MapViewMediator.h"
#include "mediator/map/MapInClusterViewMediator.h"
#include "mediator/map/MapInClusterSideViewMediator.h"
#include "mediator/map/MapControlViewMediator.h"
#include "mediator/map/MapModeOptionViewMediator.h"

#include "mediator/rg/TbtViewMediator.h"
#include "mediator/rg/LaneViewMediator.h"
#include "mediator/rg/HighwayTbtViewMediator.h"
#include "mediator/rg/SdiViewMediator.h"
#include "mediator/rg/TbtViewMediator.h"
#include "mediator/rg/SdiViewMediator.h"
#include "mediator/rg/LaneViewMediator.h"
#include "mediator/rg/CrossImageViewMediator.h"

#include "mediator/rg/FullscreenAlertViewMediator.h"
#include "mediator/rg/HiPassLaneViewMediator.h"
#include "mediator/rg/SummaryTbtViewMediator.h"
#include "mediator/rg/SpeedMeterViewMediator.h"

#include "mediator/rg/BusLaneViewMediator.h"
#include "mediator/rg/TrafficBarViewMediator.h"
#include "mediator/rg/TrafficBarGraph.h"
#include "mediator/rg/ChangeDestinationViewMediator.h"
#include "mediator/rg/PoiTabViewMediator.h"

#include "mediator/route/RouteCandidateViewMediator.h"
#include "mediator/route/RouteSummaryViewMediator.h"
#include "mediator/route/RouteSettingViewMediator.h"
#include "mediator/route/RouteRegistViewMediator.h"
#include "mediator/route/RouteSummaryViewMediator.h"

#include "mediator/setting/SettingCarIconViewMediator.h"
#include "mediator/setting/SettingGpsInfoViewMediator.h"
#include "mediator/setting/SettingGuidViewMediator.h"
#include "mediator/setting/SettingGuidVoiceViewMediator.h"
#include "mediator/setting/SettingMainViewMediator.h"
#include "mediator/setting/SettingMapCharSizeViewMediator.h"
#include "mediator/setting/SettingMapViewMediator.h"
#include "mediator/setting/SettingMapUpdateViewMediator.h"
#include "mediator/setting/SettingNaviInfoViewMediator.h"
#include "mediator/setting/SettingMobileTmapViewMediator.h"
#include "mediator/setting/SettingAttentionSectionViewMediator.h"
#include "mediator/setting/SettingSpeedResponseMapViewMediator.h"
#include "mediator/setting/SettingCameraWarningSoundViewMediator.h"
#include "mediator/setting/SettingRouteSearchViewMediator.h"
#include "mediator/setting/SettingRouteAlternateViewMediator.h"
#include "mediator/setting/SettingVoiceGuidTimeViewMediator.h"
#include "mediator/setting/SettingOilKindViewMediator.h"
#include "mediator/setting/SettingOilBrandViewMediator.h"
#include "mediator/setting/SettingOilAreaViewMediator.h"
#include "mediator/setting/SettingAQIdetailViewMediator.h"
#include "mediator/setting/SettingEvChargeTypeViewMediator.h"
#include "mediator/setting/SelfTestViewMediator.h"
#include "mediator/setting/GadgetShortcutIconViewMediator.h"

// hidden menu
#include "mediator/setting/HiddenDiagSettingViewMediator.h"
#include "mediator/setting/HiddenVsmDiagSettingViewMediator.h"
#include "mediator/setting/HiddenBoschSettingViewMediator.h"
#include "mediator/setting/HiddenMenuLoginViewMediator.h"
#include "mediator/setting/HiddenSettingFontDpViewMediator.h"

#include "mediator/search/BookmarkViewMediator.h"
#include "mediator/search/BookmarkPageViewMediator.h"
#include "mediator/search/RecentDestinationViewMediator.h"
#include "mediator/search/RecentDestinationPageViewMediator.h"
#include "mediator/search/RecentDestinationEditViewMediator.h"
#include "mediator/search/RecentSearchEditViewMediator.h"
#include "mediator/search/RecentSearchPageViewMediator.h"
#include "mediator/search/NoResultPageViewMediator.h"
#include "mediator/search/SearchResultDetailViewMediator.h"
#include "mediator/search/SearchResultViewMediator.h"
#include "mediator/search/SearchTabViewMediator.h"
#include "mediator/search/TextSearchViewMediator.h"
#include "mediator/search/IndustrySearchViewMediator.h"
#include "mediator/search/SearchResultOptionViewMediator.h"
#include "mediator/search/SearchAroundViewMediator.h"
#include "mediator/search/MapSearchViewMediator.h"
#include "mediator/search/RecentTileGridViewMediator.h"
#include "mediator/search/AddressSearchViewMediator.h"
#include "mediator/search/BookmarkRegistViewMediator.h"
#include "mediator/search/BookmarkPageEditViewMediator.h"
#include "mediator/util/ProgressIndicatorViewMediator.h"

#include "mediator/widget/WidgetSmallViewMediator.h"
#include "mediator/widget/WidgetMediumViewMediator.h"
#include "mediator/widget/WidgetLargeViewMediator.h"
#include "mediator/widget/WidgetXlViewMediator.h"
#include "mediator/widget/WidgetXxlViewMediator.h"

#include "mediator/map/MapInClusterViewMediator.h"
#include "mediator/map/MapInClusterSideViewMediator.h"
#include "mediator/map/MapInClusterHudViewMediator.h"

#include "LangConstants.h"
#include "uilayout.h"
//#include "LayoutConstants.h"
#include "WidgetConstants.h"
#include "ApplicationConstants.h"
#include "TestConstants.h"
#include "RgConstants.h"
#include "mediator/map/MapConfigDebugUiControl.h"

#include "PlatformHelper.h"
#include "MultiSenseManager.h"
#include "MultiSenseConstants.h"
#include "QmlAnyNetworkAccessManagerFactory.h"
#include "PreciseTimer.h"
#include "QmlMouseEventListener.h"

using namespace SKT;

namespace SKT {

class QmlAppHelperPrivate {
public:
    QmlAppHelperPrivate() {
        mTouchDevice = new QTouchDevice;
        mTouchDevice->setType(QTouchDevice::TouchScreen);
    }

    ~QmlAppHelperPrivate() {
        delete mTouchDevice;
    }

    QQmlApplicationEngine* mQmlEngine = nullptr;
    QObject* mAppView = nullptr;
    QObject* mGpsService = nullptr;
    QObject* mSimulator = nullptr;
    QObject* mController = nullptr;
    QObject* mAddressManager = nullptr;
    QObject* mUpdateManager = nullptr;
    QString mPrefDirPath;
    QObject* mNavigationProperty = nullptr;
    QString mResDirPath;
    TranslatorManager mTranslatorMngr;
    LayoutManager mLayoutMngr;
    bool mHasMapData = false;
    QQueue<QMap<int,QVariant>> mTestEventQueue;
    QWindow* mTestWindow = nullptr;
    QObject* mTestQuickItem = nullptr;
    bool mTestEventQueueProccessing = false;
    MultiSenseManager mMultiSenseManager;
    QmlAnyNetworkAccessManagerFactory mNamFactory;
    QChar mLanguageIndex;
    int mMexMode;
    int mColorState;
    QTouchDevice* mTouchDevice;
    QVariantMap mGadgetSignal;
};

}

QmlAppHelper::QmlAppHelper()
    : mPrivate( new QmlAppHelperPrivate )
{
}

QmlAppHelper::~QmlAppHelper()
{
}

QmlAppHelper* QmlAppHelper::instance()
{
    static QmlAppHelper Instance;

    return &Instance;
}

void QmlAppHelper::initialize( QQmlApplicationEngine* qmlAppEngine, const QVariantMap& context )
{
    auto layout = context["layout"].toInt();
    auto resDirPath = context["resDirPath"].toString();
    auto dbDirPath = context["dbDirPath"].toString();
    auto crossImagePath = context["crossImagePath"].toString();
    auto gpsService = context["gpsService"].value<QObject*>();
    auto simulator = context["simulator"].value<QObject*>();
    auto rpnSearch = context["rpnSearch"].value<QObject*>();
    auto addressManager = context["addressManager"].value<QObject*>();
    auto searchManager = context["searchManager"].value<QObject*>();
    auto controller = context["controller"].value<QObject*>();
    auto navigationProperty = context["navigationProperty"].value<QObject*>();
//    auto markerManager = context["markerManager"].value<QObject*>();
    auto prefDirPath = context["preferenceDirPath"].toString();
    auto updateManager = context["updateManager"].value<QObject*>();    
     auto isEngineInit = context["isEngineInit"].toBool();
    auto hasMapData = isEngineInit;
    auto languageIndex = context["languageIndex"].toChar();
    auto gadgetSignal = context["gadgetSignal"].toMap();
    auto colorState = context["colorState"].toInt();
    auto mexMode = context["mexMode"].toInt();

    mPrivate->mQmlEngine = qmlAppEngine;
    mPrivate->mResDirPath = resDirPath;
    mPrivate->mGpsService = gpsService;
    mPrivate->mSimulator = simulator;
    mPrivate->mController = controller;
    mPrivate->mPrefDirPath = prefDirPath;
    mPrivate->mAddressManager = addressManager;
    mPrivate->mNavigationProperty = navigationProperty;
    mPrivate->mUpdateManager = updateManager;
    mPrivate->mHasMapData = hasMapData;
    mPrivate->mLanguageIndex = languageIndex;
    mPrivate->mGadgetSignal = gadgetSignal;
    mPrivate->mColorState = colorState;
    mPrivate->mMexMode = mexMode;

    mPrivate->mLayoutMngr.setDefaultLayout( UILayout::L800x480 );
    mPrivate->mLayoutMngr.setLayout( layout );
    ResourceResolver::instance()->initialize( mPrivate->mResDirPath );

    mPrivate->mQmlEngine->setNetworkAccessManagerFactory(&mPrivate->mNamFactory);

    // initialize fonts
    auto fontPath = mPrivate->mResDirPath + "/NotoSansMonoCJKkr-Regular.otf";   // use free license font of Google
    int id = QFontDatabase::addApplicationFont( fontPath );
    fontPath = mPrivate->mResDirPath + "/ReadCondensedJK-Regular_V2000.ttf";
    id = QFontDatabase::addApplicationFont( fontPath );
    if (id != -1) {        
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        T_DEBUG( QString( "loaded '%1'(%2)" ).arg( fontPath ).arg( family ) );
        qGuiApp->setFont(QFont(family));
    }

    registerType();
    registerQmlType();

    auto appPath = QCoreApplication::applicationDirPath();

    auto ctx = mPrivate->mQmlEngine->rootContext();
    ctx->setContextProperty( "QmlAppHelper", this );
    ctx->setContextProperty( "console", this );
    ctx->setContextProperty( "Tr", this );
    ctx->setContextProperty( "Resource", ResourceResolver::instance() );
    ctx->setContextProperty( "MultiSenseManager", &mPrivate->mMultiSenseManager );

    mPrivate->mQmlEngine->addImageProvider( "ImageProvider", new ImageProvider );

#ifdef QT_DEBUG
    mPrivate->mQmlEngine->clearComponentCache();
    mPrivate->mQmlEngine->trimComponentCache();
#endif

qDebug() << "tmap ui qml initialized ^^^^^^^^^^^";
    mPrivate->mQmlEngine->load(QUrl(mPrivate->mLayoutMngr.mainWindowPath()));
}

void QmlAppHelper::startup( QObject* appView )
{    
    mPrivate->mAppView = appView;

    QVariantMap body;    
    body.insert( "qmlEngine", QVariant::fromValue(mPrivate->mQmlEngine) );
    body.insert( "resDirPath", QVariant::fromValue(mPrivate->mResDirPath) );
    body.insert( "appView", QVariant::fromValue(mPrivate->mAppView) );
    body.insert( "gpsSerivce", QVariant::fromValue(mPrivate->mGpsService) );
    body.insert( "simulator", QVariant::fromValue(mPrivate->mSimulator));
    body.insert( "controller", QVariant::fromValue(mPrivate->mController));
    body.insert( "addressManager", QVariant::fromValue(mPrivate->mAddressManager));
    body.insert( "preferenceDirPath", QVariant::fromValue(mPrivate->mPrefDirPath));
    body.insert( "navigationProperty", QVariant::fromValue(mPrivate->mNavigationProperty));
    body.insert( "updateManager", QVariant::fromValue(mPrivate->mUpdateManager));
    body.insert( "hasMapData", QVariant::fromValue(mPrivate->mHasMapData));
    body.insert( "languageIndex", QVariant::fromValue(mPrivate->mLanguageIndex));
    body.insert( "gadgetSignal", QVariant::fromValue(mPrivate->mGadgetSignal));
    body.insert( "colorState", QVariant::fromValue(mPrivate->mColorState));
    body.insert( "mexMode", QVariant::fromValue(mPrivate->mMexMode));

    // set lang
    mPrivate->mTranslatorMngr.setQmPath( mPrivate->mResDirPath + QStringLiteral("/lang") );
    mPrivate->mTranslatorMngr.addLang( LangConstants::Ko );
    mPrivate->mTranslatorMngr.addLang( LangConstants::En );
    mPrivate->mTranslatorMngr.setLang( LangConstants::Ko );
qDebug() << "tmap ui startup command ^^^^^^^^^6";
    Facade::instance()->sendNotification( this, Constants::Startup, QVariant::fromValue(body), QString() );

    // test
    QWindowList windows = QGuiApplication::allWindows();
    mPrivate->mTestWindow = windows.first();
    mPrivate->mTestQuickItem = qobject_cast<QQuickWindow*>( mPrivate->mAppView )->contentItem();

    // multisense
    mPrivate->mMultiSenseManager.setColorState( mPrivate->mColorState );// MultiSenseManager::Echo );
    connect( &mPrivate->mMultiSenseManager, &MultiSenseManager::colorStateChanged, [&](){
        Facade::instance()->sendNotification( this, MultiSenseConstants::ChangedColorState, (int)mPrivate->mMultiSenseManager.colorState() );
    });

    if (mPrivate->mHasMapData == false) {
        // 시스템에서 내비 화면으로 전환 시 map이 한번이라도 그려져야 전환함.
        // NoMap일 때 맵이 렌더링 되지 않으므로 한번 호출해 준다.
        // 19.07.23 ymhong

        qDebug() << "[CTX]QmlAppHelper::startup onMapStartedToRender onFrameSwapped";
        QMetaObject::invokeMethod(mPrivate->mController, "onMapStartedToRender");
        QMetaObject::invokeMethod(mPrivate->mController, "onFrameSwapped");
    }
}

QString QmlAppHelper::viewUrl( int viewId )
{
    return ViewRegistry::url( viewId );
}

#ifdef QT_DEBUG

#include <QtQml>
#include <private/qv8engine_p.h>
#include <private/qv4engine_p.h>
void QmlAppHelper::log( const QString& msg )
{
    qDebug(ui).noquote() << msg;
}

void QmlAppHelper::debug( const QString& msg )
{
#if (QT_VERSION >= 0x50b01)
    const QV4::CppStackFrame * frame = QV8Engine::getV4(mPrivate->mQmlEngine)->currentStackFrame;
    SKT::DebugLogger::instance()->write( frame->source(), frame->lineNumber(), frame->function(), msg, QStringLiteral("[UI]") );
#else
    const QV4::StackFrame frame = QV8Engine::getV4(mPrivate->mQmlEngine)->currentStackFrame();
    SKT::DebugLogger::instance()->write( frame.source, frame.line, frame.function, msg, QStringLiteral("[UI]") );
#endif
}

void QmlAppHelper::info( const QString& msg )
{
    qInfo(ui).noquote() << msg;
}

void QmlAppHelper::warn( const QString& msg )
{
    qWarning(ui).noquote() << msg;
}

void QmlAppHelper::error( const QString& msg )
{
    qCritical(ui).noquote() << msg;
}

void QmlAppHelper::perf( const QString& msg )
{
    T_PERF_LOG() << msg;
}

#else

void QmlAppHelper::log( const QString& msg )
{
    qDebug(ui).noquote() << msg;
}

void QmlAppHelper::info( const QString& msg )
{
    qInfo(ui).noquote() << msg;
}

void QmlAppHelper::debug( const QString& msg )
{
    qDebug(ui).noquote() << msg;
}

void QmlAppHelper::warn( const QString& msg )
{
    qWarning(ui).noquote() << msg;
}

void QmlAppHelper::error( const QString& msg )
{
    qCritical(ui).noquote() << msg;
}

void QmlAppHelper::perf( const QString& msg )
{
    T_PERF_LOG() << msg;
}

#endif

//void QmlAppHelper::singleShot( int msec, QObject* receiver, const QString& slot )
//{
//    auto cb = [&,receiver,slot]{
//        QMetaObject::invokeMethod( receiver, slot.toLatin1() );
//    };

//    QTimer::singleShot( msec, cb );
//}

bool QmlAppHelper::hitTest( const QPointF& posOnAppLocal, QObject* item, const QRectF& hitRectOnItemLocal )
{
    bool ret = false;

    if ( item == nullptr ) {
        return ret;
    }

    // TODO 5.7 이후 mapToGlobal로 대체

    // 윈도우 좌표(posOnAppLocal)를 item 기준 좌표(posOnItemLocal)로 변환한다

    auto qRootItem = qobject_cast<QQuickWindow*>( mPrivate->mAppView )->contentItem();
    auto qItem = qobject_cast<QQuickItem*>( item );

    auto posOnItemLocal = qRootItem->mapToItem( qItem, posOnAppLocal );

//    T_DEBUGV( "appPos(%.f,%.f) itemPos(%.f,%.f)",
//        posOnAppLocal.x(), posOnAppLocal.y(),
//        posOnItemLocal.x(), posOnItemLocal.y() );

    if ( hitRectOnItemLocal.isNull() ) {
        ret = qItem->contains( posOnItemLocal );
    }
    else {
        ret = hitRectOnItemLocal.contains( posOnItemLocal );
    }

    return ret;
}

namespace {

class TestAction {
public:
    enum E {
        MousePress = TestConstants::MouseActionPress,
        MouseRelease = TestConstants::MouseActionRelease,
        MouseClick = TestConstants::MouseActionClick,
        MouseDoubleClick = TestConstants::MouseActionDoubleClick,
        MouseMove = TestConstants::MouseActionMove,
        TouchPress = TestConstants::TouchActionPress,
        TouchRelease = TestConstants::TouchActionRelease,
        TouchMove = TestConstants::TouchActionMove
    };

    static bool isMouse(E action) {
        return action == MousePress || action == MouseRelease || action == MouseClick
            || action == MouseDoubleClick || action == MouseMove;
    }

    static QString toString(E action)
    {
        QString ret;

        switch (action)
        {
            case MousePress:
                ret = QStringLiteral("MousePress");
                break;
            case MouseRelease:
                ret = QStringLiteral("MouseRelease");
                break;
            case MouseDoubleClick:
                ret = QStringLiteral("MouseDoubleClick");
                break;
            case MouseMove:
                ret = QStringLiteral("MouseMove");
                break;
            case TouchPress:
                ret = QStringLiteral("TouchPress");
                break;
            case TouchRelease:
                ret = QStringLiteral("TouchRelease");
                break;
            case TouchMove:
                ret = QStringLiteral("TouchMove");
            default:
                break;
        }

        return ret;
    }
};


// test
enum TestEventArgument {
    Action,
    Window,
    Item,
    Button,
    StateKey,
    Point,
    Delay
};

void notifyTestEvent(QmlAppHelperPrivate* appPrivate, const QMap<int,QVariant>& args)
{
    //T_DEBUGV( "[TouchTest]notifyTestEvent #1" );

    TestAction::E action = (TestAction::E)args[Action].toInt();
    QWindow* window = dynamic_cast<QWindow*>(args[Window].value<QObject*>());
    QObject* item = args[Item].value<QObject*>();
    Qt::MouseButton mb = (Qt::MouseButton)args[Button].toInt();
    Qt::KeyboardModifier km = (Qt::KeyboardModifier)args[StateKey].toInt();
    int delay = args[Delay].toInt();

    if (action == TestAction::MouseClick) {
        auto tempArgs = args;
        tempArgs[Action] = (int)TestAction::MousePress;
        notifyTestEvent(appPrivate, tempArgs);
        tempArgs[Action] = (int)TestAction::MouseRelease;
        notifyTestEvent(appPrivate, tempArgs);
        return;
    }

    km = (Qt::KeyboardModifier)(static_cast<unsigned int>(km) & Qt::KeyboardModifierMask);

    if (TestAction::isMouse(action)) {
        QPointF pos = args[Point].toPointF();
        QPoint sPos;
        auto qItem = qobject_cast<QQuickItem *>(item);
        if (qItem) {
            sPos = qItem->mapToScene(pos).toPoint();
        }
        QMouseEvent event(QEvent::User, QPoint(), Qt::LeftButton, mb, km);
        switch (action)
        {
            case TestAction::MousePress:
                event = QMouseEvent(QEvent::MouseButtonPress, pos, window->mapToGlobal(sPos), mb, mb, km);
                break;
            case TestAction::MouseRelease:
                event = QMouseEvent(QEvent::MouseButtonRelease, pos, window->mapToGlobal(sPos), mb, 0, km);
                break;
            case TestAction::MouseDoubleClick:
                event = QMouseEvent(QEvent::MouseButtonDblClick, pos, window->mapToGlobal(sPos), mb, mb, km);
                break;
            case TestAction::MouseMove:
                event = QMouseEvent(QEvent::MouseMove, pos, window->mapToGlobal(sPos), Qt::NoButton, mb, km);
                break;
            default:
                break;
        }

        if (!qApp->notify(window, &event)) {
            T_DEBUG( QString("Failed to notify %1").arg(TestAction::toString(action)) );
        }

        QMetaObject::invokeMethod( appPrivate->mAppView, "showTestClickPoint",
            Q_ARG(QVariant,pos.x()), Q_ARG(QVariant,pos.y()), Q_ARG(QVariant,(int)action) );
    }
    else {
        //T_DEBUGV( "[TouchTest]notifyTestEvent #2" );

        QList<QTouchEvent::TouchPoint> touchPoints;

        auto points = args[Point].value<QList<QPointF>>();
        for (int i = 0; i < points.length(); i++) {
            auto point = points[i];
            QPoint sPos;
            auto qItem = qobject_cast<QQuickItem *>(item);
            if (qItem) {
                sPos = qItem->mapToScene(point).toPoint();
            }

            QTouchEvent::TouchPoint touchPoint;
            touchPoint.setState(Qt::TouchPointMoved);
            switch (action) {
            case TestAction::TouchPress:
                //T_DEBUGV( "[TouchTest]notifyTestEvent TouchPointPressed" );
                touchPoint.setState(Qt::TouchPointPressed);
                break;
            case TestAction::TouchRelease:
                T_DEBUGV( "[TouchTest]notifyTestEvent TouchPointReleased" );
                //touchPoint.setState(Qt::TouchPointReleased);
                break;
            default:
                break;
            }

            touchPoint.setId(i);
            auto gPos = window->mapToGlobal(sPos);
            touchPoint.setScreenPos(gPos);
            touchPoint.setPos(point);
            touchPoint.setPressure(1);

            touchPoints.append(touchPoint);

            //T_DEBUGV( "[TouchTest][%d] x: %d, y: %d", i, gPos.x(), gPos.y() );

            QMetaObject::invokeMethod( appPrivate->mAppView, "showTestTouchPoint",
                Q_ARG(QVariant,i), Q_ARG(QVariant,point.x()), Q_ARG(QVariant,point.y()), Q_ARG(QVariant,(int)action) );
        }

        QEvent::Type type = QEvent::TouchUpdate;
        QString typeStr;
        if (touchPoints.size() > 0) {
            int state = (int)touchPoints[0].state();

            if (touchPoints[0].state() == Qt::TouchPointPressed) {
                type = QEvent::TouchBegin;
                typeStr = "TouchBegin";
            }
            else if (touchPoints[0].state() == Qt::TouchPointReleased) {
                type = QEvent::TouchEnd;
                typeStr = "TouchEnd";
            }
        }

        QTouchEvent touchEv(type);
        touchEv.setDevice(appPrivate->mTouchDevice);
        touchEv.setTouchPoints(touchPoints);
        QCoreApplication::sendEvent(window, &touchEv);
        //       if (!qApp->notify(window, touchEv)) {
        //           T_DEBUG( QString("Failed to notify %1").arg(mouseActionName(action)) );
        //       }        

    }
}

}

// 18.3.8 hskim the window value can be vary when we have to manage multiple windows

void QmlAppHelper::testMouseClick(Qt::MouseButton mb, Qt::KeyboardModifiers km, const QPointF& pos, int delay)
{
    QMap<int,QVariant> event;
    event[Action] = TestAction::MouseClick;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Button] = (int)mb;
    event[StateKey] = (int)km;
    event[Point] = pos;
    event[Delay] = delay;

    testAddEvent( event );
}

void QmlAppHelper::testMousePress(Qt::MouseButton mb, Qt::KeyboardModifiers km, const QPointF& pos, int delay)
{

    QMap<int,QVariant> event;
    event[Action] = TestAction::MousePress;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Button] = (int)mb;
    event[StateKey] = (int)km;
    event[Point] = pos;
    event[Delay] = delay;

    testAddEvent( event );
}

void QmlAppHelper::testMouseRelease(Qt::MouseButton mb, Qt::KeyboardModifiers km, const QPointF& pos, int delay)
{

    QMap<int,QVariant> event;
    event[Action] = TestAction::MouseRelease;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Button] = (int)mb;
    event[StateKey] = (int)km;
    event[Point] = pos;
    event[Delay] = delay;

    testAddEvent( event );
}

void QmlAppHelper::testMouseMove(const QPointF& pos, int delay)
{
    QMap<int,QVariant> event;
    event[Action] = TestAction::MouseMove;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Button] = (int)Qt::NoButton;
    event[StateKey] = (int)Qt::KeyboardModifiers();
    event[Point] = pos;
    event[Delay] = delay;

    testAddEvent( event );
}

void QmlAppHelper::testTouchPress(const QList<QPointF>& points, int delay)
{
    //T_DEBUGV( "[TouchTest]testTouchPress" );

    QMap<int,QVariant> event;
    event[Action] = TestAction::TouchPress;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Delay] = delay;
    event[Point] = QVariant::fromValue(points);

    testAddEvent( event );
}

void QmlAppHelper::testTouchRelease(const QList<QPointF>& points, int delay)
{
    //T_DEBUGV( "[TouchTest]testTouchRelease" );

    QMap<int,QVariant> event;
    event[Action] = TestAction::TouchRelease;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Delay] = delay;
    event[Point] = QVariant::fromValue(points);

    testAddEvent( event );
}

void QmlAppHelper::testTouchMove(const QList<QPointF>& points, int delay)
{
    //T_DEBUGV( "[TouchTest]testTouchMove" );

    QMap<int,QVariant> event;
    event[Action] = TestAction::TouchMove;
    event[Window] = QVariant::fromValue( mPrivate->mTestWindow );
    event[Item] = QVariant::fromValue( mPrivate->mTestQuickItem );
    event[Delay] = delay;
    event[Point] = QVariant::fromValue(points);

    testAddEvent( event );
}

void QmlAppHelper::testDoNextEvent( bool cont )
{
    bool doNext = false;

    if ( !cont ) {
        if ( !mPrivate->mTestEventQueueProccessing && !mPrivate->mTestEventQueue.isEmpty() ) {
            doNext = true;
        }
    }
    else {
        if ( !mPrivate->mTestEventQueue.isEmpty() ) {
            doNext = true;
        }
    }

    //T_DEBUGV( "[TouchTest]testDoNextEvent #1 cont: %d doNext: %d", cont, doNext );

    if ( doNext ) {
        //T_DEBUGV( "[TouchTest]testDoNextEvent #2" );

        mPrivate->mTestEventQueueProccessing = true;

        auto args = mPrivate->mTestEventQueue.dequeue();
        TestAction::E action = (TestAction::E)args[Action].toInt();
        QWindow* window = dynamic_cast<QWindow*>(args[Window].value<QObject*>());
        QObject* item = args[Item].value<QObject*>();
        Qt::MouseButton mb = (Qt::MouseButton)args[Button].toInt();
        Qt::KeyboardModifier km = (Qt::KeyboardModifier)args[StateKey].toInt();
        QPointF pos = args[Point].toPointF();
        int delay = args[Delay].toInt();

        QTimer::singleShot(delay, this, [this, args, action, pos](){            
            //T_DEBUGV( "[TouchTest]testDoNextEvent #3" );

            notifyTestEvent( mPrivate.data(), args );

            if ( mPrivate->mTestEventQueue.isEmpty() ) {
                mPrivate->mTestEventQueueProccessing = false;
            }
            else {
                QMetaObject::invokeMethod( this, "testDoNextEvent", Qt::QueuedConnection, Q_ARG(bool,true) );
            }
        });
    }
}

void QmlAppHelper::testAddEvent( const QMap<int,QVariant>& event )
{
    mPrivate->mTestEventQueue.enqueue( event );

    testDoNextEvent( false );
}

QString QmlAppHelper::language()
{
    return mPrivate->mTranslatorMngr.lang();
}

void QmlAppHelper::setLanguage( const QString& lang )
{
    mPrivate->mTranslatorMngr.setLang(lang);

    emit languageChanged();
}

QString QmlAppHelper::d()
{
    return "";
}

int QmlAppHelper::layout()
{
    return mPrivate->mLayoutMngr.layout();
}

QString QmlAppHelper::layoutName()
{
    return mPrivate->mLayoutMngr.layoutName();
}

QString QmlAppHelper::layoutResDir()
{
    return mPrivate->mLayoutMngr.layoutResDir( mPrivate->mLayoutMngr.layout() );
}

QString QmlAppHelper::defaultLayoutResDir()
{
    return mPrivate->mLayoutMngr.layoutResDir( mPrivate->mLayoutMngr.defaultLayout() );
}

QString QmlAppHelper::layoutGlobalConstantsJs()
{
    return mPrivate->mLayoutMngr.layoutGlobalConstantsJs( mPrivate->mLayoutMngr.layout() );
}

void QmlAppHelper::setContextProperty( const QString& name, QObject* obj )
{
    auto ctx = mPrivate->mQmlEngine->rootContext();
    ctx->setContextProperty( name, obj );
}

int QmlAppHelper::multiSenseColorState()
{
    return mPrivate->mMultiSenseManager.colorState();
}

QColor QmlAppHelper::multiSenseColor()
{
    auto index = multiSenseColorState();
    auto color = mPrivate->mMultiSenseManager.color(index);

    return color;
}

void QmlAppHelper::setMultiSenseColorState(int colorState)
{
    mPrivate->mMultiSenseManager.setColorState( colorState );
}

void QmlAppHelper::setMultiSenseColorStateFromHiddenMenu(int color)
{
    mPrivate->mMultiSenseManager.setColorStateFromHiddenMenu( color );
}

bool QmlAppHelper::isDebug()
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif
}

bool QmlAppHelper::isTarget()
{
#ifdef BUILD_TARGET
    return true;
#else
    return false;
#endif
}

void QmlAppHelper::registerType()
{
    qRegisterMetaType<MapViewMediator*>();
    qRegisterMetaType<RecentTileGridViewMediator*>();
    qRegisterMetaType<TestViewMediator*>();

    qRegisterMetaType<TbtViewMediator*>();
    qRegisterMetaType<LaneViewMediator*>();
    qRegisterMetaType<SdiViewMediator*>();    
    qRegisterMetaType<CrossImageViewMediator*>();
    qRegisterMetaType<HiPassLaneViewMediator*>();
    qRegisterMetaType<HighwayTbtViewMediator*>();
    qRegisterMetaType<BusLaneViewMediator*>();
    qRegisterMetaType<TrafficBarViewMediator*>();
    qRegisterMetaType<ChangeDestinationViewMediator*>();

    qRegisterMetaType<SettingCarIconViewMediator*>();
    qRegisterMetaType<SettingGpsInfoViewMediator*>();
    qRegisterMetaType<SettingGuidViewMediator*>();
    qRegisterMetaType<SettingGuidVoiceViewMediator*>();    
    qRegisterMetaType<SettingMainViewMediator*>();
    qRegisterMetaType<SettingMapCharSizeViewMediator*>();
    qRegisterMetaType<SettingMapViewMediator*>();
    qRegisterMetaType<SettingMapUpdateViewMediator*>();
    qRegisterMetaType<SettingNaviInfoViewMediator*>();
    qRegisterMetaType<SettingMobileTmapViewMediator*>();
    qRegisterMetaType<SettingAttentionSectionViewMediator*>();
    qRegisterMetaType<SettingSpeedResponseMapViewMediator*>();
    qRegisterMetaType<SettingCameraWarningSoundViewMediator*>();    
    qRegisterMetaType<SettingRouteSearchViewMediator*>();
    qRegisterMetaType<SettingRouteAlternateViewMediator*>();
    qRegisterMetaType<SelfTestViewMediator*>();

    // hidden setting menu
    qRegisterMetaType<HiddenDiagSettingViewMediator*>();
    qRegisterMetaType<HiddenVsmDiagSettingViewMediator*>();
    qRegisterMetaType<HiddenMenuLoginViewMediator*>();
    qRegisterMetaType<HiddenSettingFontDpViewMediator*>();	

	qRegisterMetaType<BookmarkViewMediator*>();
    qRegisterMetaType<BookmarkPageViewMediator*>();
    qRegisterMetaType<RecentDestinationViewMediator*>();
    qRegisterMetaType<RecentDestinationPageViewMediator*>();
    qRegisterMetaType<RecentDestinationEditViewMediator*>();
    qRegisterMetaType<RecentSearchEditViewMediator*>();
    qRegisterMetaType<RecentSearchPageViewMediator*>();
    qRegisterMetaType<NoResultPageViewMediator*>();
    qRegisterMetaType<SearchResultDetailViewMediator*>();
    qRegisterMetaType<SearchResultViewMediator*>();
    qRegisterMetaType<SearchTabViewMediator*>();
    qRegisterMetaType<TextSearchViewMediator*>();
    qRegisterMetaType<IndustrySearchViewMediator*>(); 
    qRegisterMetaType<SearchResultOptionViewMediator*>();
    qRegisterMetaType<MapModeOptionViewMediator*>();
    qRegisterMetaType<MapControlViewMediator*>();
    qRegisterMetaType<SearchAroundViewMediator*>();
    qRegisterMetaType<NaviMenuViewMediator*>();
    qRegisterMetaType<RouteCandidateViewMediator*>();
    qRegisterMetaType<RouteSummaryViewMediator*>();
    qRegisterMetaType<RouteSettingViewMediator*>();
    qRegisterMetaType<RouteRegistViewMediator*>();
    qRegisterMetaType<MapSearchViewMediator*>();
    qRegisterMetaType<SimulationViewMediator*>();
    qRegisterMetaType<ErrorViewMediator*>();
    qRegisterMetaType<AddressSearchViewMediator*>();
    qRegisterMetaType<RegionSearchViewMediator*>();
    qRegisterMetaType<PoiTabViewMediator*>();

    qRegisterMetaType<KeyboardViewMediator*>();
    qRegisterMetaType<HardKeyViewMediator*>();
    qRegisterMetaType<Zone1ViewMediator*>();
    qRegisterMetaType<Zone3ViewMediator*>();
    qRegisterMetaType<ZoneApiTestViewMediator*>();
    qRegisterMetaType<BookmarkRegistViewMediator*>();
    qRegisterMetaType<OptionMenuViewMediator*>();

    qRegisterMetaType<FullscreenAlertViewMediator*>();

    qRegisterMetaType<SummaryTbtViewMediator*>();
    qRegisterMetaType<SpeedMeterViewMediator*>();

    qRegisterMetaType<WidgetSmallViewMediator*>();
    qRegisterMetaType<WidgetMediumViewMediator*>();
    qRegisterMetaType<WidgetLargeViewMediator*>();
    qRegisterMetaType<WidgetXlViewMediator*>();
    qRegisterMetaType<WidgetXxlViewMediator*>();

    qRegisterMetaType<MapInClusterViewMediator*>();
    qRegisterMetaType<MapInClusterSideViewMediator*>();
	qRegisterMetaType<MapInClusterHudViewMediator*>();
}

void QmlAppHelper::registerQmlType()
{
    // register for exchanging data with qml

    //qmlRegisterType<RgDataVo>("RgDataVo", 1, 0, "RgDataVo");

    // register for sharing enums with qml
    qmlRegisterUncreatableType<Constants>("Constants", 1, 0, "Constants", "");    
    qmlRegisterUncreatableType<RgSignal>("RgSignal", 1, 0, "RgSignal", "");
    qmlRegisterUncreatableType<RgSignalState>("RgSignalState", 1, 0, "RgSignalState", "");
    qmlRegisterUncreatableType<RgStatusCode>("RgStatusCode", 1, 0, "RgStatusCode", "");
    qmlRegisterUncreatableType<RgConstants>("RgConstants", 1, 0, "RgConstants", "");
    qmlRegisterUncreatableType<MapViewMode>("MapViewMode", 1, 0, "MapViewMode", "");
    qmlRegisterUncreatableType<MapViewMediator>("MapViewEvent", 1, 0, "MapViewEvent", "");
    qmlRegisterUncreatableType<RouteCandidateViewMediator>("RouteCandidateViewEvent", 1, 0, "RouteCandidateViewEvent", "");
    qmlRegisterUncreatableType<RouteSummaryViewMediator>("RouteSummaryViewEvent", 1, 0, "RouteSummaryViewEvent", "");
    qmlRegisterUncreatableType<RouteSettingViewMediator>("RouteSettingViewEvent", 1, 0, "RouteSettingViewEvent", "");
    qmlRegisterUncreatableType<RouteRegistViewMediator>("RouteRegistViewEvent", 1, 0, "RouteRegistViewEvent", "");
    qmlRegisterUncreatableType<ErrorViewMediator>("ErrorViewEvent", 1, 0, "ErrorViewEvent", "");
    qmlRegisterUncreatableType<RecentTileGridViewMediator>("RecentTileGridViewEvent", 1, 0, "RecentTileGridViewEvent", "");
    qmlRegisterUncreatableType<SdiViewMediator>("SdiViewEvent", 1, 0, "SdiViewEvent", "");
    qmlRegisterUncreatableType<SdiViewMediator>("SignType", 1, 0, "SignType", "");
    qmlRegisterUncreatableType<SimulationViewMediator>("SimulationViewEvent", 1, 0, "SimulationViewEvent", "");
    qmlRegisterUncreatableType<AddressSearchViewMediator>("AddressSearchViewEvent", 1, 0, "AddressSearchViewEvent", "");
    qmlRegisterUncreatableType<RegionSearchViewMediator>("RegionSearchViewEvent", 1, 0, "RegionSearchViewEvent", "");
    qmlRegisterUncreatableType<ApplicationMediator>("ApplicationEvent", 1, 0, "ApplicationEvent", "");        
    qmlRegisterUncreatableType<MapControlViewMediator>("MapControlViewEvent", 1, 0, "MapControlViewEvent", "");
    qmlRegisterUncreatableType<MapSearchViewMediator>("MapSearchViewEvent", 1, 0, "MapSearchViewEvent", "");
    qmlRegisterUncreatableType<Zone1ViewMediator>("Zone1ViewEvent", 1, 0, "Zone1ViewEvent", "");
    qmlRegisterUncreatableType<Zone3ViewMediator>("Zone3ViewEvent", 1, 0, "Zone3ViewEvent", "");
    qmlRegisterUncreatableType<ZoneApiTestViewMediator>("ZoneApiTestEvent", 1, 0, "ZoneApiTestEvent", "");
    qmlRegisterUncreatableType<OptionMenuViewMediator>("OptionMenuViewEvent", 1, 0, "OptionMenuViewEvent", "");
    qmlRegisterUncreatableType<TbtViewMediator>("TbtViewEvent", 1, 0, "TbtViewEvent", "");
    qmlRegisterUncreatableType<SdiViewMediator>("SdiViewEvent", 1, 0, "SdiViewEvent", "");
    qmlRegisterUncreatableType<LaneViewMediator>("LaneViewEvent", 1, 0, "LaneViewEvent", "");
    qmlRegisterUncreatableType<BusLaneViewMediator>("BusLaneViewEvent", 1, 0, "BusLaneViewEvent", "");
    qmlRegisterUncreatableType<TrafficBarViewMediator>("TrafficBarViewEvent", 1, 0, "TrafficBarViewEvent", "");
    qmlRegisterUncreatableType<TrafficBarViewMediator>("TrafficBarViewType", 1, 0, "TrafficBarViewType", "");
    qmlRegisterType           <TrafficBarGraph>("QuickTrafficBarGraph", 1, 0, "TrafficBarGraph");
    qmlRegisterUncreatableType<CrossImageViewMediator>("CrossImageViewEvent", 1, 0, "CrossImageViewEvent", "");
    qmlRegisterUncreatableType<TestViewMediator>("TestViewEvent", 1, 0, "TestViewEvent", "");
    qmlRegisterUncreatableType<TestMultiMapViewMediator>("TestMultiMapViewEvent", 1, 0, "TestMultiMapViewEvent", "");
    qmlRegisterUncreatableType<HighwayTbtViewMediator>("HighwayTbtViewEvent", 1, 0, "HighwayTbtViewEvent", "");
    qmlRegisterUncreatableType<SummaryTbtViewMediator>("SummaryTbtViewEvent", 1, 0, "SummaryTbtViewEvent", "");
    qmlRegisterUncreatableType<ProgressIndicatorViewMediator>("ProgressIndicatorViewEvent", 1, 0, "ProgressIndicatorViewEvent", "");

    qmlRegisterUncreatableType<SettingCarIconViewMediator>             ("SettingCarIconViewEvent",              1, 0,  "SettingCarIconViewEvent",            "");
    qmlRegisterUncreatableType<SettingGpsInfoViewMediator>             ("SettingGpsInfoViewEvent",              1, 0,  "SettingGpsInfoViewEvent",            "");
    qmlRegisterUncreatableType<SettingGuidViewMediator>                ("SettingGuidViewEvent",                 1, 0,  "SettingGuidViewEvent",               "");
    qmlRegisterUncreatableType<SettingGuidVoiceViewMediator>           ("SettingGuidVoiceViewEvent",            1, 0,  "SettingGuidVoiceViewEvent",          "");    
    qmlRegisterUncreatableType<SettingMainViewMediator>                ("SettingMainViewEvent",                 1, 0,  "SettingMainViewEvent",               "");
    qmlRegisterUncreatableType<SettingMapCharSizeViewMediator>         ("SettingMapCharSizeViewEvent",          1, 0,  "SettingMapCharSizeViewEvent",        "");
    qmlRegisterUncreatableType<SettingMapViewMediator>                 ("SettingMapViewEvent",                  1, 0,  "SettingMapViewEvent",                "");
    qmlRegisterUncreatableType<SettingMapUpdateViewMediator>           ("SettingMapUpdateViewEvent",            1, 0,  "SettingMapUpdateViewEvent",          "");
    qmlRegisterUncreatableType<SettingNaviInfoViewMediator>            ("SettingNaviInfoViewEvent",             1, 0,  "SettingNaviInfoViewEvent",           "");
    qmlRegisterUncreatableType<SettingMobileTmapViewMediator>          ("SettingMobileTmapViewEvent",           1, 0,  "SettingMobileTmapViewEvent",         "");
    qmlRegisterUncreatableType<SettingAttentionSectionViewMediator>    ("SettingAttentionSectionViewEvent",     1, 0,  "SettingAttentionSectionViewEvent",   "");
    qmlRegisterUncreatableType<SettingSpeedResponseMapViewMediator>    ("SettingSpeedResponseMapViewEvent",     1, 0,  "SettingSpeedResponseMapViewEvent",   "");
    qmlRegisterUncreatableType<SettingCameraWarningSoundViewMediator>  ("SettingCameraWarningSoundViewEvent",   1, 0,  "SettingCameraWarningSoundViewEvent", "");
    qmlRegisterUncreatableType<SettingRouteSearchViewMediator>         ("SettingRouteSearchViewEvent",          1, 0,  "SettingRouteSearchViewEvent",        "");
    qmlRegisterUncreatableType<SettingRouteAlternateViewMediator>      ("SettingRouteAlternateViewEvent",       1, 0,  "SettingRouteAlternateViewEvent",     "");
    qmlRegisterUncreatableType<SettingVoiceGuidTimeViewMediator>       ("SettingVoiceGuidTimeViewEvent",        1, 0,  "SettingVoiceGuidTimeViewEvent",      "");
    qmlRegisterUncreatableType<SettingOilKindViewMediator>             ("SettingOilKindViewEvent",              1, 0,  "SettingOilKindViewEvent",            "");
    qmlRegisterUncreatableType<SettingOilBrandViewMediator>            ("SettingOilBrandViewEvent",             1, 0,  "SettingOilBrandViewEvent",           "");
    qmlRegisterUncreatableType<SettingOilAreaViewMediator>             ("SettingOilAreaViewEvent",              1, 0,  "SettingOilAreaViewEvent",            "");
    qmlRegisterUncreatableType<SettingAQIdetailViewMediator>           ("SettingAQIdetailViewEvent",            1, 0,  "SettingAQIdetailViewEvent",          "");
    qmlRegisterUncreatableType<SettingEvChargeTypeViewMediator>        ("SettingEvChargeTypeViewEvent",         1, 0,  "SettingEvChargeTypeViewEvent",       "");
    qmlRegisterUncreatableType<SelfTestViewMediator>                   ("SelfTestViewEvent",                    1, 0,  "SelfTestViewEvent",                  "");
    qmlRegisterUncreatableType<HiddenSettingFontDpViewMediator>        ("HiddenSettingFontDpViewEvent",         1, 0,  "HiddenSettingFontDpViewEvent",       "");
    qmlRegisterUncreatableType<GadgetShortcutIconViewMediator>         ("GadgetShortcutIconViewEvent",          1, 0,  "GadgetShortcutIconViewEvent",        "");

    qmlRegisterUncreatableType<BookmarkPageViewMediator>               ("BookmarkPageViewEvent",                1, 0,  "BookmarkPageViewEvent",              "");
    qmlRegisterUncreatableType<RecentDestinationPageViewMediator>      ("RecentDestinationPageViewEvent",       1, 0,  "RecentDestinationPageViewEvent",     "");
    qmlRegisterUncreatableType<RecentDestinationEditViewMediator>      ("RecentDestinationEditViewEvent",       1, 0,  "RecentDestinationEditViewEvent",     "");
    qmlRegisterUncreatableType<RecentSearchEditViewMediator>           ("RecentSearchEditViewEvent",            1, 0,  "RecentSearchEditViewEvent",          "");
    qmlRegisterUncreatableType<RecentSearchPageViewMediator>           ("RecentSearchPageViewEvent",            1, 0,  "RecentSearchPageViewEvent",          "");
    qmlRegisterUncreatableType<NoResultPageViewMediator>               ("NoResultPageViewEvent",                1, 0,  "NoResultPageViewEvent",              "");
    qmlRegisterUncreatableType<SearchResultDetailViewMediator>         ("SearchResultDetailViewEvent",          1, 0,  "SearchResultDetailViewEvent",        "");
    qmlRegisterUncreatableType<SearchResultViewMediator>               ("SearchResultViewEvent",                1, 0,  "SearchResultViewEvent",              "");
    qmlRegisterUncreatableType<SearchTabViewMediator>                  ("SearchTabViewEvent",                   1, 0,  "SearchTabViewEvent",                 "");
    qmlRegisterUncreatableType<TextSearchViewMediator>                 ("TextSearchViewEvent",                  1, 0,  "TextSearchViewEvent",                "");
    qmlRegisterUncreatableType<IndustrySearchViewMediator>             ("IndustrySearchViewEvent",              1, 0,  "IndustrySearchViewEvent",            "");

    qmlRegisterUncreatableType<SearchResultOptionViewMediator>         ("SearchResultOptionViewEvent",          1, 0,  "SearchResultOptionViewEvent",        "");
    qmlRegisterUncreatableType<MapModeOptionViewMediator>              ("MapModeOptionViewEvent",               1, 0,  "MapModeOptionViewEvent",             "");
    qmlRegisterUncreatableType<SearchAroundViewMediator>               ("SearchAroundViewEvent",                1, 0,  "SearchAroundViewEvent",              "");
    qmlRegisterUncreatableType<NaviMenuViewMediator>                   ("NaviMenuViewEvent",                    1, 0,  "NaviMenuViewEvent",                  "");

    qmlRegisterUncreatableType<BookmarkRegistViewMediator>             ("BookmarkRegistViewEvent",              1, 0,  "BookmarkRegistViewEvent",            "");
    qmlRegisterUncreatableType<BookmarkPageEditViewMediator>           ("BookmarkPageEditViewEvent",            1,0,   "BookmarkPageEditViewEvent",          "");

    qmlRegisterUncreatableType<KeyboardViewMediator>("KeyboardViewEvent", 1, 0, "KeyboardViewEvent", "");
    qmlRegisterUncreatableType<HardKeyViewMediator>("HardKeyViewEvent", 1, 0, "HardKeyViewEvent", "");
    qmlRegisterUncreatableType<Zone1ViewMediator>("Zone1ViewEvent", 1, 0, "Zone1ViewEvent", "");

    qmlRegisterUncreatableType<FullscreenAlertViewMediator>("FullscreenAlertViewEvent", 1, 0, "FullscreenAlertViewEvent", "");
    qmlRegisterUncreatableType<HiPassLaneViewMediator>("HiPassLaneViewEvent", 1, 0, "HiPassLaneViewEvent", "");
    qmlRegisterUncreatableType<SpeedMeterViewMediator>("SpeedMeterViewEvent", 1, 0, "SpeedMeterViewEvent", "");
    qmlRegisterUncreatableType<PoiTabViewMediator>("PoiTabViewEvent", 1, 0, "PoiTabViewEvent", "");

    qmlRegisterUncreatableType<WidgetSmallViewMediator>("WidgetSmallViewEvent", 1, 0, "WidgetSmallViewEvent", "");
    qmlRegisterUncreatableType<WidgetMediumViewMediator>("WidgetMediumViewEvent", 1, 0, "WidgetMediumViewEvent", "");
    qmlRegisterUncreatableType<WidgetLargeViewMediator>("WidgetLargeViewEvent", 1, 0, "WidgetLargeViewEvent", "");
    qmlRegisterUncreatableType<WidgetXlViewMediator>("WidgetXlViewEvent", 1, 0, "WidgetXlViewEvent", "");
    qmlRegisterUncreatableType<WidgetXxlViewMediator>("WidgetXxlViewEvent", 1, 0, "WidgetXxlViewEvent", "");


    qmlRegisterUncreatableType<MonkeyTestViewMediator>("MonkeyTestViewEvent", 1, 0, "MonkeyTestViewEvent", "");
    qmlRegisterUncreatableType<MapInClusterViewMediator>("MapInClusterViewEvent", 1, 0, "MapInClusterViewEvent", "");
    qmlRegisterUncreatableType<MapInClusterSideViewMediator>("MapInClusterSideViewEvent", 1, 0, "MapInClusterSideViewEvent", "");
	qmlRegisterUncreatableType<MapInClusterHudViewMediator>("MapInClusterHudViewEvent", 1, 0, "MapInClusterHudViewEvent", "");

    // hidden menu
    qmlRegisterUncreatableType<HiddenDiagSettingViewMediator>("HiddenDiagSettingViewEvent", 1, 0, "HiddenDiagSettingViewEvent", "");    
    qmlRegisterUncreatableType<HiddenVsmDiagSettingViewMediator>("HiddenVsmDiagSettingViewEvent", 1, 0, "HiddenVsmDiagSettingViewEvent", "");    
    qmlRegisterUncreatableType<HiddenBoschSettingViewMediator>("HiddenBoschSettingViewEvent", 1, 0, "HiddenBoschSettingViewEvent", "");
    qmlRegisterUncreatableType<HiddenMenuLoginViewMediator>("HiddenMenuLoginViewEvent", 1, 0, "HiddenMenuLoginViewEvent", "");    
    qmlRegisterType<MapConfigDebugUiControl>("MapConfigControl", 1, 0, "MapConfigControl");

    // register qml singleton    
    qmlRegisterSingletonType<RgLaneCode>("RgLaneCode", 1, 0, "RgLaneCode", &RgLaneCode::qmlInstance);
    qmlRegisterSingletonType<RgRouteSummaryCongestionCode>("RgRouteSummaryCongestionCode", 1, 0, "RgRouteSummaryCongestionCode", &RgRouteSummaryCongestionCode::qmlInstance);
    qmlRegisterSingletonType<CommonConstants>("CommonConstants", 1, 0, "CommonConstants", &CommonConstants::qmlInstance);
    qmlRegisterSingletonType<ZoneConstants>("ZoneConstants", 1, 0, "ZoneConstants", &ZoneConstants::qmlInstance);
    qmlRegisterSingletonType<SettingConstants>("SettingConstants", 1, 0, "SettingConstants", &SettingConstants::qmlInstance);
    qmlRegisterSingletonType<SearchConstants>("SearchConstants", 1, 0, "SearchConstants", &SearchConstants::qmlInstance);    
    qmlRegisterSingletonType<HiddenSettingConstants>("HiddenSettingConstants", 1, 0, "HiddenSettingConstants", &HiddenSettingConstants::qmlInstance );

    qmlRegisterUncreatableType<ViewRegistry>("ViewRegistry", 1, 0, "ViewRegistry", "");
    qmlRegisterUncreatableType<ApplicationConstants>("LayoutConstants", 1, 0, "LayoutConstants", "");
    qmlRegisterUncreatableType<WidgetConstants>("WidgetConstants", 1, 0, "WidgetConstants", "");
    qmlRegisterUncreatableType<ApplicationConstants>("ApplicationConstants", 1, 0, "ApplicationConstants", "");
    qmlRegisterUncreatableType<TestConstants>("TestConstants", 1, 0, "TestConstants", "");
    qmlRegisterUncreatableType<MultiSenseManager>("MultiSenseConstants", 1, 0, "MultiSenseConstants", "");

    qmlRegisterUncreatableType<ChangeDestinationViewMediator>("ChangeDestinationViewEvent", 1, 0, "ChangeDestinationViewEvent", "");
    qmlRegisterUncreatableType<CommonDialogViewMediator>("CommonDialogViewEvent", 1, 0, "CommonDialogViewEvent", "");

    qmlRegisterType<Ring>("Ring", 1, 0, "Ring");
    qmlRegisterType<Csw>("Csw", 1, 0, "Csw");
    qmlRegisterType<PreciseTimer>("PreciseTimer", 1, 0, "PreciseTimer");
    qmlRegisterType<QmlMouseEventListener>("MouseEventListener", 1, 0, "MouseEventListener");
}
