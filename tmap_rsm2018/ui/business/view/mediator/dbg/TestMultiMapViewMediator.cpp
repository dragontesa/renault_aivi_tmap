#include "TestMultiMapViewMediator.h"
#include "Constants.h"
#include "MapConstants.h"
#include "ViewRegistry.h"
#include "QmlAppHelper.h"
#include "VSM_Navi.h"
#include "RouteConstants.h"

using namespace SKT;

TestMultiMapViewMediator::TestMultiMapViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

TestMultiMapViewMediator::~TestMultiMapViewMediator()
{
    T_DEBUGV( "#1" );
}

QList<int> TestMultiMapViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
    };

    return notes;
}

bool TestMultiMapViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Mediated:
        {
            mMapViewList.append( findChild( "renderer1" ) );
            mMapViewList.append( findChild( "renderer2" ) );
        }
        break;
    case MapViewOnDestruction:
        {        
            T_DEBUG( "MapViewOnDestruction #1" );
            sendNotification(MapConstants::DetachMapViewFromVsmNavi,data);
        }
        break;
    case Navigation:
        {
            QVariantMap m;
            m["mapView"] = QVariant::fromValue( mMapViewList[0] );
            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);

            m["mapView"] = QVariant::fromValue( mMapViewList[1] );
            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);
        }
        break;
    case DrawRouteAllClickedUsingApi:
        {
            QVariantMap m;
            m["mapView"] = QVariant::fromValue( mMapViewList[0] );
            sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);

            QVariantList vlist;
            vlist << 1 << QString("tvas_route1.db");
            vlist << 32 << QString("tvas_route2.db");
            vlist << 8 << QString("tvas_route3.db");
            sendNotification(RouteConstants::SetTvas, vlist);

            QTimer::singleShot(1000/30, this, [&](){
                T_DEBUGV( "DrawRouteAllClickedUsingApi!" );
                QVariantMap m;
                m["mapView"] = QVariant::fromValue( mMapViewList[0] );
                sendNotification(MapConstants::DrawRouteAll, m);
            });
        }
		break;

    case DrawRouteAllClickedUsingMapViewQuick:
        {
			T_DEBUGV( "DrawRouteAllClickedUsingMapViewQuick!" );
			auto routeId = 0;
			for (auto i = 0; i<2; i++) {
				QVariantMap m;
				m["mapView"] = QVariant::fromValue( mMapViewList[i] );
				sendNotification(MapConstants::AttachMapViewOnVsmNavi, m);				

				QVariantList vlist;
				vlist << 1 << QString("tvas_route1.db");
				vlist << 32 << QString("tvas_route2.db");
				vlist << 8 << QString("tvas_route3.db");
				sendNotification(RouteConstants::SetTvas, vlist);
			}

			QMetaObject::invokeMethod(mViewComponent, "drawRouteAll1");
			QMetaObject::invokeMethod(mViewComponent, "drawRouteAll2");

        }
        break;
    }

    return false;
}


bool TestMultiMapViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    return false;
}
