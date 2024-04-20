#include "ApplicationMediator.h"
#include <QQuickItem>
#include <QQuickWindow>
#include "Constants.h"
#include "CommonConstants.h"
#include "KeyboardConstants.h"
#include "MapConstants.h"
#include "PlatformHelper.h"
#include "QmlAppHelper.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "SimulatorConstants.h"
#include "ViewRegistry.h"
#include "ZoneConstants.h"
#include "SearchConstants.h"

#include <QElapsedTimer>
#include "PreferenceConstants.h"

using namespace SKT;

// create/show/hide/closeView는 기반 메소드들로 내부에 특정 view나 업무 처리를 위한 로직을 넣지 않아야 함

void ApplicationMediator::createView( const QVariantMap& args )
{
    //qDebug() << "[Perf][CreateView]before";
    QVariantMap tempArgs = args;
    auto url = tempArgs[CommonConstants::Url].toString();
    //qDebug() << "[Perf][CreateView]url: " << url;
    auto ownership = QQmlEngine::CppOwnership;
    if ( tempArgs.contains(CommonConstants::ObjectOwnership) ) {
        ownership = (QQmlEngine::ObjectOwnership)tempArgs[CommonConstants::ObjectOwnership].toInt();
    }

    // 17.11.15 hskim added the capability to load views based on the current layout
    auto layoutNameSuffix = QString( QStringLiteral("_%1") ).arg( QmlAppHelper::instance()->layoutName() );
    QFileInfo fi( url );
    auto baseName = fi.baseName();
    auto dir = QString( url ).remove( fi.fileName() );
    auto layoutBasedUrl = QString( QStringLiteral( "%1%2%3.qml" ) ).arg( dir ).arg( baseName ).arg( layoutNameSuffix );
    if ( QFileInfo( QString( layoutBasedUrl ).remove( QStringLiteral("qrc") ) ).exists() ) {
        //T_DEBUG( QString( "Adjusted the original url to the current layout based one '%1'" ).arg( layoutBasedUrl ) );
        url = layoutBasedUrl;
    }

    // createComponent 실행 중 event loop 실행에 의한 동일 view 중복 생성 방지

    bool skip = false;
    if ( mCreateViewStatus.contains( url ) ) {
        skip = true;
    }

    if ( skip ) {
        qDebug() << "[CreateView]" << QString( "Detected duplicated creations for the same view. \"%1\"" ).arg( url );
        return;
    }

    mCreateViewStatus[url] = Creating;

    // 인자에 visible false가 지정되어 있지 않으면 기본적으로 show 메소드를 호출 함

    bool visible = true;
    if ( tempArgs.contains( CommonConstants::Visible ) ) {
        visible = tempArgs[CommonConstants::Visible].toBool();
    }

    QObject* parent = nullptr;
    auto parentObject = tempArgs[QStringLiteral("parent")].value<QObject*>();
    if ( parentObject != nullptr ) {
        parent = parentObject;
    }
    else {
        auto parentObjectName = tempArgs[QStringLiteral("parentObjectName")].toString();
        if ( !parentObjectName.isEmpty()  ) {
            if ( parentObjectName == QStringLiteral("rootWindow")  ) {
                parent = mViewComponent;
            }
            else {
                parent = mViewComponent->findChild<QObject*>( parentObjectName );
            }
        }
    }

    if ( parent == nullptr ) {
        parent = mContentRoot;
    }

    tempArgs.remove( QStringLiteral("parent") );

    QQuickItem* parentItem = nullptr;
    if ( parent == mViewComponent ) {
        parentItem = qobject_cast<QQuickWindow*>( parent )->contentItem();
    }
    else {
        parentItem = qobject_cast<QQuickItem*>( parent );
    }

    QQmlEngine* qmlEngine = mQmlEngine;
    auto viewQmlEngine = tempArgs[QStringLiteral("qmlEngine")].value<QQmlEngine*>();
    if ( viewQmlEngine ) {
        qmlEngine = viewQmlEngine;
    }

// ViewComponent를 생성하는 시간을 측정하기 위한 매크로
// 엔지니어링 모드에서만 사용할 것.
#define PROFILE_CREATE_VIEW_TIME

#ifdef PROFILE_CREATE_VIEW_TIME
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();
#endif

    QQmlComponent component( qmlEngine, QUrl(url), QQmlComponent::PreferSynchronous, parent );
    auto view = qobject_cast<QQuickItem*>( component.beginCreate( qmlEngine->rootContext() ) );

    if ( view == nullptr ) {
        qDebug() << "[CreateView]" << QString( "Could not create an object from '%1'\n%2" ).arg( url ).arg( component.errorString() );
        return;
    }
    else {
        QQmlEngine::setObjectOwnership( view, ownership );

        view->setParentItem( parentItem );
        for ( auto it = tempArgs.constBegin(); it != tempArgs.constEnd(); it++ ){
            QQmlProperty::write( view, it.key(), QVariant::fromValue( it.value() ) );
        }
    }
    component.completeCreate();

#ifdef PROFILE_CREATE_VIEW_TIME
    auto value = elapsedTimer.elapsed();
//    sendNotification(PreferenceConstants::SavePreference, QVariantMap{
//         {"preferencesName", "Profiler"},
//         {CommonConstants::Section,QStringLiteral("CreateView")},
//         {CommonConstants::Key,url},
//         {CommonConstants::Value, value}
//    });
    static int totalElapsed = 0;
    totalElapsed += value;
    qDebug() << "[QML] " << url << ", " << value << "ms, total: " << totalElapsed;
#endif

    qDebug() << QString( "[CreateView]Created url:%1 parent:%2" ).arg( url ).arg( parentItem->objectName() );

    bool isView = false;
    auto className = QQmlProperty::read(view, QStringLiteral("className")).toString();
    if ( className == QStringLiteral("__View__") || className == QStringLiteral("__PopupView__") ) {
        isView = true;
    }

    if ( isView ) {
        QFileInfo fi( url );        
        auto baseName = fi.baseName();

        // layout
        auto layout = QQmlProperty::read( view, QStringLiteral("layout") ).value<QObject*>();
        if ( !layout ) {
            auto dir = QString( url ).remove( fi.fileName() );
            auto layoutUrl = dir + baseName + QStringLiteral("Layout.qml");

            QFileInfo fi( QString( layoutUrl ).remove( QStringLiteral("qrc") ) );
            if ( !fi.exists() ) {
            }
            else {
                QQmlComponent layoutComponent( qmlEngine, QUrl(layoutUrl), QQmlComponent::PreferSynchronous, view );
                layout = qobject_cast<QObject*>( layoutComponent.beginCreate( qmlEngine->rootContext() ) );
                if ( layout == nullptr ) {
                    qDebug() << QString( "[CreateView]Could not create an layout object from '%1'\n%2" ).arg( layoutUrl ).arg( layoutComponent.errorString() );
                }
                else {
                    QQmlEngine::setObjectOwnership( layout, ownership );
                    layoutComponent.completeCreate();

                    QQmlProperty::write(view, QStringLiteral("layout"), QVariant::fromValue( layout ) );
                }
            }
        }
        if ( layout ) {
            QMetaObject::invokeMethod(layout, "updateLayout", Q_ARG(QVariant,QmlAppHelper::instance()->layout()) );
        }

        QQmlProperty::write(view, CommonConstants::Url, url );
        if ( visible ) {
            QMetaObject::invokeMethod(view, "show");
        }
        else {
            QQmlProperty::write(view, CommonConstants::Visible, visible );
        }

        QObject* parentView = nullptr;
        QObject* tParent = QQmlProperty::read(view, QStringLiteral("parent")).value<QObject*>();
        while( tParent ) {
            auto className = QQmlProperty::read(tParent, QStringLiteral("className")).toString();
            if ( className == QStringLiteral("__View__") || className == QStringLiteral("__PopupView__") ) {
                parentView = tParent;
                break;
            }
            tParent = QQmlProperty::read(tParent, QStringLiteral("parent")).value<QObject*>();
        }

        QString parentViewObjectName;
        if ( parentView ) {
            parentViewObjectName = QQmlProperty::read(parentView,CommonConstants::ObjectName).toString();
        }

        // set default object name using url file path
        auto objectName = QQmlProperty::read(view,CommonConstants::ObjectName).toString();
        if ( objectName.isEmpty() ) {
            // 17.11.15 hskim normalize baseName to avoid the inconsistency which is caused by the difference from the original url and the adjusted one.
            baseName = QString( baseName ).remove( layoutNameSuffix );
            objectName = baseName;
            QQmlProperty::write(view,CommonConstants::ObjectName, objectName);
        }

        tempArgs[QStringLiteral("view")] = QVariant::fromValue( view );

        auto viewId = ViewRegistry::regist( view );

        QObject::connect( view, SIGNAL(viewVisibleChanged(const QVariant&)), this, SLOT(onViewVisibleChanged(const QVariant&)) );
        QObject::connect( view, SIGNAL(viewDestruction(const QVariant&,const QVariant&)), this, SLOT(onViewDestruction(const QVariant&,const QVariant&)) );

        mViewTree.addView( view, parentView, fi.absoluteFilePath() );

        sendNotification( Constants::MediateView, tempArgs );
		
        mViewIdList.append( viewId );
        mViewDataMap[viewId] = QVariantMap{
            {ApplicationConstants::ViewId, viewId}
        };		
    }

    mCreateViewStatus[url] = Created;

    //qDebug() << "[Perf][CreateView]after";
}

void ApplicationMediator::showView( const QVariantMap& args )
{
    auto objectName = args[CommonConstants::ObjectName].toString();

    auto view = mViewTree.findViewByObjectName( objectName );
    if ( view == nullptr ) {
        T_DEBUG( QString( "Could not find view with name '%1'" ).arg( objectName ) );
    }
    else {
        QMetaObject::invokeMethod(view, "show");
    }
}

void ApplicationMediator::hideView( const QVariantMap& args )
{
    auto objectName = args[CommonConstants::ObjectName].toString();

    auto view = mViewTree.findViewByObjectName( objectName );
    if ( view == nullptr ) {
        T_DEBUG( QString( "Could not find view with name '%1'" ).arg( objectName ) );
    }
    else {
        QQmlProperty::write(view, CommonConstants::Visible, false);
    }
}

void ApplicationMediator::closeView( const QVariantMap& args )
{
    auto objectName = args[CommonConstants::ObjectName].toString();

    if ( args.contains( QStringLiteral("hold") ) || args.contains( QStringLiteral("release") ) ) {
        auto hold = args[QStringLiteral("hold")].toInt();
        auto release = args[QStringLiteral("release")].toInt();

        mViewTree.visitNodes( [&](const ViewTreeNodePtr& node, bool visible, int depth) {
            auto view = node->view();
            auto objectName = QQmlProperty::read( view, QStringLiteral("objectName") ).toString();
            auto regId = QQmlProperty::read( view, QStringLiteral("regId") ).toInt();            

            if ( !ViewRegistry::hasTraits( regId, hold ) && ViewRegistry::hasTraits( regId, release ) ) {
                QMetaObject::invokeMethod(view, "close");
            }

            return true;
        });
    }
    else {
        auto view = mViewTree.findViewByObjectName( objectName );
        if ( view == nullptr ) {
//            T_DEBUG( QString( "Could not find view with name '%1'" ).arg( objectName ) );
        }
        else {
            QMetaObject::invokeMethod(view, "close");            
        }
    }
}

void ApplicationMediator::onViewVisibleChanged( const QVariant& view )
{
    auto obj = view.value<QObject*>();

    mViewTree.handleViewVisibleChanged( obj );
}

void ApplicationMediator::onViewDestruction( const QVariant& view, const QVariant& data )
{
    sendNotification( Constants::RemoveView, data );

    auto obj = view.value<QObject*>();
    mViewTree.handleViewDestruction( obj );

	auto objName = QQmlProperty::read(obj, CommonConstants::ObjectName).toString();
    auto url = QQmlProperty::read(obj, CommonConstants::Url).toString();
    mCreateViewStatus.remove(url);
	
    auto regId = QQmlProperty::read(obj, ApplicationConstants::RegId).toInt();
    auto index = mViewIdList.indexOf( regId );
    if ( index == -1 ) {
        T_DEBUG( QString( "Detected an attempt to remove '%1' from the view list" ).arg( objName ) );
    }
    else {
        mViewIdList.removeAt( index );
        mViewDataMap.remove( regId );
    }	
}

void ApplicationMediator::onViewTreeVisibilityChanged()
{
    auto depthList = mViewTree.visibleDepthList();    

    ViewTreeNodePtr activeNode;

    int tick = -1;
    for ( int depth = depthList.count() - 1; depth >= 0; depth-- ) {
        auto& nodeList = depthList[depth];                
        for ( auto node : nodeList ) {
            auto view = node->view();
            auto regId = QQmlProperty::read( view, QStringLiteral("regId") ).toInt();
            auto exclude = ViewRegistry::hasTraits( regId,
                ViewRegistry::GuiFrame |
                ViewRegistry::RgPopup |
                ViewRegistry::Overlap |
                //ViewRegistry::Widget |
                ViewRegistry::Test
                );
				
            if ( !exclude ) {
                if ( node->tick() > tick ) {
                    tick = node->tick();
                    activeNode = node;
                }
            }
        }
    }

    if ( !activeNode.isNull() ) {
        auto view = activeNode->view();
        auto objName = QQmlProperty::read( view,CommonConstants::ObjectName).toString();
        if ( view != mLastActiveView ) {
            if ( mLastActiveView ) {
                auto lastActiveObjName = QQmlProperty::read( mLastActiveView,CommonConstants::ObjectName).toString();
                QMetaObject::invokeMethod( mLastActiveView, "viewDeactivated", Qt::QueuedConnection );
                sendNotification( ApplicationConstants::DeactivatedView,
                    QVariantMap{
                        {CommonConstants::ObjectName, lastActiveObjName}
                    }
                );
            }

            qDebug() << QString( "[ViewTree]Activate View: %1" ).arg( objName );            
            QMetaObject::invokeMethod( view, "viewActivated", Qt::QueuedConnection );
            sendNotification( ApplicationConstants::ActivatedView,
                QVariantMap{
                    {CommonConstants::ObjectName, objName}
                }
            );

            mLastActiveView = view;
        }
    }
    else {
        mLastActiveView = nullptr;
    }

#ifdef QT_DEBUG	
    if ( mLastActiveView ) {		
        auto lastActiveObjName = QQmlProperty::read( mLastActiveView,CommonConstants::ObjectName).toString();
        //T_DEBUG( QString( "Current Activate View: %1" ).arg( lastActiveObjName ) );
    }
#endif
}
