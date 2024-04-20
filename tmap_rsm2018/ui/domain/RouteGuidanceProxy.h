#ifndef ROUTEGUIDANCEPROXY_H
#define ROUTEGUIDANCEPROXY_H

#include <QElapsedTimer>

#include "AbstractProxy.h"
#include "VSM_Navi_RgData.h"
#include "RgDataVo.h"
#include "RgProtocol.h"
#include "locationdataptr.h"
#include "qctestingfeatures.h"

namespace SKT {

T_SHAREABLE( RouteRequest );
T_SHAREABLE( RouteResponseDto );
T_SHAREABLE( RouteGuidanceProxy );

class GpsManagerInterface;
class SimulatorInterface;
class RouteGuidanceProxy : public AbstractProxy
{
    Q_OBJECT

public:
    enum BindStateK {
        BIND_STATE_NONE = 0,
        BIND_STATE_GPS,
    };

    RouteGuidanceProxy( QObject* gpsService, QObject *simulator );
    virtual ~RouteGuidanceProxy();

    GpsManagerInterface* getGpsServiceHandle();

    void startRouteGuidance(bool isSimul = false);
    void stopRouteGuidance();
    void setBindState(BindStateK state);
    BindStateK bindState() const;
    void setRerouting(bool);
    void stopRerouteTimer();
    void blockReroute(bool block);
    void setOnline(bool online);
    void setChannelPolicyReceived(bool received);
    bool isChannelPolicyReceived() const;

    void startSimulator();
    void stopSimulator();
    void setTotalDistance(int distance);
    void setSimulatorTotalDistance(int distance);
    bool isSimulatorRunning() const;

    bool isRg() const;
    int remainDistance() const;
    int movingDistance() const;

    void connectGpsLocation(bool enabled);
    void changeGpsLogUsage(bool enabled); // change state for going to use gps log, by lckxx 2018.1.4

    void applyGpsTimeBias(int bias); // force adjust gps time
    void setPeriodicRerouteTime(int t);
    void forcePeriodicRerouteForDebugging();
    void enableSimulatorStopBySpeed(bool b);

    // TMCRSM-2180
    void recreateTbtList();

    QDateTime currentTime() const;
    void restartPeriodicRerouteTimer();

    void setUpdateContinueRg();
    bool isUpdateContinueRg() const;

public slots:
    void onRouteGuidanceUpdated(SKT::RGDataPtr rgData);
    void onSimulatorGuidanceUpdated(SKT::RGDataPtr rgData);
    void onRemainDistanceChanged(int distance);
    void onGpsLocationDataUpdated(SKT::LocationDataPtr location);
    void onGpsMapmatchTimestampUpdated(SKT::LocationDataPtr location); // 2017.12.21 lckxx, to display gps mapmatching timestamp on engineer mode
signals:
    void gpsTimeBiasChanged(int bias);

private:
    void setIsRg( bool isRg );

    struct RerouteResult {
        bool isEnableToPassVia = false;
    };
    RerouteResult handleReroute(SKT::RGDataPtr rgData);

    void handleRouteGuidanceUpdated(bool isSimul, SKT::RGDataPtr rgData, const RerouteResult& rerouteResult);
    void handleNavigationProperty(SKT::RGDataPtr rgData);
    bool isGpsSignalAcceptable(SKT::RGDataPtr rgData) const;
    void updateCurrentPosition(SKT::RGDataPtr rgData);

    GpsManagerInterface* mGpsService;
    SimulatorInterface* mSimulator;

   //bool mSimuationMode;
    BindStateK mBindState;
    bool mRerouting;
    bool mNeedFirstMatching;
    QElapsedTimer mPeriodicRerouteTimer;
    int mPeriodicDRGIndex;
    bool mForcePeriodicReroute = false;
    bool mBlockReroute = false;
    bool mIsOnline = false;
    bool mChannelPolicyReceived = false;

    // highway
    int mHighwayTBTIdx;
    int mHighwayTBTGroupIdx;
    QString mGoPosName;
    QVector<VSM_OsTBTList> mHighwayTbtList;
    bool mHighwayDisplayState;

    // summary
    int mSummaryTBTIdx;
    int mSummaryTBTGroupIdx;
    QVector<VSM_OsTBTList> mSummaryTbtList;

    int mSimulatorTotalDistance;

    //
    bool mIsRgFinished;
    bool mIsRg;
    bool mIsSimulatorStarted = false;
    int mRemainDistance;
    int mTotalDistance;
    int mPeriodicRerouteTime;
    bool mUpdateContinueRg = false;

    // signal
    RgSignalState::RgSignalStateK mSignalState = RgSignalState::NONE;

    // TMCRSM-2180
    bool mRecreateTbtList = false;
    bool mEnableStopSimulatorBySpeed = true;

    QDateTime mCurrentTime;
};

}

#endif // ROUTEGUIDANCEPROXY_H
