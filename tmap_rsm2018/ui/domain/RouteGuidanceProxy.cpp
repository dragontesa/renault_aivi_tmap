#include "RouteGuidanceProxy.h"
#include "VSM.h"
#include "VSM_Map.h"
#include "VSM_Navi_RgData.h"
#include "VSM_Navi.h"
#include "RgDataVo.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "SimulatorConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "StringHelper.h"
#include "VmsGuideInfoObserver.h"
#include "navigationdata.h"
#include "gpsmanagerinterface.h"
#include "simulatorinterface.h"

#include "CommonConstants.h"
#include "ViewRegistry.h"
#include "TStringUtils.h"
#include "ZoneConstants.h"

#define PERIODIC_REROUTE_TIMEOUT_MS        (5*60*1000)  // TODO: from setting?

using namespace SKT;

RouteGuidanceProxy::RouteGuidanceProxy( QObject* gpsService, QObject *simulator )
    : AbstractProxy( "RgProxy" ),
    mGpsService( qobject_cast<SKT::GpsManagerInterface*>(gpsService) ),
    mSimulator( qobject_cast<SKT::SimulatorInterface*>(simulator) ),
    mBindState( BindStateK::BIND_STATE_GPS ),
    mRerouting( false ),
    mNeedFirstMatching( true ),
    mPeriodicDRGIndex( 0 ),
    mHighwayTBTIdx( -1 ),
    mHighwayTBTGroupIdx( -1 ),
    mSummaryTBTIdx( -1 ),
    mSummaryTBTGroupIdx( -1 ),
    mSimulatorTotalDistance(0),
    mHighwayDisplayState(false),
    mIsRgFinished( false ),
    mIsRg(false),
    mRemainDistance(0),
    mTotalDistance(0),
    mPeriodicRerouteTime(PERIODIC_REROUTE_TIMEOUT_MS)
{
    QObject::connect( mGpsService->qobject(), SIGNAL(routeGuidanceUpdated(SKT::RGDataPtr)),
                      this, SLOT(onRouteGuidanceUpdated(SKT::RGDataPtr)) );
    QObject::connect( this, SIGNAL(gpsTimeBiasChanged(int)),
                      mGpsService->qobject(), SLOT(applyGpsTimeBias(int)) );
    QObject::connect( mSimulator->qobject(), SIGNAL(routeGuidanceUpdated(SKT::RGDataPtr)),
                      this, SLOT(onSimulatorGuidanceUpdated(SKT::RGDataPtr)) );
    QObject::connect( mSimulator->qobject(), SIGNAL(remainDistanceChanged(int)),
                      this, SLOT(onRemainDistanceChanged(int)) );
    QObject::connect( mGpsService->qobject(), SIGNAL(locationUpdated(SKT::LocationDataPtr)),
                    this, SLOT(onGpsLocationDataUpdated(SKT::LocationDataPtr)) );
}

RouteGuidanceProxy::~RouteGuidanceProxy()
{

}


GpsManagerInterface* RouteGuidanceProxy::getGpsServiceHandle()
{
    return mGpsService;
}

void RouteGuidanceProxy::startRouteGuidance(bool isSimul)
{
    if ( !isSimul ) {
        setIsRg( true );
        mIsRgFinished = false;
        mNeedFirstMatching = true;
        mHighwayTBTIdx = -1;
        mHighwayTBTGroupIdx = -1;
        mSummaryTBTIdx = -1;
        mSummaryTBTGroupIdx = -1;
        mPeriodicRerouteTimer.start();
        mPeriodicDRGIndex = 0;
    }
    else {
        setIsRg( true );
        mIsRgFinished = false;
        mHighwayTBTIdx = -1;
        mHighwayTBTGroupIdx = -1;
        mSummaryTBTIdx = -1;
        mSummaryTBTGroupIdx = -1;
    }
}

void RouteGuidanceProxy::stopRouteGuidance()
{
    setIsRg( false );
    mPeriodicRerouteTimer.invalidate();
}

void RouteGuidanceProxy::setBindState(BindStateK state)
{
    mBindState = state;

    // determine whether subscribe route guidance from GpsService or not
    mGpsService->subscribeRouteGuidance(mBindState == BIND_STATE_GPS);
}

RouteGuidanceProxy::BindStateK RouteGuidanceProxy::bindState() const
{
    return mBindState;
}

void RouteGuidanceProxy::setRerouting(bool reroute)
{
    mRerouting = reroute;
    mHighwayTBTIdx = -1;
    mHighwayTBTGroupIdx = -1;
    mSummaryTBTIdx = -1;
    mSummaryTBTGroupIdx = -1;
}

void RouteGuidanceProxy::stopRerouteTimer()
{
    mPeriodicRerouteTimer.invalidate();
}

void RouteGuidanceProxy::blockReroute(bool block)
{
    mBlockReroute = block;
}

void RouteGuidanceProxy::setOnline(bool online)
{
    mIsOnline = online;
}

void RouteGuidanceProxy::setChannelPolicyReceived(bool received)
{
    mChannelPolicyReceived = received;
}

bool RouteGuidanceProxy::isChannelPolicyReceived() const
{
    return mChannelPolicyReceived;
}

void RouteGuidanceProxy::startSimulator()
{
    startRouteGuidance(true);

    if (mSimulator) {
        mSimulator->start();
    }
    mIsSimulatorStarted = true;
}

void RouteGuidanceProxy::stopSimulator()
{
    if (mIsSimulatorStarted) {
        setIsRg( false );

        if (mSimulator) {
            mSimulator->stop();
        }
        mIsSimulatorStarted = false;
    }
}

void RouteGuidanceProxy::setTotalDistance(int distance)
{
    mTotalDistance = distance;
    mRemainDistance = distance;
}

void RouteGuidanceProxy::setSimulatorTotalDistance(int distance)
{
    mSimulatorTotalDistance = distance;
}

bool RouteGuidanceProxy::isSimulatorRunning() const
{
    return mIsSimulatorStarted;
}

bool RouteGuidanceProxy::isRg() const
{
    return mIsRg;
}

int RouteGuidanceProxy::remainDistance() const
{
    return mRemainDistance;
}

int RouteGuidanceProxy::movingDistance() const
{
    auto distance = qMax(0, mTotalDistance - mRemainDistance);

    return distance;
}

/**!
 * dynamically attach or dettach gps location signal
 * lckxx added for gps timestamp on engineer mode 2017.12.22
 */

void RouteGuidanceProxy::connectGpsLocation(bool enabled)
{
	if (enabled) {
		QObject::connect (
				mGpsService->qobject(), SIGNAL(locationUpdated(SKT::LocationDataPtr)),
				this, SLOT(onGpsMapmatchTimestampUpdated(SKT::LocationDataPtr)) );
	}
	else {
		QObject::disconnect(
				mGpsService->qobject(), SIGNAL(locationUpdated(SKT::LocationDataPtr)),
				this, SLOT(onGpsMapmatchTimestampUpdated(SKT::LocationDataPtr)));
	}

}

void RouteGuidanceProxy::changeGpsLogUsage(bool enabled)
{
    mGpsService->changeGpsLogUsage(enabled);
}


void RouteGuidanceProxy::applyGpsTimeBias(int bias)
{
    emit gpsTimeBiasChanged(bias);
}

void RouteGuidanceProxy::setIsRg( bool isRg )
{
    if ( mIsRg != isRg ) {
        QVariantMap m;
        m["isRg"] = isRg;
        sendNotification(RgConstants::ChangedRouteGuidanceState, m);
        T_DEBUG( "#1" );
    }

    mIsRg = isRg;
    if (!mIsRg) {
        mRemainDistance = 0;
        mTotalDistance = 0;
    }
}

void RouteGuidanceProxy::onRouteGuidanceUpdated(SKT::RGDataPtr rgData)
{
    if (mBindState != BindStateK::BIND_STATE_GPS) {
        return;
    }

#ifndef TEST_GADGET_RG
    if (mIsSimulatorStarted) {
        return;
    }
#endif

    // handle reroute
    RerouteResult rerouteResult;

    // 경로 안내중이 아니면 재탐색 이벤트가 발생하지 않도록 처리.
    // 17.07.27 ymhong.
    if (isRg()) {
        // 비통신 시 재탐색 금지 설정이 되었다면 처리가 되지 않도록 함.
        // 19.08.07 ymhong.
        auto isOnline = mIsOnline && mChannelPolicyReceived;

        if (isOnline || !mBlockReroute) {
            rerouteResult = handleReroute(rgData);
        }
    }

    handleRouteGuidanceUpdated(false, rgData, rerouteResult);

    handleNavigationProperty(rgData);

#ifdef QT_DEBUG
    // 재탐색 가능 구간인지 Zone1에 표시.
    if (rgData)
        sendNotification(RgConstants::UpdatePeriodicRerouteArea, rgData->bPeriodicReRouteArea);
#endif
}

void RouteGuidanceProxy::onSimulatorGuidanceUpdated(SKT::RGDataPtr rgData)
{
    RerouteResult rerouteResult;

    handleRouteGuidanceUpdated(true, rgData, rerouteResult);

    handleNavigationProperty(rgData);
}

void RouteGuidanceProxy::onRemainDistanceChanged(int distance)
{
    int currentSpeed;
    if (VSMNavi_GetCurrentSpeed(currentSpeed) == VSM_FALSE)  {
        currentSpeed = 0;
    }
#ifdef QT_DEBUG
    if (!mEnableStopSimulatorBySpeed) {
        currentSpeed = 0;
    }
#endif
    QVariantMap m;
    m["total"] = mSimulatorTotalDistance;
    m["remain"] = distance;
    m["currentSpeed"] = currentSpeed;
    sendNotification(SimulatorConstants::UpdateSimulator, m);
}

void RouteGuidanceProxy::onGpsLocationDataUpdated(LocationDataPtr location)
{
    //qDebug() << "onGpsMapmatchTimestampUpdated(gpsTime)" << gpsTime.toString();
    auto currentTime = location->localDateTime();// QDateTime::currentDateTime();
    //qDebug() << "onGpsMapmatchTimestampUpdated(currentTime)" << currentTime.toString();

    if(location->gpsMode() == LocationData::DR_GPS) {
        auto newSignalState = location->accuracy() <= -99 ? RgSignalState::OFF : RgSignalState::ON;
        if ( mSignalState != newSignalState ) {
            mSignalState = newSignalState;
            QVariantMap m;
            m["state"] = mSignalState;
            sendNotification(RgConstants::ChangedSignalState, m);
        }
    }

    mCurrentTime = currentTime;

    sendNotification(RgConstants::UpdateGpsTime, currentTime);
    sendNotification(SearchConstants::UpdateGpsTime, currentTime);
}

void RouteGuidanceProxy::onGpsMapmatchTimestampUpdated(SKT::LocationDataPtr location)
{
	uint timestamp = (uint)location->timeStamp();

    QDateTime utc   = location->dateTime();
    QDateTime gpstime   = location->localDateTime();
    //T_DEBUG( QString("utc: %1 local: %2").arg(utc).arg(gpstime) );
    //qDebug() << "RouteGuidanceProxy::onGpsMapmatchTimestampUpdated utc: " << utc << " local: " << gpstime;

    QVariantMap m;
    m[QStringLiteral("timestamp")] = timestamp;
    m[QStringLiteral("gpstime")]   = gpstime;
	sendNotification(RgConstants::UpdateGpsMapmatchTimestamp, QVariant::fromValue(m));
}

static const int HIGHWAY_TURN_DISTANCE = 2000;

void RouteGuidanceProxy::handleRouteGuidanceUpdated(bool isSimul, SKT::RGDataPtr rgData, const RerouteResult& rerouteResult)
{
    if ( rgData.isNull() ) {
        return;
    }

    RgDataVo rgDataVo( rgData );
    rgDataVo.setIsSimul( isSimul );

    // reroute
    rgDataVo.setEnableToPassVia( rerouteResult.isEnableToPassVia );

    // summary
    if ( rgData->nTBTListCount > 0 ) {
        bool changed = false;
        if ( mSummaryTBTIdx != rgDataVo.nTBTIndexSummary() ) {
            changed = true;
            T_DEBUG( QString( "[SummaryTbtList]nTBTIndexSummary %1 => %2" ).arg( mSummaryTBTIdx ).arg( rgDataVo.nTBTIndexSummary() ) );
        }
        if ( mSummaryTBTGroupIdx != rgDataVo.nGroupID()) {
            changed = true;
            T_DEBUG( QString( "[SummaryTbtList]nGroupID %1 => %2" ).arg( mSummaryTBTGroupIdx ).arg( rgDataVo.nGroupID() ) );
        }

        // TMCRSM-2180
        if ( mRecreateTbtList ) {
            changed = true;
        }

        struct Internal {
            static void getSummaryTBTList(QVector<VSM_OsTBTList>& tbtList, int tbtIndex, int tbtListCount, int groupId ) {
                if(VSMNavi_IsRoute()){
                    std::vector<VSM_OsTBTList> vsmTbtList;
                    //qInfo() << "[TMCRSM-4108][Summary]tbtIndex: " << tbtIndex << " tbtListCount: " << tbtListCount;
                    if (VSMNavi_GetRouteTBTList(VSM_TBTLIST_ALL, tbtIndex, tbtListCount, vsmTbtList) == VSM_TRUE) {
                        //qDebug() << QString( "[SummaryTbtView]VSMNavi_GetRouteTBTList #1 tbtListCount: %1 vsmTbtList.size: %2" ).arg(tbtListCount).arg(vsmTbtList.size());

//                        #ifdef QT_DEBUG
//                        auto it = vsmTbtList.begin();
//                        for (; it != vsmTbtList.end(); it++ ) {
//                            auto name = StringHelper::instance()->fromCp949( it->szTBTMainText ).trimmed();
//                            auto hex = TStringUtils::charArrayToHexString( it->szTBTMainText, VSM_TEXT_SIZE_ );
//                            qDebug() << QString( "[SummaryTbtView]bHighway: %1 nGroupID: %2 name: %3 tc: %4 " ).arg(it->bHighway).arg(it->nGroupID).arg( name ).arg( it->nTurnCode );
//                        }
//                        #endif

                        auto it = vsmTbtList.begin();
                        for (; it != vsmTbtList.end(); it++ ) {
                            if ( it->bHighway ) {
                                if ( it->nTurnCode == RgTbtCode::TOL_EXP ) {
                                    tbtList.append( *it );
                                }
                                break;
                            }
                            tbtList.append( *it );
                        }

                        //qDebug() << QString( "[SummaryTbtView]VSMNavi_GetRouteTBTList #2 tbtList.size: %1" ).arg(tbtList.size());
                    }
                    else {
                        //T_DEBUG( QString( "VSMNavi_GetRouteTBTList Faied" ) );
                    }
                }
            }
        };

        if( changed ) {
            qDebug() << QString( "[SummaryTBTList]nGroupID: %1 >>>>>>>>>>>>>>>>>>>> " ).arg(rgDataVo.nGroupID());
            mSummaryTBTIdx = rgDataVo.nTBTIndexSummary();
            mSummaryTBTGroupIdx = rgDataVo.nGroupID();
            mSummaryTbtList.clear();                        
            Internal::getSummaryTBTList(mSummaryTbtList, rgData->stTBTListPos.nTBTIndexSummary, rgData->nTBTListCount, rgDataVo.nGroupID());

            // TMCRSM-2884 ramp section
            if (mSummaryTbtList.length() == 0) {
                std::vector<VSM_OsTBTList> tbtList;
                auto newGroupIndex = rgDataVo.nGroupID() + 1;
                qDebug() << QString( "[SummaryTBTList]Retry nGroupID: %1" ).arg(newGroupIndex);
                Internal::getSummaryTBTList(mSummaryTbtList, rgData->stTBTListPos.nTBTIndexSummary, rgData->nTBTListCount, newGroupIndex);
            }

            qDebug() << QString( "<<<<<<<<<<<<<<<<<<<< [SummaryTBTList]length: %1" ).arg( mSummaryTbtList.length() );

            rgDataVo.setSummaryTbtListUpdated( true );
        }

        rgDataVo.setSummaryTbtList( mSummaryTbtList );
    }

    updateCurrentPosition(rgData);
    sendNotification( RgConstants::UpdateRouteGuidance, QVariant::fromValue( rgDataVo ) );
    mRemainDistance = rgData->nTotalDist;

    // highway    
    if ( rgData && rgDataVo.nShowHighway() == 1 ) {
        rgDataVo.setHighwayDisplay( true );
		
        bool changed = false;
        if ( mHighwayTBTIdx != rgDataVo.nTBTIndexHW()) {
            changed = true;
            qDebug() << QString( "[HighwayTBTList]nTBTIndexHW %1 => %2" ).arg( mHighwayTBTIdx ).arg( rgDataVo.nTBTIndexHW() );
        }        
        if ( mHighwayTBTGroupIdx != rgDataVo.nGroupID()) {
            changed = true;
            qDebug() << QString( "[HighwayTBTList]nGroupID %1 => %2" ).arg( mHighwayTBTGroupIdx ).arg( rgDataVo.nGroupID() );
        }

        // TMCRSM-2180
        if ( mRecreateTbtList ) {
            changed = true;
        }

        if( changed ) {
            qDebug() << QString( "[HighwayTBTList] >>>>>>>>>>>>>>>>>>>>" );

            mHighwayTBTIdx = rgDataVo.nTBTIndexHW();
            mHighwayTBTGroupIdx = rgDataVo.nGroupID();
			
            mHighwayTbtList.clear();
            std::vector<VSM_OsTBTList> tbtList;

            if(VSMNavi_IsRoute())
            {
                // 17.10.12 hskim 고속도로 마지막 데이터 처리 추가.
                // VSMNavi_GetRouteTBTList 리턴 리스트의 마지막 요소는 고속도로 마지막 데이터.
                //qInfo() << "[TMCRSM-4108][Summary]tbtIndex: " << rgDataVo.nTBTIndexHW() << " tbtListCount: " << rgData->nTBTListCount;
                if (VSMNavi_GetRouteTBTList(VSM_TBTLIST_HW, rgDataVo.nTBTIndexHW(), rgData->nTBTListCount, tbtList) == VSM_TRUE) {

                    //T_DEBUG( QString( "Highway TBT ============================" ) );

                    int groupID = rgDataVo.nGroupID();
                    for (const VSM_OsTBTList& tbt : tbtList) {
                        if (tbt.nGroupID == groupID) {
                            #ifdef QT_DEBUG
                                auto name = StringHelper::instance()->fromCp949( tbt.szTBTMainText ).trimmed();
                                qDebug() << QString( "[HighwayTBTList]nGroupID: %1 name: %2 fee: %3" ).arg( groupID ).arg( name ).arg( tbt.nTollFee );
                            #endif

                            mHighwayTbtList.append(tbt);
                        }
                    }

                    // 17.10.12 hskim 마지막 요소와 그 바로 이전 요소가 같을 시 이전 요소는 스킵하도록 처리.
                    if ( mHighwayTbtList.size() > 1 ) {
                        auto rightBeforeLast = mHighwayTbtList[mHighwayTbtList.size() - 2];
                        auto last = mHighwayTbtList[mHighwayTbtList.size() - 1];
                        if ( memcmp( &last, &rightBeforeLast, sizeof(VSM_OsTBTList) ) == 0 ) {
                            mHighwayTbtList.remove( mHighwayTbtList.size() - 2 );
                        }
                    }
                }
            }

            qDebug() << QString( "<<<<<<<<<<<<<<<<<<<< [HighwayTBTList]length: %1" ).arg( mHighwayTbtList.length() );

            rgDataVo.setHighwayTbtListUpdated( true );
        }

        rgDataVo.setHighwayTbtList( mHighwayTbtList );

        // High Way BusLine guide info // 고속도로 && 버스전용차선 && 버스전용차선 가능  -> TMCRSM-3477 fixed
       if (rgData->stCurrent.linkfacil == 0 && rgData->stSDI[0].nSdiType == RgSdiCode::BUS_LANE && rgData->stSDI[0].nDrivingBusLine != 0) {
           // 버스 전용 차선 해제 시나리오??
           sendNotification(RgConstants::ShowBusLaneGuidance,QVariant::fromValue(rgData->stSDI[0].nDrivingBusLine));
        }
    }
    else {
        mHighwayTBTIdx = -1;
        mHighwayTBTGroupIdx = -1;
        mHighwayTbtList.clear();
    }

    auto highwayDisplayStateChanged = mHighwayDisplayState != rgDataVo.highwayDisplay();
    mHighwayDisplayState = rgDataVo.highwayDisplay();

    if ( rgDataVo.highwayDisplay() || highwayDisplayStateChanged ) {
        sendNotification( RgConstants::UpdateHighwayRouteGuidance, QVariant::fromValue( rgDataVo ) );
    }

    // VMS guide info
    if (rgData->bHasVmsInfo) {
    	if (!VmsGuideInfoObserver::isPending())
    	{
    		VmsGuideInfoObserverPtr obs = QSharedPointer<VmsGuideInfoObserver>(new VmsGuideInfoObserver(rgData->nVmsId),VmsGuideInfoObserver::doDeleteLater);
    		obs->requestGuideInfo();
    	}
    }

    // finish
    if ( rgData->eRgStatus == VSM_RG_STATUS_FINISH ) {
        T_DEBUG( "<B>VSM_RG_STATUS_FINISH</B>" );
        if ( !mIsRgFinished ) {
            mIsRgFinished = true;
            sendNotification( RgConstants::TerminatedRouteGuidance );
        }
    } else if ( rgData->eRgStatus == VSM_RG_STATUS_PLAYING){
        //T_DEBUG( "<B>VSM_RG_STATUS_FINISH</B>" );
        if(rgData->nTotalDist <= 30){
//            if(rgData->stGuidePointNext != NULL ){
                if(rgData->stGuidePoint.nTBTTurnType == RgTbtCode::GOAL
                        || rgData->stGuidePointNext.nTBTTurnType == RgTbtCode::GOAL){
                    if ( !mIsRgFinished ){
                        mIsRgFinished = true;
                        sendNotification( RgConstants::TerminatedRouteGuidance );
                    }
                }
//            }
        }
    }

    // TMCRSM-2180
    mRecreateTbtList = false;
}

RouteGuidanceProxy::RerouteResult RouteGuidanceProxy::handleReroute(SKT::RGDataPtr rgData)
{
    RerouteResult ret;

    if (!rgData.isNull()) {
        if (mNeedFirstMatching && rgData->eRgStatus == VSM_RG_STATUS_PLAYING) {
            mNeedFirstMatching = false;
        }

        if (!mRerouting) {
            if (rgData->eRgStatus == VSM_RG_STATUS_TVAS_REROUTE) { // breakaway

                QVariantMap params;
                params["rgdata"] = QVariant::fromValue(RgDataVo(rgData));
                params["periodic"] = false;
                //params["isskipvia"] = true;
                sendNotification(RouteConstants::DoReroute, params);
                // hide Skip Via Button
                ret.isEnableToPassVia = false;

                if (mNeedFirstMatching) {
                    mNeedFirstMatching = false;
                }

            } else if (rgData->bAroundGoPos) {

                if (rgData->eGoPosCode == VSM_NeGoPosType::VSM_PosVia1
                    || rgData->eGoPosCode == VSM_NeGoPosType::VSM_PosVia2
                    || rgData->eGoPosCode == VSM_NeGoPosType::VSM_PosVia3 ) {
                    if(rgData->nGoPosDist > 30) {
                        // show skip via button
                        ret.isEnableToPassVia = true;
                    } else {
                        QVariantMap params;
                        params["rgdata"] = QVariant::fromValue(RgDataVo(rgData));
                        params["periodic"] = false;
                        params["isskipvia"] = true;
                        sendNotification(RouteConstants::DoReroute, params);
                        // hide skip via button -> isskipvia에서 처리
                    }
                } else {
                    QVariantMap params;
                    params["rgdata"] = QVariant::fromValue(RgDataVo(rgData));
                    params["periodic"] = false;
                    //params["isskipvia"] = true;
                    sendNotification(RouteConstants::DoReroute, params);
                    // hide Skip Via Button
                    ret.isEnableToPassVia = false;
                }
            }
        }
    }

    if (!mNeedFirstMatching && !mRerouting) {
        auto isPeriodic = mPeriodicRerouteTimer.isValid() && mPeriodicRerouteTimer.hasExpired(mPeriodicRerouteTime);
#ifdef QT_DEBUG
        if (mForcePeriodicReroute) {
            isPeriodic = true;
            mForcePeriodicReroute = false;
        }
#endif
        if (isPeriodic) {
            if (!rgData.isNull() && isGpsSignalAcceptable(rgData)) {
                QVariantMap params;
                params["rgdata"] = QVariant::fromValue(RgDataVo(rgData));
                params["periodic"] = true;
                sendNotification(RouteConstants::DoReroute, params);
            }
        }

        // DRG
        if (!rgData.isNull() && mPeriodicDRGIndex != rgData->stDRGInfo.nDrgVXIdx && rgData->stDRGInfo.nDrgType > 0) {
            mPeriodicDRGIndex = rgData->stDRGInfo.nDrgVXIdx;
            QVariantMap params;
            params["rgdata"] = QVariant::fromValue(RgDataVo(rgData));
            params["force"] = true;
            params[RPREQ_KEY_CongestRid] = rgData->stDRGInfo.congestRid;
            sendNotification(RouteConstants::DoReroute, params);
        }
    }

    return ret;
}

bool RouteGuidanceProxy::isGpsSignalAcceptable(RGDataPtr rgData) const
{
    // 신호 없음, 터널, 지하차도에서는 false

    if (mSignalState == RgSignalState::OFF) {
        return false;
    }
    switch (rgData->eVirtualGps) {
    case 1: // tunnel
    case 2: // underpass
        return false;

    default:
        return true;
    }
}

void RouteGuidanceProxy::updateCurrentPosition(RGDataPtr rgData)
{
    if (rgData.isNull()) {
        return;
    }
    RgDataVo vo(rgData);

    if (mIsRg) {
        QVariantMap body;

        auto yah = vo.stCurrent_szPosAddrName();// rgData->stCurrent.szPosAddrName;
        auto road = vo.stCurrent_szPosRoadName();// rgData->stCurrent.szNextRoadName;
        auto totalRemainDist = vo.nTotalRemainDist();// rgData->nTotalDist;
        auto totalRemainTime = vo.nTotalRemainTime();// rgData->nTotalTime;

        body[QStringLiteral("yah")] = yah;
        body[QStringLiteral("road")] = road;
        body[QStringLiteral("remainDistance")] = totalRemainDist;
        body[QStringLiteral("remainTime")] = totalRemainTime;

        sendNotification(ZoneConstants::UpdateSoftkeyRouteGuide, body);
    } else {
        auto address = vo.stCurrent_szPosAddrName();// rgData->stCurrent.szPosAddrName;

        sendNotification(ZoneConstants::UpdateSoftkeyAddress, address);
    }
}

void RouteGuidanceProxy::handleNavigationProperty(SKT::RGDataPtr rgData)
{
    if(rgData != NULL)
    {
        RgDataVo rg(rgData);
        QVariantMap body;
        body.insert( QStringLiteral("type"), NavigationPropertyProtocol::TmapRGDataInfo );
        body.insert( QStringLiteral("propertyValue"),QVariant::fromValue(rgData));
        sendNotification( NavigationPropertyConstants::setNavigationProperty, body );
    }
}

void RouteGuidanceProxy::setPeriodicRerouteTime(int t)
{
    mPeriodicRerouteTime = t;
}

void RouteGuidanceProxy::forcePeriodicRerouteForDebugging()
{
    mForcePeriodicReroute = true;
}

void RouteGuidanceProxy::enableSimulatorStopBySpeed(bool b)
{
    mEnableStopSimulatorBySpeed = b;
}

// TMCRSM-2180
void RouteGuidanceProxy::recreateTbtList()
{
    mRecreateTbtList = true;
}

QDateTime RouteGuidanceProxy::currentTime() const
{
    return mCurrentTime;
}

void RouteGuidanceProxy::restartPeriodicRerouteTimer()
{
    mPeriodicRerouteTimer.restart();
}

void RouteGuidanceProxy::setUpdateContinueRg()
{
    mUpdateContinueRg = true;
}

bool RouteGuidanceProxy::isUpdateContinueRg() const
{
    return mUpdateContinueRg;
}
