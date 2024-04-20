#include "TestViewMediator.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "SettingConstants.h"
#include "TestConstants.h"
#include "ViewRegistry.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "LangConstants.h"
#include "uiconstants.h"
#include "ViewRegistry.h"
#include "ApplicationConstants.h"
#include "WidgetConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProxy.h"
#include "TRandomUtils.h"
#include "QmlAppHelper.h"
#include "HiddenSettingConstants.h"
#include "MapConstants.h"
#include "../common/mapinclusterlayout.h"
#include "TRandomUtils.h"
#include "NavibookmarkAgentProxy.h"
#include "ProfileManager.h"
#include "NavigationControllerProxy.h"
#include "cipherserviceinterface.h"
#include "TStringUtils.h"
#include "UtilConstants.h"
#include "DebugUtils.h"
#include "RgConstants.h"
#include "SearchConstants.h"
#include "../common/searchrequest.h"

using namespace SKT;

TestViewMediator::TestViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    QWindowList windows = QGuiApplication::allWindows();
    QWindow* win = windows.first();
    win->installEventFilter( this );
}

QList<int> TestViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ApplicationConstants::ReplyToViewList,
        TestConstants::AddMessage
    };

    return notes;
}

bool TestViewMediator::eventFilter(QObject *target, QEvent *event )
{
    static int stroke = -1;

    if (mIsUnderRecordingTouch) {
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease ||
            event->type() == QEvent::MouseMove) {
            auto me = static_cast<QMouseEvent*>(event);
            if (event->type() == QEvent::MouseButtonPress) {
                mTouchPointElapsedTimer.restart();
                if (me->buttons() & Qt::LeftButton) {
//                    stroke++;
//                    stroke %= 2;
//                    if (stroke == 0) {
//                        T_DEBUG( QString("clearStroke") );
//                        QMetaObject::invokeMethod(mViewComponent, "clearStroke" );
//                    }

                    T_DEBUG( QString("[TouchTest]Press") );

                    QMetaObject::invokeMethod(mViewComponent, "addPointToStroke", Q_ARG(QVariant, stroke), Q_ARG(QVariant,me->localPos()) );
                    mIsTouchDragging = true;
                }
            }
            else if (event->type() == QEvent::MouseButtonRelease) {
                //if (me->buttons() & Qt::LeftButton) {
                    T_DEBUG( QString("[TouchTest]Release") );

                    QMetaObject::invokeMethod(mViewComponent, "addPointToStroke", Q_ARG(QVariant, stroke), Q_ARG(QVariant,me->localPos()) );
                    QMetaObject::invokeMethod(mViewComponent, "stopRecordTouch", Q_ARG(QVariant, mTouchButtonIndex) );
                    mIsUnderRecordingTouch = false;
                    mIsTouchDragging = false;
                //}
            }
            else if (event->type() == QEvent::MouseMove) {
                //T_DEBUG( QString("MouseMove: (%1,%2)").arg( me->localPos().x() ).arg( me->localPos().y() ) );                
                if (me->buttons() & Qt::LeftButton) {
                    if (mIsTouchDragging) {
                        T_DEBUG( QString("[TouchTest]Move") );

                        QMetaObject::invokeMethod(mViewComponent, "addPointToStroke", Q_ARG(QVariant, stroke), Q_ARG(QVariant,me->localPos()) );
                    }
                }
            }

            if (mIsTouchDragging) {
                TouchPoint point;
                point.action = event->type();
                point.interval = mTouchPointElapsedTimer.elapsed();
                point.localPos = me->localPos();
                point.windowPos = me->windowPos();
                point.screenPos = me->screenPos();
                mTouchPoints.append(point);

                mTouchPointElapsedTimer.restart();
            }
        }
    }

    return QObject::eventFilter(target, event);
}

bool TestViewMediator::onEvent( int event, const QVariant& data )
{
    auto updateMapTheme = [&](const QString& item, const QVariant& data) {
        auto value = data.toString();
        T_DEBUG("updateMapTheme #1 value: " + value);
        if (value.isEmpty()) {
            return;
        }

        DebugUtils::setMapThemeValue(item, value);

        QVariantMap params;
        params[QStringLiteral("item")] = item;
        params[QStringLiteral("value")] = value;

        sendNotification( TestConstants::UpdateMapTheme, params );
    };

    switch ( event ) {
    case GotoBusan:
        {
            QVariantMap body;
            body["clear"] = true;
            sendNotification(RouteConstants::SetDeparture, body);
            // busan
            body["x"] = 67768269;
            body["y"] = 18573004;
            sendNotification(RouteConstants::SetArrival, body);
            sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)}});

            QMetaObject::invokeMethod(mViewComponent, "close" );

        }
        return true;
    case GotoSokcho:
        {
            QVariantMap body;
            body["clear"] = true;
            sendNotification(RouteConstants::SetDeparture, body);
            body["x"] = 67420336;
            body["y"] = 20032316;
            sendNotification(RouteConstants::SetArrival, body);
            sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)}});

            QMetaObject::invokeMethod(mViewComponent, "close" );

        }
        return true;
    case English:
        {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ChangeLanguage );
            body.insert( LangConstants::Lang, LangConstants::En );
            sendNotification( NavigationControllerConstants::ApplyController, body );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        return true;
    case Korean:
        {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ChangeLanguage );
            body.insert( LangConstants::Lang, LangConstants::Ko );
            sendNotification( NavigationControllerConstants::ApplyController, body );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        return true;
    case Widget:
        {
            QVariantMap body;
            body.insert( QStringLiteral("type"), WidgetConstants::SizeXL );
            body.insert( QStringLiteral("bool"), true );
            sendNotification(ApplicationConstants::ShowGadget, body);
        }
        return true;
    case WithMapData: {
            QVariantMap body;
            body.insert( "state", ApplicationConstants::StateWithMapData);
            sendNotification( ApplicationConstants::ChangeState, body );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        return true;
    case NoMapData: {
            QVariantMap body;
            body.insert( "state", ApplicationConstants::StateNoMapData);
            sendNotification( ApplicationConstants::ChangeState, body );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        return true;
    case WidgetXL:
        {
            QVariantMap body;
            body.insert( QStringLiteral("type"), WidgetConstants::SizeXL );
            body.insert( QStringLiteral("bool"), true );
            sendNotification(ApplicationConstants::ShowGadget, body);
        }
        return true;
    case Etc:
        {
            // closing views by traits
            QVariantMap body;
            body.insert( "release", ViewRegistry::Inquiry);
            sendNotification( Constants::CloseView, body );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case UserProfileChanged:
        {
            QVariantMap body;
            sendNotification(NavigationControllerConstants::UpdatedUserProfile, body);
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case Test:
        {
            T_DEBUG( "#3" );
            auto vals = data.value<QVariantMap>();
            auto test = vals["test"].toInt();
            QVariantMap body;
            body.insert( "test", test );
            sendNotification( TestConstants::RunTest, body );
        }
        return true;
    case GotoStandBy:
        {
            QVariantMap body;
            sendNotification(NavigationControllerConstants::GoToStandby, body);
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case GotoRunning:
        {
            QVariantMap body;
            sendNotification(NavigationControllerConstants::GoToRunning, body);
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
//    case SearchTabView:
//        {
//            sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::TestSearchTabView)}});
//            QMetaObject::invokeMethod(mViewComponent, "close" );
//        }
//        break;
    case CreateSession: {
        const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        auto qobj = qobject_cast<QObject*>(proxy.data());
        QMetaObject::invokeMethod(qobj,"onSessionCreated",Qt::DirectConnection, Q_ARG(bool,true), Q_ARG(int,0), Q_ARG(int,0));
        qDebug() << "UI test create session !!!";
        }
        break;
    case MonkeyTestRandClick: {
            sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::MonkeyTestView)}});
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case MonkeyTestMapControl: {
            monkeyTestMapControl();
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
	case ViewListClicked: {
			sendNotification( ApplicationConstants::RequestViewList );
		}
		break;
    case MultiMapViewClicked: {
            sendNotification(Constants::CreateView,QVariantMap{{CommonConstants::Url,ViewRegistry::url(ViewRegistry::TestMultiMapView)}});
            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case CreateMapInCluster: {
            sendNotification(MapConstants::StartMapInCluster);
        }
        break;
    case DestroyMapInCluster: {
//            sendNotification(Constants::CloseView,QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::MapInClusterView)}});
//            QMetaObject::invokeMethod(mViewComponent, "close" );
            sendNotification(MapConstants::StopMapInCluster);
        }
        break;
    case ChangeMultiSenseMode: {
            int themes[4] = { 0x75e04b, 0x0091ff,  0xadb7c9, 0xe82e15};
            int selected  = data.toInt();
            sendNotification(HiddenSettingConstants::ChangeTheme, themes[selected]);
        }
        break;
    case DisplayCenteralMapInClusterWindow: {
            sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true},{QStringLiteral("layout"),(int)MapInClusterLayout::Layout::CenteralMap}});
        }
        break;
    case DisplaySideMapInClusterWindow: {
            sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true},{QStringLiteral("layout"),(int)MapInClusterLayout::Layout::SideMap}});
        }
        break;
    case DisplayHudMapInClusterWindow: {
            sendNotification(MapConstants::StartMapInCluster, QVariantMap{{QStringLiteral("asWindow"),true},{QStringLiteral("layout"),(int)MapInClusterLayout::Layout::HudMap}});
        }
        break;		
    case ClearFavorites: {
            auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
            proxy->clearFavorites();
        }
        break;
    case AddFavoritesInMaximum: {
            struct Internal {
                static QString randString(int len) {
                    QString s;
                    for ( int i = 0; i < len; i++ ) {
                        QChar ch = TRandomUtils::randInt(0, 11172) + 0xAC00;
                        s.append( ch );
                    }
                    return s;
                }
            };

            auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
            for ( int i = 0; i < TMAP_UI_FAVORITE_MAX; i++ ) {
                FavoriteDto dto;

                //dto.setDest(QString("아르마니익스체인지 신세계사이먼"));
                dto.setDest(Internal::randString(24));
                dto.setAddress(Internal::randString(24));
                dto.setDestType(0);

                // prevent duplication
                //dto.setMapX(66424843);
                //dto.setMapY(19800610);
                dto.setMapX(TRandomUtils::randInt(0, 99999999));
                dto.setMapY(TRandomUtils::randInt(0, 99999999));
                dto.setPoiID(TRandomUtils::randInt(0, 999999999));
                dto.setNavSeq(0);
                dto.setRpFlag(71);
                dto.setInsTime(QString(""));
                dto.setAlias(Internal::randString(24));
                dto.setOpiPoleCate(0);
                dto.setOpiPrice(0);

                proxy->addFavorite(dto);
            }
        }
        break;
    case DbusEncrypt:
    case DbusEncryptMt:
    case QaesEncrypt:
    case QaesEncryptMt:
        testEncrypt( event, data );
        break;
    case GadgetIconFuel:
        {
            const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto qobj = qobject_cast<QObject*>(proxy.data());
            QMetaObject::invokeMethod(qobj,"onShowGadget",Qt::DirectConnection, Q_ARG(int,UIConstants::GadgetIconFuel), Q_ARG(bool,true), Q_ARG(bool,false));
        }
        break;
    case GadgetIconParking:
        {
            const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto qobj = qobject_cast<QObject*>(proxy.data());
            QMetaObject::invokeMethod(qobj,"onShowGadget",Qt::DirectConnection, Q_ARG(int,UIConstants::GadgetIconParking), Q_ARG(bool,true), Q_ARG(bool,false));
        }
        break;
    case GadgetSaveImage:
        {
            sendNotification(NavigationControllerConstants::SaveGadgetImage, data);
        }
        break;
    case StopGadgetIconFuel:
        {
            const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto qobj = qobject_cast<QObject*>(proxy.data());
            QMetaObject::invokeMethod(qobj,"onShowGadget",Qt::DirectConnection, Q_ARG(int,UIConstants::GadgetIconFuel), Q_ARG(bool,false), Q_ARG(bool,false));
        }
        break;
    case StopGadgetIconParking:
        {
            const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto qobj = qobject_cast<QObject*>(proxy.data());
            QMetaObject::invokeMethod(qobj,"onShowGadget",Qt::DirectConnection, Q_ARG(int,UIConstants::GadgetIconParking), Q_ARG(bool,false), Q_ARG(bool,false));
        }
        break;
    case ScreenCapture:
        {
            QQmlProperty::write( mViewComponent, "visible", false );
            QTimer::singleShot(500, this, [this](){
                sendNotification(HiddenSettingConstants::CaptureScreenshot);
                QTimer::singleShot(500, this, [this](){
                    QQmlProperty::write( mViewComponent, "visible", true );
                });
            });
        }
        break;
    case FileGpsSetPos:
        {
            auto percent = data.toDouble();
            sendNotification(NavigationControllerConstants::FileGpsSetPos, percent);
        }
        break;
    case FileGpsRewind:
        {
            sendNotification(NavigationControllerConstants::FileGpsRewind);
        }
        break;
    case FileGpsFastForward:
        {
            sendNotification(NavigationControllerConstants::FileGpsFastForward);
        }
        break;
    case AddressSearchVr:
        {
            const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto qobj = qobject_cast<QObject*>(proxy.data());

            QVariantMap params;
            params[QStringLiteral("state")] = "경남";
            params[QStringLiteral("place")] = "창원시";
            params[QStringLiteral("road")] = "동읍 용잠리";
            params[QStringLiteral("houseNumber")] = "575";
            QMetaObject::invokeMethod(qobj,"onRequestSearchAddressOccur",Qt::DirectConnection, Q_ARG(QVariant,params));
        }
        break;
    case AddressSearchVr2:
        {
            const QSharedPointer<NavigationControllerProxy>& proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            auto qobj = qobject_cast<QObject*>(proxy.data());

            QVariantMap params;
            params[QStringLiteral("state")] = "경남";
            QMetaObject::invokeMethod(qobj,"onRequestSearchAddressOccur",Qt::DirectConnection, Q_ARG(QVariant,params));
        }
        break;
    case ShowProgressIndicator:
        {
            sendNotification( UtilConstants::ShowProgressIndicator );
        }
        break;
    case ShowGadgetXXL:
        {
            QVariantMap params;
            params[QStringLiteral("type")] = UIConstants::GadgetXXL;
            params[QStringLiteral("show")] = true;
            params[QStringLiteral("asView")] = true;
            sendNotification( ApplicationConstants::ShowGadget, params );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case ShowGadgetXL:
        {
            QVariantMap params;
            params[QStringLiteral("type")] = UIConstants::GadgetXL;
            params[QStringLiteral("show")] = true;
            params[QStringLiteral("asView")] = true;
            sendNotification( ApplicationConstants::ShowGadget, params );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case ShowGadgetL:
        {
            QVariantMap params;
            params[QStringLiteral("type")] = UIConstants::GadgetLarge;
            params[QStringLiteral("show")] = true;
            params[QStringLiteral("asView")] = true;
            sendNotification( ApplicationConstants::ShowGadget, params );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case ShowGadgetM:
        {
            QVariantMap params;
            params[QStringLiteral("type")] = UIConstants::GadgetMedium;
            params[QStringLiteral("show")] = true;
            params[QStringLiteral("asView")] = true;
            sendNotification( ApplicationConstants::ShowGadget, params );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case ShowGadgetS:
        {
            QVariantMap params;
            params[QStringLiteral("type")] = UIConstants::GadgetSmall;
            params[QStringLiteral("show")] = true;
            params[QStringLiteral("asView")] = false;
            sendNotification( ApplicationConstants::ShowGadget, params );

            QMetaObject::invokeMethod(mViewComponent, "close" );
        }
        break;
    case QuitGadget:
        {
            //sendNotification( Constants::CloseView, QVariantMap{{QStringLiteral("release"),ViewRegistry::Widget}} );
            QVariantMap params;
            params[QStringLiteral("show")] = false;
            sendNotification( ApplicationConstants::ShowGadget, params );
        }
        break;
    case C_C_CS_C:
        {
            updateMapTheme(TestConstants::C_C_CS_C, data);
        }
        break;
    case C_C_CS_T:
        {
            updateMapTheme(TestConstants::C_C_CS_T, data);
        }
        break;
    case C_S_CS_C:
        {
            updateMapTheme(TestConstants::C_S_CS_C, data);
        }
        break;
    case C_S_CS_T:
        {
            updateMapTheme(TestConstants::C_S_CS_T, data);
        }
        break;
    case D_CS_C:
        {
            updateMapTheme(TestConstants::D_CS_C, data);
        }
        break;
    case D_CS_T:
        {
            updateMapTheme(TestConstants::D_CS_T, data);
        }
        break;
    case GXXL_CS_C:
        {
            updateMapTheme(TestConstants::GXXL_CS_C, data);
        }
        break;
    case GXL_CS_C:
        {
            updateMapTheme(TestConstants::GXL_CS_C, data);
        }
        break;
    case GL_CS_C:
        {
            updateMapTheme(TestConstants::GL_CS_C, data);
        }
        break;
    case GXXL_CS_T:
        {
            updateMapTheme(TestConstants::GXXL_CS_T, data);
        }
        break;
    case GXL_CS_T:
        {
            updateMapTheme(TestConstants::GXL_CS_T, data);
        }
        break;
    case GL_CS_T:
        {
            updateMapTheme(TestConstants::GL_CS_T, data);
        }
        break;

    case C_C_RLT:
        {
            updateMapTheme(TestConstants::C_C_RLT, data);
        }
        break;
    case C_S_RLT:
        {
            updateMapTheme(TestConstants::C_S_RLT, data);
        }
        break;
    case D_RLT:
        {
            updateMapTheme(TestConstants::D_RLT, data);
        }
        break;
    case GXXL_RLT:
        {
            updateMapTheme(TestConstants::GXXL_RLT, data);
        }
        break;
    case GXL_RLT:
        {
            updateMapTheme(TestConstants::GXL_RLT, data);
        }
        break;
    case GL_RLT:
        {
            updateMapTheme(TestConstants::GL_RLT, data);
        }
        break;

    case C_C_RLTT:
        {
            updateMapTheme(TestConstants::C_C_RLTT, data);
        }
        break;
    case C_S_RLTT:
        {
            updateMapTheme(TestConstants::C_S_RLTT, data);
        }
        break;

    case D_RLTT:
        {
            updateMapTheme(TestConstants::D_RLTT, data);
        }
        break;
    case GXXL_RLTT:
        {
            updateMapTheme(TestConstants::GXXL_RLTT, data);
        }
        break;
    case GXL_RLTT:
        {
            updateMapTheme(TestConstants::GXL_RLTT, data);
        }
        break;
    case GL_RLTT:
        {
            updateMapTheme(TestConstants::GL_RLTT, data);
        }
        break;
    case RecordTouch:
        {
            auto m = data.toMap();
            mIsUnderRecordingTouch = m["state"].toInt();
            mTouchButtonIndex = m["index"].toInt();

            QMetaObject::invokeMethod(mViewComponent,"enableClickBlocker", Q_ARG(QVariant,mIsUnderRecordingTouch));
        }
        break;
    case SimPanning:
        {
            T_DEBUGV( "[TouchTest]SimPanning" );

            for (int i = 0; i < mTouchPoints.length(); i++ ) {
                QList<QPointF> points;
                QPointF leftPoint = mTouchPoints[i].windowPos;
                points.append(leftPoint);
                auto interval = mTouchPoints[i].interval;
                if (i == 0) {
                    T_DEBUGV( "[TouchTest]SimPanning Press" );
                    QmlAppHelper::instance()->testTouchPress(points, interval);
                }
                else if (i == mTouchPoints.length() - 1) {
                    T_DEBUGV( "[TouchTest]SimPanning Release" );
                    QmlAppHelper::instance()->testTouchRelease(points, interval);
                }
                else {
                    QmlAppHelper::instance()->testTouchMove(points, interval);
                }
            }
        }
        break;
    case SkyStateDawn:
        {
            QMetaObject::invokeMethod(mViewComponent,"changeSkyState", Q_ARG(QVariant,SkyStateDawn));
        }
        break;
    case SkyStateDusk:
        {
            QMetaObject::invokeMethod(mViewComponent,"changeSkyState", Q_ARG(QVariant,SkyStateDusk));
        }
        break;
    case ClearTouch:
        {
            mTouchPoints.clear();
            QMetaObject::invokeMethod(mViewComponent, "clearStroke" );
        }
        break;
    case SimPinchIn:
        {
            T_DEBUGV( "[TouchTest]SimPinchIn" );

            QPointF leftPoint;
            int leftPointInterval = 0;
            QPointF rightPoint;
            int rightPointInterval = 0;

            int midPos = mTouchPoints.length() / 2;
            for (int i = 0; i < midPos; i++ ) {
                int leftPos = i;
                int rightPos = mTouchPoints.length() - 1 - i;

                if (rightPos >= midPos) {
                    leftPoint = mTouchPoints[leftPos].windowPos;
                    leftPointInterval = mTouchPoints[leftPos].interval;
                    rightPoint = mTouchPoints[rightPos].windowPos;
                    rightPointInterval = mTouchPoints[rightPos].interval;
                }

                QList<QPointF> points;
                points.append(leftPoint);
                points.append(rightPoint);

                if (i == 0) {
                    QmlAppHelper::instance()->testTouchPress(points, 10);
                }
                else if (i == midPos - 1) {
                    QmlAppHelper::instance()->testTouchRelease(points, 10);
                }
                else {
                    QmlAppHelper::instance()->testTouchMove(points, 10);
                }
            }
        }
        break;
    case SimPinchOut:
        {
            T_DEBUGV( "[TouchTest]SimPinchOut" );

            QPointF leftPoint;
            QPointF rightPoint;
            int leftPointInterval = 10;
            int rightPointInterval = 10;
            int midPos = mTouchPoints.length() / 2;
            for (int i = 0; i < midPos; i++ ) {
                if (midPos - i >= 0) {
                    int leftPos = midPos - i;
                    int rightPos = midPos + i;
                    if (midPos + i < mTouchPoints.length()) {
                        leftPoint = mTouchPoints[leftPos].windowPos;
                        leftPointInterval = mTouchPoints[leftPos].interval;
                        rightPoint = mTouchPoints[rightPos].windowPos;
                        rightPointInterval = mTouchPoints[rightPos].interval;
                    }
                }

                QList<QPointF> points;
                points.append(leftPoint);
                points.append(rightPoint);

                if (i == 0) {
                    T_DEBUGV( "[TouchTest]SimPinchOut press" );
                    QmlAppHelper::instance()->testTouchRelease(points, 10);
                }
                else if (i == midPos - 1) {
                    T_DEBUGV( "[TouchTest]SimPinchOut release" );
                    QmlAppHelper::instance()->testTouchPress(points, 10);
                }
                else {
                    T_DEBUGV( "[TouchTest]SimPinchOut move" );
                    QmlAppHelper::instance()->testTouchMove(points, 10);
                }
            }
        }
        break;
    case GadgetBackground:
        {
            QVariantMap params;
            params[QStringLiteral("show")] = false;
            sendNotification( ApplicationConstants::ShowGadget, params );
        }
        break;

    case GpsAntennaOn:
        {
            QVariantMap m;
            m["state"] = RgSignalState::ON;
            sendNotification(RgConstants::ChangedSignalState, m);
        }
        break;
    case GpsAntennaOff:
        {
            QVariantMap m;
            m["state"] = RgSignalState::OFF;
            sendNotification(RgConstants::ChangedSignalState, m);
        }
        break;        
    case CrossImageVnext:
        {
            QString url = "https://cdn.tmap.co.kr/rsd/day/480x540/3278";

            QVariantMap body;
            body.insert( CommonConstants::Url, url );
            body.insert( SearchConstants::SearchType, SearchRequestBase::SearchComplexImage );
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;
    case GadgetFocus:
        {
            auto m = data.toMap();
            //auto type = (UIConstants::GadgetType)m["type"].toInt();
            auto type = UIConstants::GadgetMedium;
            //auto pressed = m["pressed"].toBool();
            auto pressed = true;
            sendNotification(ApplicationConstants::FocusToWidget, QVariantMap{
                 {CommonConstants::FocusIndex, type},
                 {CommonConstants::Focus, pressed}
             });
        }
        break;
    }

    return false;
}

void TestViewMediator::testEncrypt( int event, const QVariant& data )
{
    const int TEST_ENCRYPT_REPEATS = 100;
    const int TEST_ENCRYPT_CHAR_COUNT = 80;
    const int TEST_ENCRYPT_ITEM_COUNT = TMAP_UI_RECENTDESTINATION_MAX;

    struct TestCase {
        TestCase(const QString& name, TestViewMediator* thes, bool qaes, int repeats, int charCount, int itemCount)
            : mName(name), mThis(thes), mQaes(qaes), mRepeats(repeats), mCharCount(charCount), mItemCount(itemCount) {
            mThis->addMessage( QString( "[%1] ideal thread count: %2" ).arg(name).arg( QThread::idealThreadCount() ) );
            mThis->addMessage( QString( "[%1] max thread count: %2" ).arg(name).arg( QThreadPool::globalInstance()->maxThreadCount() ) );

            auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
            mCipher = proxy->getControllerHandle()->cipherService(mQaes);
            for ( int i = 0; i < mItemCount; i++ ) {
                mBas.append( TStringUtils::randomString(charCount).toLocal8Bit() );
            }
        }

        TestViewMediator* mThis = nullptr;
        QString mName;
        bool mQaes = false;
        int mRepeats = 0;
        int mCharCount = 0;
        int mItemCount = 0;
        QList<QByteArray> mBas;
        CipherServiceInterface* mCipher = nullptr;
        int mTestCount = 0;
        QTime mTime;
        double mTotalTime = 0;

        void test() {
            mTime.start();
            mCipher->encrypt(mBas, [this](QList<QByteArray> s){
                mCipher->decrypt(s, [this](QList<QByteArray> s){
                    mTotalTime += mTime.elapsed();
                    for ( int i = 0; i < mItemCount; i++ ) {
                        if ( s.at(i) != mBas.at( i ) ) {
                            mThis->addMessage( QString( "[%1] [%2] %3 != %4" ).arg( mName ).arg( i ).arg( QString::fromLocal8Bit( s.at(i) ) ).arg( QString::fromLocal8Bit( mBas.at(i) ) ) );
                        }
                    }
                    mTestCount++;
                    if ( mTestCount == mRepeats ) {
                        mThis->addMessage(QString("[%1] Done! total: %2").arg(mName).arg(mTotalTime));
                        delete this;
                    }
                    else {
                        test();
                    }
                });
            });
        }

        void test_mt() {
            mTime.start();
            mCipher->encrypt_mt(mBas, [this](QList<QByteArray> s){
                mCipher->decrypt_mt(s, [this](QList<QByteArray> s){
                    mTotalTime += mTime.elapsed();
                    for ( int i = 0; i < mItemCount; i++ ) {
                        if ( s.at(i) != mBas.at( i ) ) {
                            mThis->addMessage( QString( "[%1] %2 != %3" ).arg( i ).arg( QString::fromLocal8Bit( s.at(i) ) ).arg( QString::fromLocal8Bit( mBas.at(i) ) ) );
                        }
                    }
                    mTestCount++;
                    if ( mTestCount == mRepeats ) {
                        mThis->addMessage(QString("[%1] Done! total: %2").arg(mName).arg(mTotalTime));
                        delete this;
                    }
                    else {
                        test_mt();
                    }
                });
            });
        }
    };

    switch ( event ) {
#ifdef BUILD_TARGET
    case DbusEncrypt:
        {
            auto tc = new TestCase( "DBUS", this, false, TEST_ENCRYPT_REPEATS, TEST_ENCRYPT_CHAR_COUNT, TEST_ENCRYPT_ITEM_COUNT);
            tc->test();
        }
        break;
    case DbusEncryptMt:
        {
            auto tc = new TestCase( "DBUS MT", this, false, TEST_ENCRYPT_REPEATS, TEST_ENCRYPT_CHAR_COUNT, TEST_ENCRYPT_ITEM_COUNT);
            tc->test_mt();
        }
        break;
#endif
    case QaesEncrypt:
        {
            auto tc = new TestCase( "QAES", this, true, TEST_ENCRYPT_REPEATS, TEST_ENCRYPT_CHAR_COUNT, TEST_ENCRYPT_ITEM_COUNT);
            tc->test();
        }
        break;
    case QaesEncryptMt:
        {
            auto tc = new TestCase( "QAES MT", this, true, TEST_ENCRYPT_REPEATS, TEST_ENCRYPT_CHAR_COUNT, TEST_ENCRYPT_ITEM_COUNT);
            tc->test_mt();
        }
        break;
    }
}

void TestViewMediator::monkeyTestRandClick()
{
    for ( int i = 0; i < 1000000; i++ ) {
        QPointF pos;
        pos.rx() = TRandomUtils::instance()->randInt( 0, 819 );
        pos.ry() = TRandomUtils::instance()->randInt( 0, 1023 );
        QmlAppHelper::instance()->testMouseClick(Qt::LeftButton,Qt::NoModifier,pos,10);
    }
}

void TestViewMediator::monkeyTestMapControl()
{
    // map view click
    QmlAppHelper::instance()->testMouseClick(Qt::LeftButton,Qt::NoModifier,QPoint( 404, 295 ),1000);

    // map view drag
    QmlAppHelper::instance()->testMousePress(Qt::LeftButton,Qt::NoModifier,QPoint( 404, 295 ),500);
    QmlAppHelper::instance()->testMouseMove(QPoint(405,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(407,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(409,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(411,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(412,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(413,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(415,294),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(416,293),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(419,293),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(420,293),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(423,293),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(424,293),10);
    QmlAppHelper::instance()->testMouseMove(QPoint(425,293),10);
    QmlAppHelper::instance()->testMouseRelease(Qt::LeftButton,Qt::NoModifier,QPoint( 425, 293 ),100);
}

bool TestViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            auto viewList = data[ApplicationConstants::ViewIdList].value<QList<int>>();
            auto viewDataMap = data[ApplicationConstants::ViewDataMap].value<QMap<int,QVariantMap>>();
            for ( auto viewId :viewList ) {
                T_DEBUG( QString("ReplyToViewList objName: %1 visible: %2 active: %3")
                    .arg( ViewRegistry::objectName( viewId ) )
                    .arg( viewDataMap[viewId][ApplicationConstants::Visible].toBool() )
                    .arg( viewDataMap[viewId][ApplicationConstants::Active].toBool() )
                         );
            }
        }
        break;
    case TestConstants::AddMessage:
        {
            QMetaObject::invokeMethod(mViewComponent, "addMessage", Q_ARG(QVariant,note->getBody()) );
        }
        break;
    }

    return false;
}

void TestViewMediator::addMessage( const QString& message )
{
    QMetaObject::invokeMethod(mViewComponent, "addMessage", Q_ARG(QVariant,message) );
}
