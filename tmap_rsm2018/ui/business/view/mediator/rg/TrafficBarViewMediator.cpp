#include "TrafficBarViewMediator.h"
#include "CommonConstants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "RouteConstants.h"
#include "mediator/app/MapViewMode.h"
#include "RgDataVo.h"
#include "VSM_Navi.h"
#include "NavigationControllerConstants.h"

namespace SKT {

TrafficBarViewMediator::TrafficBarViewMediator(QObject* viewComponent) :
     AbstractViewMediator(viewComponent),
	 mCurMapViewMode( MapViewMode::Normal ),
     mCurMapViewModeReason( MapViewMode::None ),
     mIsOnline(false),
     mSummaryTbtVisible(false),
     mHighwayVisible(false),
     mHighwayTbtListVisible(false),
     mCrossImageVisible(false)
{
	mTrafficbarView = findChild("trafficBar");
}

QList<int> TrafficBarViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
		RgConstants::UpdateRouteGuidance,
        MapConstants::ApplyMapViewMode,
        RouteConstants::SelectRoute,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::ResponseOnlineStatus,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState,
        RouteConstants::UpdateRouteSummaryByReroute,
        NavigationControllerConstants::ResponseSkyState,
        NavigationControllerConstants::UpdateSkyState
    };

    return notes;
}

bool TrafficBarViewMediator::onEvent( int event, const QVariant& data )
{
	switch ( event ) {
	case Mediated:
	{
		sendNotification(MapConstants::NotifyMapViewMode);
        sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        sendNotification( NavigationControllerConstants::RequestSkyUpdate );
	}
	break;
	default:
		return false;
	}

	return true;
}

bool TrafficBarViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
	switch (note->getId())
	{
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:
    {
        mIsOnline = note->getBody().toBool();
        QQmlProperty::write( mViewComponent, QStringLiteral("onlineState"), mIsOnline?1:0);
        sendNotification(RgConstants::RequestUiState);
    }
    break;
    case RgConstants::ReplyToUiState:
    case RgConstants::ChangedUiState:
    {
        auto m = note->getBody().toMap();
        auto item = m[QStringLiteral("item")].toInt();
        auto value = m[QStringLiteral("value")].toInt();

        if (item == RgConstants::SummaryTbtVisible) {
            mSummaryTbtVisible = value;
            qDebug() << "[RG][ViewSwitch][TrafficBarView]SummaryTbtVisible: " << mSummaryTbtVisible;
        }
        else if (item == RgConstants::HighwayVisible) {
            mHighwayVisible = value;
            qDebug() << "[RG][ViewSwitch][TrafficBarView]HighwayVisible: " << mHighwayVisible;
        }
        else if (item == RgConstants::CrossImageVisible) {
            mCrossImageVisible = value;
            qDebug() << "[RG][ViewSwitch][TrafficBarView]CrossImageVisible: " << mCrossImageVisible;
        }
        else if ( item == RgConstants::HighwayTbtListVisible ) {
            qDebug() << "[RG][ViewSwitch][TrafficBarView]HighwayTbtListVisible: " << mHighwayTbtListVisible;
            mHighwayTbtListVisible = value;
        }

        bool highwayVisible = mHighwayVisible ? mHighwayTbtListVisible : false;        

        // 2019.10.2 lckxx, visualable state : regarding application's display mode like gadget or window
        bool visualable = QQmlProperty::read(mViewComponent, QStringLiteral("visualable")).toBool();
        qDebug() << "[3892]highwayVisible: " << highwayVisible << " visualable: " << visualable;

        //        auto visible = mIsOnline && (!mSummaryTbtVisible && !highwayVisible && !mCrossImageVisible);
        auto visible = (!mSummaryTbtVisible && !highwayVisible && !mCrossImageVisible);
        if ( visible ) {
            if (visualable) {
                QQmlProperty::write( mViewComponent, QStringLiteral("visible"), visible );
                qDebug() << "[RG][ViewSwitch][TrafficBarView]visible: " << visible << " highway: " << highwayVisible << " view: " << mHighwayVisible << " list: " << mHighwayTbtListVisible;
            }
        }
        else {
            QQmlProperty::write( mViewComponent, QStringLiteral("visible"), visible );
        }
    }
    break;
    case RgConstants::UpdateRouteGuidance:
    {
        if ( MapViewMode::isInRouteGuidance( mCurMapViewMode ) ) {
            auto rg = note->getBody().value<RgDataVo>();
            // update remain distance
            double remainDist = rg.nTotalRemainDist();
            QQmlProperty::write(mTrafficbarView,QStringLiteral("leftDistance"),QVariant::fromValue(remainDist));
        }
    }
    break;
    case RouteConstants::SelectRoute:
    case RouteConstants::UpdateRouteSummaryByReroute:
	{
        auto m = note->getBody().toMap();
        auto distance = m["distance"].toInt();
        int routeId   = m["routeId"].toInt();
        qDebug()<<"tb---> up-RS d:" << distance;
		setTotalRouteDistance(distance);
        updateTrafficGuidance(routeId);
	}
		break;
	case MapConstants::ApplyMapViewMode:
	{
		auto data = note->getBody().value<QVariantMap>();
        mCurMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
	    mCurMapViewModeReason = (MapViewMode::ReasonK)data[QStringLiteral("reason")].toInt();
	}
	 break;
    //case NavigationControllerConstants::ResponseSkyState:
    case NavigationControllerConstants::UpdateSkyState: {
        qDebug() << "[TrafficBar]NavigationControllerConstants::UpdateSkyState!";

        auto m = note->getBody().toMap();
        auto nightMode = m["nightMode"].toBool();
        QQmlProperty::write( mViewComponent, "skyState", nightMode?1:0 );
    }
        break;
	default:
		return false;
	}

	return true;
}

void TrafficBarViewMediator::setTotalRouteDistance(uint distance)
{
  // directly
//	std::vector<VSM_stTmapSummaryBaseInfo> summaryBaseList;
//	 VSM_Bool result = VSMNavi_GetRouteSummaryInfo(summaryBaseList);
//	    if (result == VSM_FALSE) {
//	    	T_DEBUG(QStringLiteral("TrafficbarViewMedi: failed to get route summary info"));
//	        return;
//	    }

	QQmlProperty::write(mTrafficbarView,QStringLiteral("totalDistance"),distance);
}

void TrafficBarViewMediator::updateTrafficGuidance(int routeId)
{
    int alternativeRouteId = routeId;
    if (alternativeRouteId < 0 || alternativeRouteId > 2) {
        alternativeRouteId = 0;
        T_DEBUGV("invalid route id %d for traffic info", routeId);
    }

	// update traffic guidance
	std::vector<VSM_ROUTETRAFFIC> trafficGuide;
    VSM_Bool success = VSMNavi_GetRouteTrafficAllList(alternativeRouteId,trafficGuide);
	if (success == VSM_FALSE)
	{
		T_DEBUG("TrafficbarMedi: failed to get route traffic guide");
		return;
	}

	if (trafficGuide.size() > 0)
	{
		// takeout total distance
//		double totalDistance = trafficGuide.back().uAccDist;
//		QQmlProperty::write(mTrafficbarView,QStringLiteral("totalDistance"),totalDistance);

		QVariantList guides;
		for (int i=0;i<trafficGuide.size();i++)
		{
			const VSM_ROUTETRAFFIC& t = trafficGuide.at(i);
			QVariantList guide;
			guide << t.uAccDist << t.uCong;
			guides.append(QVariant::fromValue(guide));
		}

		QQmlProperty::write(mTrafficbarView,QStringLiteral("trafficGuideList"),QVariant::fromValue(guides));
	}
}

}
