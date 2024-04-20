#include "ApplicationMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "RgConstants.h"
#include "uiconstants.h"
#include "view/mediator/widget/WidgetUtils.h"
#include "QmlAppHelper.h"
#include "QmlOffscreenRenderer.h"
#include "ImageProvider.h"
#include "ResourceResolver.h"
#include "NavigationControllerConstants.h"
#include "uiconstants.h"
#include "ClusterConstants.h"
#include "GadgetOffscreenRenderer.h"
#include "MapConstants.h"
#include "NavigationControllerProxy.h"
#include "../environment.h"

using namespace SKT;

void ApplicationMediator::initGadgetHanlder()
{
    auto window = qobject_cast<QQuickWindow*>(mViewComponent);

    if ( mGadgetRenderer == nullptr ) {
        mGadgetRenderer = new GadgetOffscreenRenderer( window );

        connect(mGadgetRenderer, &GadgetOffscreenRenderer::stopped, this, &ApplicationMediator::onGadgetOffscreenRendererStopped );
        connect(mGadgetRenderer, &GadgetOffscreenRenderer::windowHasRendered, this, &ApplicationMediator::onGadgetOffscreenRendererWindowHasRendered );
    }
    if ( mIconGadgetProvider.isNull() ) {
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        mIconGadgetProvider = t_new_shared<IconGadgetProvider>( window, proxy->getControllerHandle() );
    }
}

void ApplicationMediator::onGadgetWindowClosing()
{
    sendNotification(ApplicationConstants::ShowGadget, QVariantMap{{"visible",false}});
}

void ApplicationMediator::onGadgetOffscreenRendererStopped()
{
    qDebug() << "[GadgetSwitch]stopped thread: " << QThread::currentThread();
    changeToApp();
}

void ApplicationMediator::onGadgetOffscreenRendererWindowHasRendered()
{
    qDebug() << "[GadgetSwitch]windowHasRendered thread: " << QThread::currentThread();
    auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    proxy->respondToContextSwitchRequest(QVariantMap{{QStringLiteral("response"), NavigationController::CONTEXT_STATE_ACCEPTED}});
}

void ApplicationMediator::stopGadget()
{
    qDebug() << "[GadgetSwitch]stopGadget #1";

    QQmlProperty::write( mBlankWrap, QStringLiteral("visible"), true );

    auto oldIsGadgetVisible = mIsGadgetVisible;
    mIsGadgetVisible = false;
    if ( oldIsGadgetVisible != mIsGadgetVisible ) {
        sendNotification(ApplicationConstants::ChangedGadgetState, QVariantMap{{"visible", mIsGadgetVisible}});
    }

    mActivatedGadgets.clear();
    mGadgetRenderer->updateActivations( 2, mActivatedGadgets );
    mGadgetRenderer->stop();

    qDebug() << "[GadgetSwitch]stopGadget #2";
}

void ApplicationMediator::changeToApp()
{
    qDebug() << "[GadgetSwitch]changeToApp mIsGadgetVisible: " << mIsGadgetVisible;
#ifdef QT_DEBUG
    if ( mGadgetDebugWindow ) {
        QMetaObject::invokeMethod(mGadgetDebugWindow, "clear");
    }
#endif

    if ( mIsGadgetVisible ) {
        return;
    }

    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetXxlView)}});
    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetXlView)}});
    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetLargeView)}});
    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetMediumView)}});
    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetSmallView)}});

    QMetaObject::invokeMethod( mViewComponent, "changeDisplayMode", Q_ARG(QVariant, false), Q_ARG(QVariant, WidgetConstants::SizeNone), Q_ARG(QVariant, QSize()));

    QMetaObject::invokeMethod( mContentWrap, "changeToApp");
    sendNotification( ApplicationConstants::ChangeToApp );


    if ( !mIsGadgetVisible ) {
        QQmlProperty::write( mBlankWrap, QStringLiteral("visible"), false );
        if ( mLastActiveView ) {
            auto regId = QQmlProperty::read( mLastActiveView, QStringLiteral("regId") ).toInt();
            if ( !ViewRegistry::hasTraits( regId, ViewRegistry::DoNotCallActivateWhenSwitchGadgetToNavi ) ) {
                QMetaObject::invokeMethod( mLastActiveView, "viewActivated", Qt::QueuedConnection );
                auto objectName = QQmlProperty::read(mLastActiveView, CommonConstants::ObjectName).toString();
                sendNotification( ApplicationConstants::ActivatedView,
                    QVariantMap{
                        {CommonConstants::ObjectName, objectName}
                    }
                );
            }
        }

        // TMCRSM-3964 update the visibilities of rg views
        sendNotification( RgConstants::RequestUiState );
    }
}

void ApplicationMediator::handleWidget( const mvc::INotificationPtr& note )
{ 
    switch ( note->getId() ) {
    case ApplicationConstants::FocusToWidget:
        {
            auto m = note->getBody().toMap();
            auto instanceId = m[CommonConstants::FocusIndex].toInt();
            auto hasFocus = m[CommonConstants::Focus].toBool();
            auto outlineColor = QColor("#ffffff");
            auto width = 4;
            auto isInteraction = m.contains(CommonConstants::Pressed);
            auto isPressed = false;
            if (isInteraction) {
                isPressed = m[CommonConstants::Pressed].toBool();
                if (isPressed) {
                    outlineColor = QmlAppHelper::instance()->multiSenseColor();
                    width = 2;
                }
            }
            auto r = outlineColor.redF();
            auto g = outlineColor.greenF();
            auto b = outlineColor.blueF();

            // 가젯 이미지는 BGR

            qDebug() << "[3539]FocusToWidget type: " << instanceId << " focus: " << hasFocus;

            switch(instanceId) {
            case UIConstants::GadgetIconAqi:
            case UIConstants::GadgetIconFuel:
            case UIConstants::GadgetIconParking:
                outlineColor.setRgbF(b, g, r);
                mIconGadgetProvider->update(instanceId, hasFocus, isPressed, outlineColor, width);
                break;
            }

            outlineColor.setRgbF(r, g, b);
            mGadgetRenderer->updateFocus( static_cast<UIConstants::GadgetType>(instanceId), hasFocus, outlineColor, width );
        }
        break;
    case ApplicationConstants::RequestGadgetState:
        {
            sendNotification(ApplicationConstants::ReplyToGadgetState, QVariantMap{{"visible", mIsGadgetVisible}});
        }
        break;
    case ApplicationConstants::ShowGadget:
        {
            if ( !mHasMapData ) {
                return;
            }

            auto args = note->getBody().toMap();
            auto viewType = (UIConstants::GadgetType)args[QStringLiteral("type")].toInt();
            auto show = args[QStringLiteral("show")].toBool();
            auto asWindow = args[QStringLiteral("asWindow")].toBool();
            auto asView = args[QStringLiteral("asView")].toBool();

            qDebug() << "[GadgetSwitch][ApplicationMediator]ShowGadget" << "type: " << viewType << " show: " << show << " asWindow: " << asWindow;

            bool isIconGadget = false;
            switch ( viewType ) {
            case UIConstants::GadgetIconAqi:
            case UIConstants::GadgetIconFuel:
            case UIConstants::GadgetIconParking:
                {
                    isIconGadget = true;

                    // 아래 IconGadgetProvider생성 처리를 initGadgetHanlder()으로 이동.
                    // FocusToWidget에서도 mIconGadgetProvider가 사용. 추후 펌웨어에서 시그널 순서가 바뀌면 null pointer접근 가능성 있음
                    // 19.11.07 ymhong.
#if (0)
                    auto window = qobject_cast<QQuickWindow*>(mViewComponent);
                    if ( mIconGadgetProvider.isNull() ) {
                        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
                        mIconGadgetProvider = t_new_shared<IconGadgetProvider>( window, proxy->getControllerHandle() );
                    }
#endif
                    if ( show ) {
                        QString imgFile;
                        switch ( viewType ) {
                        case UIConstants::GadgetIconFuel:
                            imgFile = QStringLiteral( "gad_fuel_charge.png" );
                            break;
                        case UIConstants::GadgetIconParking:
                            imgFile = QStringLiteral( "gad_parking.png" );
                            break;
                        case UIConstants::GadgetIconAqi:
                            imgFile = QStringLiteral( "gad_finedust.png" );
                            break;
                        }
                        mIconGadgetProvider->start( viewType, imgFile );
                    }
                    else {
                        mIconGadgetProvider->stop( viewType );
                    }
                }
                break;
            }

            if ( isIconGadget ) {
                return;
            }

            if ( show ) {
                sendNotification(ApplicationConstants::ResetNavi);

                auto oldIsGadgetVisible = mIsGadgetVisible;
                mIsGadgetVisible = show;
                if ( oldIsGadgetVisible != mIsGadgetVisible ) {
                    sendNotification(ApplicationConstants::ChangedGadgetState, QVariantMap{{"visible", mIsGadgetVisible}});
                }

#if (0)
                closeView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::MapControlView)}});
#endif

                QString viewUrl = WidgetUtils::viewUrl( (UIConstants::GadgetType)viewType );
                QSize viewSize = WidgetUtils::viewSize( (UIConstants::GadgetType)viewType );
                QString viewObjectName = WidgetUtils::viewObjectName( (UIConstants::GadgetType)viewType );

                QMetaObject::invokeMethod( mViewComponent, "changeDisplayMode", Q_ARG(QVariant, true), Q_ARG(QVariant, viewType), Q_ARG(QVariant, viewSize));

                if ( !viewUrl.isEmpty() ) {
                    auto window = qobject_cast<QQuickWindow*>(mViewComponent);

                    QVariantMap map;
                    map["type"] = (int)viewType;
                    map["size"] = viewSize;
                    sendNotification( ApplicationConstants::ChangeToWidget, map );

                    QMetaObject::invokeMethod( mContentWrap, "changeToWidget", Q_ARG(QVariant, viewSize));

                    auto args = QVariantMap{
                        {CommonConstants::Url,viewUrl},
                        {QStringLiteral("parentObjectName"),"widgetWrap"}
                    };

                    switch ( viewType ) {
                    case UIConstants::GadgetXXL:
                        mActivatedGadgets.removeOne( UIConstants::GadgetXL );
                        hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetXlView)}});
                        break;
                    case UIConstants::GadgetXL:
                        mActivatedGadgets.removeOne( UIConstants::GadgetXXL );
                        hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetXxlView)}});
                        break;
                    case UIConstants::GadgetLarge:
                        args["y"] = -390;
                        break;
                    case UIConstants::GadgetMedium:
                        args["x"] = 396;
                        args["y"] = -191;
                        break;
                    case UIConstants::GadgetSmall:
                        args["x"] = 396;
                        args["y"] = -191 + -191;
                        break;
                    }

                    createView(args);
                    showView(QVariantMap{{CommonConstants::ObjectName,viewObjectName}});                    

                    if  (mActivatedGadgets.indexOf( viewType ) == -1 ) {
                        mActivatedGadgets.append( viewType );
                    }

                    // wait until gadget views are created
                    QTimer::singleShot(0, this, [this, viewType, viewSize]() {
                        if ( !mIsGadgetVisible ) {
                            return;
                        }

                        if ( mActivatedGadgets.isEmpty() ) {
                            return;
                        }

                        qDebug() << "[GadgetSwitch]Show activations: " << mActivatedGadgets;
                        mGadgetRenderer->updateActivations( 0, mActivatedGadgets );

                        auto window = qobject_cast<QQuickWindow*>(mViewComponent);

                        QVariantMap map;
                        map.clear();
                        map["type"] = (int)viewType;
                        map["size"] = viewSize;
#ifdef QT_DEBUG
                        auto imageProvider = new ImageProvider;
                        auto qmlEngine = new QQmlEngine();
                        auto ctx = qmlEngine->rootContext();
                        ctx->setContextProperty( "QmlAppHelper", QmlAppHelper::instance() );
                        ctx->setContextProperty( "console", QmlAppHelper::instance() );
                        ctx->setContextProperty( "Resource", ResourceResolver::instance() );
                        qmlEngine->addImageProvider( "ImageProvider", imageProvider );                        

                        if (mGadgetDebugWindow == nullptr) {
                            QString url = QStringLiteral("qrc:/rsm_ui/qml/GadgetEmulationWindow.qml");
                            QQmlComponent component( qmlEngine, QUrl(url), QQmlComponent::PreferSynchronous, mViewComponent );
                            auto obj = component.beginCreate( qmlEngine->rootContext() );
                            mGadgetDebugWindow = qobject_cast<QQuickWindow*>( obj );
                            if ( mGadgetDebugWindow == nullptr ) {
                                T_DEBUG( QString( "Could not create GadgetRendererWindow error: '%1'" ).arg( component.errorString() ) );
                                return;
                            }
                            else {
                                QQmlEngine::setObjectOwnership( window, QQmlEngine::CppOwnership );
                            }
                            component.completeCreate();

                            QQmlProperty::write( mGadgetDebugWindow, QStringLiteral("appObj"), QVariant::fromValue( this ) );

                            //mGadgetDebugWindow->resize( viewSize );
                        }

                        if ( UIConstants::isNaviMapGadget( viewType ) ) {
                             QQmlProperty::write( mGadgetDebugWindow, QStringLiteral("mapType"), viewType );
                        }

                        mGadgetDebugWindow->show();

                        auto imageProviderObj = dynamic_cast<QObject*>(imageProvider);
                        map["imageProvider"] = QVariant::fromValue( imageProviderObj );
                        map["gadgetDebugWindow"] = QVariant::fromValue( mGadgetDebugWindow );
#endif                        
                        map["renderer"] = QVariant::fromValue( mGadgetRenderer );

                        sendNotification( NavigationControllerConstants::StartGadgetRenderer, map );
                    });
                }
            }
            else {
                switch ( viewType ) {
                case UIConstants::GadgetXXL:                    
                    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetXxlView)}});
                    QMetaObject::invokeMethod(mGadgetDebugWindow, "clearXxl");
                    break;
                case UIConstants::GadgetXL:                    
                    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetXlView)}});
                    QMetaObject::invokeMethod(mGadgetDebugWindow, "clearXl");
                    break;
                case UIConstants::GadgetLarge:
                    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetLargeView)}});
                    QMetaObject::invokeMethod(mGadgetDebugWindow, "clearL");
                    break;
                case UIConstants::GadgetMedium:
                    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetMediumView)}});
                    QMetaObject::invokeMethod(mGadgetDebugWindow, "clearM");
                    break;
                case UIConstants::GadgetSmall:
                    hideView(QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::WidgetSmallView)}});
                    QMetaObject::invokeMethod(mGadgetDebugWindow, "clearS");
                    break;
                }
#ifdef QT_DEBUG
                if ( mGadgetDebugWindow ) {
                    //mGadgetDebugWindow->hide();
                }
#endif

                auto window = qobject_cast<QQuickWindow*>(mViewComponent);
                window->update();

                mActivatedGadgets.removeOne( viewType );
                qDebug() << "[GadgetSwitch]Hide viewType: " << viewType << " activations: " << mActivatedGadgets;
                mGadgetRenderer->updateActivations( 1, mActivatedGadgets );
            }
        }
        break;
    }
}
