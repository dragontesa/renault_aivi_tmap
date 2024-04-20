#include "MapViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "QmlAppHelper.h"
#include <QWindow>
#include <QMouseEvent>
#include <QQuickItem>
#include "RouteManager.h"
#include "StringHelper.h"
#include "SimulationViewMediator.h"
#include "ViewRegistry.h"
#include "SettingDto.h"
#include "PreferenceVo.h"
#include "PreferenceConstants.h"
#include "SoundConstants.h"
#include "NavigationControllerConstants.h"
#include "rprequest.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "mediator/rg/RgUtils.h"
#include "DebugUtils.h"
#include "ApplicationConstants.h"

//added by cjlee 2018.12.21 - 스크린 타임스탬프 로그 파일 기록
#define USE_SCREENTIMELOG   1
#if USE_SCREENTIMELOG
    #include "../environment.h"
    #include <fstream>
    #include <iostream>
    #include <sstream>
    #include <iomanip>
#endif

//#define TMAP_RSM_UI_ENABLE_DYNAMIC_CREATION_OF_RG_POPUPS
using namespace SKT;

void MapViewMediator::rerouteIfNeeded( const QVariantMap& data )
{
    bool isPeriodic = data["periodic"].toBool();
    bool isForce = data["force"].toBool();
    bool isSkipVia = data["isskipvia"].isNull() == true ? false : data["isskipvia"].toBool();
    if (isPeriodic || isForce) {
        if (mSupportPeriodicReroute) {
            const RgDataVo &rgDataVo = data["rgdata"].value<RgDataVo>();

            QVariantMap m;
            auto isSkip = false;

            if (isPeriodic && !mIsOnline) {
                // TMCRSM-2199. 정주기 재탐색 시 비통신이면 교통정보 반영할 수 없는 상황이므로
                // 무시한다.
                // 19.01.03 ymhong.
                isSkip = true;
            } else {
                m["periodic"] = isPeriodic;

                if (!rgDataVo.isNull()) {
                    if (rgDataVo.nDrgVXIdx() > 0) {
                        // 강제 재탐색
                        m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteForce);
                        m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_FORCED_RE_SEARCH_DES);
                        m.insert(RPREQ_KEY_CongestRid, rgDataVo.congestRid());
                    } else {
                        if (rgDataVo.bPeriodicReRouteArea()) {
                            // 정주기 재탐색 - 차량이 안내점 구간 외부에 있음
                            m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteByAuto);
                            m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_REROUTE_PERIOD_OUTSIDE_DES);
                        } else {
                            // 정주기 재탐색 – 차량이 안내점 구간 내부에 있음
                            m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteForce);
                            m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_REROUTE_PERIOD_INSIDE_DES);
                            isSkip = true;
                        }
                    }
                } else {
                    // 음영 지역 내부 재탐색 처리
                    m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteForce);
                    m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_REROUTE_PERIOD_INSIDE_DES);
                    isSkip = true;
                }
            }

            // TMCRSM-1525 정주기 재탐색 항목 참조
            if (!isSkip && !mIsSoundPlaying) {
                sendNotification( RouteConstants::RequestReroute, m );
            }
        }
    } else if (isSkipVia) {
        // 경유지에 근접 하여 자동으로 재탐색
        QVariantMap m;
        m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteByAuto);
        m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_RE_USER_DES);
        sendNotification( RouteConstants::RequestSkipViaReroute, m );
    } else {
        if (mSupportBreakawayReroute) {
            requestRerouteByBreakaway();
        }
    }
}

void MapViewMediator::updateCurrentPosition( const RgDataVo& )
{
    // MapMode 또는 MapView.qml의 property로는 경로 안내중인지 알기 힘들어
    // 아래의 처리를 RouteGuidanceProxy로 옮긴다.
    // 19.04.12 ymhong.

    // 참고> RouteGuidanceProxy::updateCurrentPosition()
}

void MapViewMediator::updateVia( const RgDataVo& rgDataVo )
{
    if ( rgDataVo.isNull() ) {
        return;
    }

    auto isEnableToPassVia = rgDataVo.isEnableToPassVia();

    //T_DEBUG( QString( "isEnableToPassVia %1" ).arg( mIsEnableToPassVia ) );

    if ( mIsEnableToPassVia != isEnableToPassVia ) {
        mIsEnableToPassVia = isEnableToPassVia;

        T_DEBUG( QString( "isEnableToPassVia changed %1" ).arg( mIsEnableToPassVia ) );

        QMetaObject::invokeMethod( mViewComponent, "setSkipViaButtonVisible",  Q_ARG(QVariant, isEnableToPassVia) );
    }
}

void MapViewMediator::createRgViews()
{
#ifndef TMAP_RSM_UI_ENABLE_DYNAMIC_CREATION_OF_RG_POPUPS
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SdiView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("sdiViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::TbtView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("tbtViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::LaneView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("laneViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::CrossImageView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("crossImageViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::HighwayTbtView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("highwayTbtViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SummaryTbtView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("summaryTbtViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::HiPassLaneView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("hiPassLaneViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );
    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SpeedMeterView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("speedMeterViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );

    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::BusLaneView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("busLaneViewWrap")))},
            {CommonConstants::Visible,false}
        }
    );

    sendNotification(Constants::CreateView,
        QVariantMap{
            {CommonConstants::Url,ViewRegistry::url(ViewRegistry::TrafficBarView)},
            {CommonConstants::Parent,QVariant::fromValue(findChild(QStringLiteral("trafficBarWrap")))},
            {CommonConstants::Visible,false}
        }
    );
#endif
}

bool MapViewMediator::handleRg( int event, const QVariant& data )
{
    switch ( event ) {
    case CreateRgPopup: {
#ifdef TMAP_RSM_UI_ENABLE_DYNAMIC_CREATION_OF_RG_POPUPS
            auto jsVal = data.value<QJSValue>();
            auto viewId = jsVal.property( CommonConstants::ViewId ).toInt();
            auto parent = jsVal.property( CommonConstants::Parent ).toQObject();

            T_DEBUG( QString("#1 %1").arg( ViewRegistry::objectName(viewId)) );

            sendNotification(Constants::CreateView,
                QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(viewId)},
                    {CommonConstants::Parent,QVariant::fromValue(parent)}
                }
            );
#endif
        }
        break;
    case CloseRgPopup: {
#ifdef TMAP_RSM_UI_ENABLE_DYNAMIC_CREATION_OF_RG_POPUPS
            auto jsVal = data.value<QJSValue>();
            auto viewId = jsVal.property( CommonConstants::ViewId ).toInt();

            T_DEBUG( QString("#2 %1").arg( ViewRegistry::objectName(viewId)) );

            sendNotification(Constants::CloseView,
                QVariantMap{
                    {CommonConstants::ObjectName,ViewRegistry::objectName(viewId)}
                }
            );
#endif
        }
        break;

    case ReRouteButtonClicked:
        {
            QVariantMap data;
            data.insert( CommonConstants::Command , tr("경로 수동 재탐색") );
            data.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteForce );
            data.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_RE_USER_DES);
            sendNotification( RouteConstants::RequestReroute, data );
        }
        break;

    case SkipViaButtonClicked:
        {
            T_DEBUG( "<B>SkipViaButtonClicked</B>" );
            QVariantMap m;
            m.insert( QStringLiteral("RequestType"), RPRequest::RequestReRouteByAuto);
            m.insert( CommonConstants::Command , tr("경유지 통과") );
            m.insert(RPREQ_KEY_DestSearchCode, RouteConstants::EXPLORER_RE_USER_DES);
            sendNotification(RouteConstants::RequestSkipViaReroute,m);
            QMetaObject::invokeMethod( mViewComponent, "setSkipViaButtonVisible",  Q_ARG(QVariant, false) );
        }
        break;
    }

    return true;
}

void MapViewMediator::handleRg( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::ChangedUiState:
    case RgConstants::ReplyToUiState:
        {
            auto m = note->getBody().toMap();
            auto item = m[QStringLiteral("item")].toInt();
            auto value = m[QStringLiteral("value")].toInt();
            if ( item == RgConstants::SummaryTbtVisible ) {                
                mSummaryTbtVisible = value;
                qDebug() << "[RG][MapView]mSummaryTbtVisible: " << mSummaryTbtVisible;
            }
            else if ( item == RgConstants::HighwayVisible ) {                
                mHighwayTbtVisible = value;
                qDebug() << "[RG][MapView]mHighwayVisible: " << mHighwayTbtVisible;
            }
            else if ( item == RgConstants::HighwayTbtListVisible ) {                
                mHighwayTbtListVisible = value;
                qDebug() << "[RG][MapView]mHighwayTbtListVisible: " << mHighwayTbtListVisible;
            }

            QQmlProperty::write( mViewComponent, "summaryTbtVisible", mSummaryTbtVisible );
            QQmlProperty::write( mViewComponent, "highwayTbtVisible", mHighwayTbtVisible );
            QQmlProperty::write( mViewComponent, "highwayTbtListVisible", mHighwayTbtListVisible );
        }
        break;

    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            auto m = note->getBody().toMap();
            auto isRg = m["isRg"].toBool();
            QQmlProperty::write(mViewComponent, "isRg", isRg);
        }
        break;

    case RgConstants::ChangedSignalState:
        {
            auto m = note->getBody().value<QVariantMap>();
            auto state = m[QStringLiteral("state")].toInt();
            QQmlProperty::write(mGpsState, "signalState", state);
        }
        break;
    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            auto isUpdate = false;

            switch( mMapViewMode ) {
            case MapViewMode::Normal:
            case MapViewMode::Navi:
            case MapViewMode::Simul:
                {
                    updateCurrentPosition( vo );
                    updateVia( vo );
                    isUpdate = true;
                }
                break;
            case MapViewMode::MapControl:
            case MapViewMode::MapInquiry:
            case MapViewMode::MenuNavi:
            case MapViewMode::NormalMapControl:
                updateCurrentPosition( vo );
                break;
            }

            auto hasSimulationView = mViewList.contains(ViewRegistry::SimulationView);
            if (hasSimulationView && !isUpdate) {
                updateCurrentPosition( vo );
            }


            // display MMFB , 2018.4.24
            if (mDisplayMMFB) {
#if USE_SCREENTIMELOG
                struct timespec rtime;
                clock_gettime(CLOCK_MONOTONIC, &rtime);
                std::ostringstream ossRTTime;
                ossRTTime << std::setfill('0') << std::setw(5) << rtime.tv_sec << "."  // 5 digits for 24 hours
                    << std::setw(6) << (int)(rtime.tv_nsec / 1000);  // [ns] => [ms]

                auto ossRTTimeStr = QString::fromStdString(ossRTTime.str());
//                std::ofstream outf;
//                QString logName = Environment::getSysGpsLogPath() + "/screen_time.dat";
//                outf.open(logName.toStdString() ,std::ios_base::out | std::ios_base::app);
//                if(outf.is_open()) {
//                    RGDataPtr rg = vo.rgdata();
//                    QString timeStamp = QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Timestamp);
//                    outf << ossRTTime.str()  <<  "," << timeStamp.toStdString()  << std::endl;
//                    outf.close();
//                }
#endif
                QString mmfb;
                RGDataPtr rg = vo.rgdata();
                mmfb.append(QStringLiteral("timestamp:"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Timestamp));
                mmfb.append(QStringLiteral("\nossRTTime:"));
                mmfb.append(ossRTTimeStr);
                if (mLastSessionId.length()) {
                    mmfb.append(QString("\nsessionId: %1").arg(mLastSessionId));
                }
                if (mRpDebugText.length()) {
                    mmfb.append("\n");
                    mmfb.append(mRpDebugText);
                }
                mmfb.append(QStringLiteral("\n---------------------------"));
                mmfb.append(QStringLiteral("\n deltaLongitude :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Candidates[0].m_deltaLongitude, 0, 'f', 6));
                mmfb.append(QStringLiteral("\n deltaLatitude  :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Candidates[0].m_deltaLatitude, 0, 'f', 6));
                mmfb.append(QStringLiteral("\n probability    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Candidates[0].m_probability));
                mmfb.append(QStringLiteral("\n segmentFlag    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentFlag));
                mmfb.append(QStringLiteral("\n segmentHeading :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Candidates[0].m_segmentHeading));
                mmfb.append(QStringLiteral("\n streetWidth    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_Candidates[0].m_streetWidth));
                mmfb.append(QStringLiteral("\n nGpsMethod    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_nGpsMethod));



#if 0
                mmfb.append(QStringLiteral("\n\n [offload]"));
                mmfb.append(QStringLiteral("\n deltaLongitude :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_deltaLongitude, 0, 'f', 6));
                mmfb.append(QStringLiteral("\n deltaLatitude  :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_deltaLatitude, 0, 'f', 6));
                mmfb.append(QStringLiteral("\n probability    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_probability));
                mmfb.append(QStringLiteral("\n segmentFlag    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_segmentFlag));
                mmfb.append(QStringLiteral("\n segmentHeading :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_segmentHeading));
                mmfb.append(QStringLiteral("\n streetWidth    :"));
                mmfb.append(QString("%1").arg(rg->m_MapMatchingFeedbackInfo.m_OffroadCandidate.m_streetWidth));
                mmfb.append(QStringLiteral("\n---------------------------"));
#endif
                QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("gpsMapmatchFeedbackData"), mmfb );
            }
        }
        break;

    case RgConstants::ReplyToIsSoundPlaying:
        mIsSoundPlaying = note->getBody().toBool();
        break;

    case RouteConstants::DoReroute:
        {
            sendNotification(RgConstants::RequestIsSoundPlaying);

            auto m = note->getBody().toMap();
            rerouteIfNeeded( m );
        }
        break;

    case RouteConstants::RequestSkipViaReroute:
        // 경유지를 근거리로 지나갈 때 자동 skip
        // 경유지 통과 버튼 hide 처리
        // 18.01.18 ymhong
        mIsEnableToPassVia = false;
        QMetaObject::invokeMethod( mViewComponent, "setSkipViaButtonVisible",  Q_ARG(QVariant, false) );
        break;

    case RgConstants::ShowRouteGuidanceContinuously:
        {
            // add the state that user profile got changed up 2018.3.19 by lckxx
            auto body = note->getBody().toMap();
            int from = -1;
            if (body.contains(CommonConstants::From)) {
                from = body[CommonConstants::From].toInt();
            }

            // 경로 이어 가기
            sendNotification( PreferenceConstants::RequestPreference,
                QVariantMap{
                    { CommonConstants::Caller, QStringLiteral("MapViewMediator") },
                    { QStringLiteral("preferencesName"), PreferenceConstants::Rg },
                    { CommonConstants::Section, QStringLiteral("Termination") },
                    { CommonConstants::From, from }
                } );
        }
        break;
    case RgConstants::UpdateGpsMapmatchTimestamp:
    {
        static QTime ticks = QTime::currentTime(); // count in 500ms
        if ( ticks.msecsTo(QTime::currentTime()) >= 500 ) {
            auto body = note->getBody().toMap();
            auto timestamp = body[QStringLiteral("timestamp")].toUInt();
            auto gpstime   = body[QStringLiteral("gpstime")].toDateTime();
//    		QString t = QDateTime::fromMSecsSinceEpoch(gpstime).toString("hh:mm:ss:zzz");
            QString fm=QString("%1 (gps: %2)").arg(timestamp).arg(gpstime.toString("hh:mm:ss:zzz"));
            QQmlProperty::write( mGpsMapmatchTimestamp, QStringLiteral("gpsTimestampData"), fm );

            ticks = QTime::currentTime(); // count in 500ms
        }
    }
    break;
    }
}
