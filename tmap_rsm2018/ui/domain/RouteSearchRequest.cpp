#include "RouteSearchRequest.h"
#include "mediator/AbstractViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "ToastsConstants.h"
#include "RouteManager.h"
#include "../common/rprequest.h"

namespace SKT
{

RouteSearchRequest::RouteSearchRequest(AbstractViewMediator *mediator)
{
    mMediator = mediator;
}

// 로컬 검색 시 사용
void RouteSearchRequest::request(TeRouteOption option, int routeId)
{
    QList<TeRouteOption> options;
    options << option;

    QVariantMap body;
    QStringList *filenames = new QStringList();
    QList<TeRouteOption> *routeOption = new QList<TeRouteOption>();

    filenames->append(QString("tvas_route%1.db").arg(routeId));
    routeOption->append(option);

    body.insert(RPREQ_KEY_TvasFileName, QVariant::fromValue(QStringListPtr(filenames)));
    body.insert(RPREQ_KEY_RPOptions, QVariant::fromValue(QRouteOptionListPtr(routeOption)));
    body.insert(RPREQ_KEY_Online, false);
    body.insert(RPREQ_KEY_Command, QObject::tr("경로 요청"));
    body.insert(RPREQ_KEY_RequestType, RPRequest::RequestRoute);

    mMediator->sendNotification(SearchConstants::RequestCommand, body);
}

// 서버 검색 시 사용
void RouteSearchRequest::request(const QList<TeRouteOption> &option)
{
    QVariantMap body;
    QStringList *filenames = new QStringList();
    QList<TeRouteOption> *routeOption = new QList<TeRouteOption>();
    int id = 1;
    for (TeRouteOption o : option) {
        filenames->append(QString("tvas_route%1.db").arg(id));
        routeOption->append(o);
        id++;
    }

    body.insert(RPREQ_KEY_TvasFileName, QVariant::fromValue(QStringListPtr(filenames)));
    body.insert(RPREQ_KEY_RPOptions, QVariant::fromValue(QRouteOptionListPtr(routeOption)));
    body.insert(RPREQ_KEY_Online, true);
    body.insert(RPREQ_KEY_Command, QObject::tr("경로 요청"));
    body.insert(RPREQ_KEY_RequestType, RPRequest::RequestRoute);

    mMediator->sendNotification(SearchConstants::RequestCommand, body);
}

void RouteSearchRequest::requestByVr(TeRouteOption option, bool isOnline)
{
    QList<TeRouteOption> options;
    options << option;

    QVariantMap body;
    QStringList *filenames = new QStringList();
    QList<TeRouteOption> *routeOption = new QList<TeRouteOption>();

    filenames->append(QString("tvas_route%1.db").arg(1));
    routeOption->append(option);

    body.insert(RPREQ_KEY_TvasFileName, QVariant::fromValue(QStringListPtr(filenames)));
    body.insert(RPREQ_KEY_RPOptions, QVariant::fromValue(QRouteOptionListPtr(routeOption)));
    body.insert(RPREQ_KEY_Online, isOnline);
    body.insert(RPREQ_KEY_Command, QObject::tr("경로 요청"));
    body.insert(RPREQ_KEY_ByVR, true);
    body.insert(RPREQ_KEY_RequestType, RPRequest::RequestRoute);

    mMediator->sendNotification(SearchConstants::RequestCommand, body);
}

void RouteSearchRequest::cancel()
{
    QVariantMap m;
    m.insert(RPREQ_KEY_RequestType, RPRequest::RequestCancel);
    mMediator->sendNotification(SearchConstants::RequestCommand, m);
}

void RouteSearchRequest::requestSummary(int id, int x, int y)
{
    QVariantMap m;
    m[SearchConstants::Clear] = true;
    mMediator->sendNotification(RouteConstants::SetDeparture, m);

    m.clear();
    m["x"] = x;
    m["y"] = y;
    m["skipSendWaypointOperation"] = true;
    mMediator->sendNotification(RouteConstants::SetArrival, m);

    m.clear();
    m.insert(RPREQ_KEY_RequestType, RPRequest::RequestRouteSummary);
    QStringList *filenames = new QStringList();
    auto filename = QString("summary%1.db").arg(id);
    filenames->append(filename);
    m[RPREQ_KEY_TvasFileName] = QVariant::fromValue(QStringListPtr(filenames));
    m[SearchConstants::Id] = id;
    mMediator->sendNotification(SearchConstants::RequestCommand, m);
}

}//SKT
