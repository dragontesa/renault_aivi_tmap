#ifndef RPNSEARCHSERVER_H
#define RPNSEARCHSERVER_H

#include "rpnsearch.h"
#include <vector>

namespace SKT {

class RPnSearchServer : public RPnSearch
{
public:
    RPnSearchServer();

    void requestRouteSummary(RPRequestPtr& req);
    void requestRoute(RPRequestPtr& req);
    void requestReroute(RPRequestPtr& req);
    void cancelRoute(RPRequestPtr& req);

    void requestVMSInfo(RPRequestPtr& req);

    void parseSessionId(const char *p);
    QString sessionId();

protected:
    void makeRouteRequest_DefaultData();
    void makeRouteRequest_Option(QRouteOptionListPtr option);
    void makeRouteRequest_Departure();
    void makeRouteRequest_Via1();
    void makeRouteRequest_Via2();
    void makeRouteRequest_Via3();
    void makeRouteRequest_Destination();
    void makeRouteRequest_RerouteData();
    void makeRouteRequest_AngleAndSpeed(RPRequestPtr& req);
    void makeRouteRequest_OilType(RPRequestPtr& req);
    void makeRouteRequest_DefaultData(RPRequestPtr& req, bool reroute = false);

    void makeRouteResponse_DefaultData();
    void makeRouteResponse_Data(QStringListPtr tvasFileNames);
    void makeRouteRequest_GpsTraceData(RPRequestPtr& req);

    void requestClearData();

    DeErrorCode requestRouteUsingDualChannel(RPRequestPtr& req, int &sessionId, int &resCount);

private:
    void makeRouteRequest_RerouteDataUsingMap(const QVariantMap &m, bool isReroute);

private:
    DS_ROUTE_REQ req1;
    std::vector<int> mPreRids;
    std::vector<int> mPreSecs;

    static const int cameraTypes;
    static const int dangerAreaOptions;
    std::string mGpsTraceData;

public:
    QString mSessionId;
    QByteArray mRD4;
    QMutex mSessionMutex;
};

}

#endif // RPNSEARCHSERVER_H
