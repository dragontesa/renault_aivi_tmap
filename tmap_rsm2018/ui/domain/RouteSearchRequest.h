#ifndef ROUTESEARCHREQUEST_H
#define ROUTESEARCHREQUEST_H

#include "MNS_Define.h"

namespace SKT
{

class AbstractViewMediator;

class RouteSearchRequest
{
public:
    RouteSearchRequest(AbstractViewMediator *mediator);

    void setRouteOption(const QList<TeRouteOption> &option);

    void request(TeRouteOption option, int routeId = 1);
    void request(const QList<TeRouteOption> &option);
    void requestByVr(TeRouteOption option, bool isOnline);
    void cancel();
    void requestSummary(int id, int x, int y);

private:
    AbstractViewMediator *mMediator;
    QList<TeRouteOption> options;
    quint32 mProcessed;
};

}//SKT

#endif // ROUTESEARCHREQUEST_H
