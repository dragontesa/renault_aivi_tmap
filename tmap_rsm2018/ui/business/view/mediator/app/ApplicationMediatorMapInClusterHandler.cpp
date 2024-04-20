#include "ApplicationMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "PlatformHelper.h"
#include "QmlAppHelper.h"
#include "ResourceResolver.h"
#include "ImageProvider.h"
#include "NavigationControllerConstants.h"
#include "mapinclusterlayout.h"
#include "DebugUtils.h"
#include <QQuickItem>
#include <QQuickWindow>
#include "CommonConstants.h"
#include "../common/mapinclusterlayout.h"
#include "ClusterConstants.h"

using namespace SKT;

void ApplicationMediator::handleMapInCluster( const mvc::INotificationPtr& note )
{
    switch (note->getId()) {
    case MapConstants::StartMapInCluster:
        {
            if ( !mHasMapData ) {
                return;
            }

            bool displayAsWindow = false;
            int layout = MapInClusterLayout::Layout::CenteralMap;
            if ( note->getBody().canConvert( QVariant::Map ) ) {
                displayAsWindow = note->getBody().toMap()["asWindow"].toBool();
                layout = (MapInClusterLayout::Layout)note->getBody().toMap()["layout"].toInt();
            }

            if ( displayAsWindow ) {
                auto qmlEngine = new QQmlEngine();
                auto ctx = qmlEngine->rootContext();
                ctx->setContextProperty( "QmlAppHelper", QmlAppHelper::instance() );
                ctx->setContextProperty( "console", QmlAppHelper::instance() );
                ctx->setContextProperty( "Resource", ResourceResolver::instance() );
                qmlEngine->addImageProvider( "ImageProvider", new ImageProvider );

                QQuickWindow* parentWindow = qobject_cast<QQuickWindow*>( mViewComponent );
                QString url = QStringLiteral("qrc:/rsm_ui/qml/MapInClusterEmulationWindow.qml");
                QQmlComponent component( qmlEngine, QUrl(url), QQmlComponent::PreferSynchronous, mViewComponent );
                auto obj = component.beginCreate( qmlEngine->rootContext() );
                auto window = qobject_cast<QQuickWindow*>( obj );
                if ( window == nullptr ) {
                    T_DEBUG( QString( "Could not create the MapInCluster Window error: '%1'" ).arg( component.errorString() ) );
                    return;
                }
                else {
                    QQmlEngine::setObjectOwnership( window, QQmlEngine::CppOwnership );
                    //view->setParent( parentWindow );
                }
                component.completeCreate();

                auto layoutSize = MapInClusterLayout::layoutSize( (MapInClusterLayout::Layout)layout );
                window->setWidth( layoutSize.width() );
                window->setHeight( layoutSize.height() );

                auto parent = window->findChild<QObject*>( QStringLiteral("mapInClusterWrap") );
                QString viewUrl;
                QString objName;
                if ( layout == MapInClusterLayout::Layout::CenteralMap ) {
                    viewUrl = ViewRegistry::url(ViewRegistry::MapInClusterView);
                    objName = ViewRegistry::objectName(ViewRegistry::MapInClusterView);
                }
                else if ( layout == MapInClusterLayout::Layout::SideMap ) {
                    viewUrl = ViewRegistry::url(ViewRegistry::MapInClusterSideView);
                    objName = ViewRegistry::objectName(ViewRegistry::MapInClusterSideView);
                }
				else if ( layout == MapInClusterLayout::Layout::HudMap ) {
                    viewUrl = ViewRegistry::url(ViewRegistry::MapInClusterHudView);
                    objName = ViewRegistry::objectName(ViewRegistry::MapInClusterHudView);
                }

                sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName,objName}});

                QTimer::singleShot(200, this, [this,parent,layout,viewUrl,objName,qmlEngine,window](){
                    sendNotification(Constants::CreateView,
                        QVariantMap{
                            {CommonConstants::Url,viewUrl},
                            {QStringLiteral("parent"), QVariant::fromValue( parent ) },
                            {QStringLiteral("qmlEngine"), QVariant::fromValue( qmlEngine ) },
                            {QStringLiteral("parentWindow"), QVariant::fromValue( window ) }
                        }
                    );
                });
            }
            else {
                auto m = note->getBody().toMap();
                if ( !mMapInClusterRenderer ) {
                    QString viewUrl;
                    QString objName;
                    if ( layout == MapInClusterLayout::Layout::CenteralMap ) {
                        viewUrl = ViewRegistry::url(ViewRegistry::MapInClusterView);
                        objName = ViewRegistry::objectName(ViewRegistry::MapInClusterView);
                    }
                    else if ( layout == MapInClusterLayout::Layout::SideMap ) {
                        viewUrl = ViewRegistry::url(ViewRegistry::MapInClusterSideView);
                        objName = ViewRegistry::objectName(ViewRegistry::MapInClusterSideView);
                    }
                    else if ( layout == MapInClusterLayout::Layout::HudMap ) {
                        viewUrl = ViewRegistry::url(ViewRegistry::MapInClusterHudView);
                        objName = ViewRegistry::objectName(ViewRegistry::MapInClusterHudView);
                    }

                    auto layoutSize = MapInClusterLayout::layoutSize( (MapInClusterLayout::Layout)layout );

                    auto qmlEngine = new QQmlEngine();
                    auto ctx = qmlEngine->rootContext();
                    ctx->setContextProperty( "QmlAppHelper", QmlAppHelper::instance() );
                    ctx->setContextProperty( "console", QmlAppHelper::instance() );
                    ctx->setContextProperty( "Resource", ResourceResolver::instance() );
                    qmlEngine->addImageProvider( "ImageProvider", new ImageProvider );
                    mMapInClusterRenderer = new QmlOffscreenRenderer( qmlEngine, QStringLiteral("qrc:/rsm_ui/qml/MapInClusterRenderer.qml"), layoutSize, ClusterConstants::MapViewExpectedFps );
                    connect( mMapInClusterRenderer, &QmlOffscreenRenderer::loaded, [&](QQmlEngine* theQmlEngine, QObject* rootItem){
                        auto parent = rootItem->findChild<QObject*>( QStringLiteral("mapInClusterWrap") );
                        sendNotification(Constants::CreateView,
                            QVariantMap{
                                {CommonConstants::Url,viewUrl},
                                {QStringLiteral("parent"), QVariant::fromValue( parent ) },
                                {QStringLiteral("qmlEngine"), QVariant::fromValue( theQmlEngine ) },
                                {CommonConstants::ObjectOwnership, QQmlEngine::CppOwnership }
                            }
                        );
                    } );
                    sendNotification(NavigationControllerConstants::StartMapInClusterRenderer, QVariant::fromValue( mMapInClusterRenderer ) );
                }
            }
        }
        break;
    case MapConstants::StopMapInCluster:
        {
            if ( mMapInClusterRenderer ) {
                delete mMapInClusterRenderer;
                mMapInClusterRenderer = nullptr;
            }
        }
        break;
    case MapConstants::ChangeMapInClusterLayout:
        {
            if ( mMapInClusterRenderer ) {
                auto size = MapInClusterLayout::layoutSize((MapInClusterLayout::Layout)note->getBody().toInt());
                mMapInClusterRenderer->resize( size );
            }
        }
        break;
    }
}

