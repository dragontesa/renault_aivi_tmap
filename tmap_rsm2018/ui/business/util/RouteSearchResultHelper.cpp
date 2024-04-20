#include "RouteSearchResultHelper.h"
#include "rprequest.h"
#include "RouteSummaryHelper.h"
#include "../environment.h"
#include <QFileInfo>

namespace SKT
{

static QString getRpTypeString(int type)
{
    switch (type) {
    case RPRequest::RequestNone:
        return "RequestNone";
    case RPRequest::RequestCancel:
        return "RequestCancel";
    case RPRequest::RequestRoute:
        return "RequestRoute";
    case RPRequest::RequestRouteSummary:
        return "RequestRouteSummary";
    case RPRequest::RequestReRouteForce:
        return "RequestReRouteForce";
    case RPRequest::RequestReRouteByAuto:
        return "RequestReRouteByAuto";
    case RPRequest::RequestReRouteByBreakaway:
        return "RequestReRouteByBreakaway";
    case RPRequest::RequestVMSInfo:
        return "RequestVMSInfo";
    default:
        return "--ERROR--";
    }
}

static QString getOptionsString(QRouteOptionListPtr options)
{
    QString str;

    if (nullptr != options) {
        for (auto option : *options.data()) {
            str += RouteSummaryHelper::routeToString(option);
            str += ",";
        }
    }
    return str;
}

static QString getFilenamesString(QStringListPtr filenames)
{
    QString str;

    QString fileBase = Environment::getRoutePath();
    if (nullptr != filenames) {
        for (auto filename : *filenames.data()) {
            auto file = fileBase + "/" + filename;
            QFileInfo info(file);
            auto size = info.size();
            str += filename;
            str += QString("(%1)").arg(size);
            str += ",";
        }
    }

    return str;
}

void RouteSearchResultHelper::dumpResponseCommand(const QVariantMap &req, const QVariantMap &res)
{
    auto type = req[RPREQ_KEY_RequestType].toInt();
    auto errorCode = res[RPREQ_KEY_ErrorCode].toInt();
    auto options = req[RPREQ_KEY_RPOptions].value<QRouteOptionListPtr>();
    auto filenames = req[RPREQ_KEY_TvasFileName].value<QStringListPtr>();
    auto isOnline = req[RPREQ_KEY_Online].toBool();
    auto dateTime = QDateTime::currentDateTime();

    qDebug() << "------------------------------------------------------";
    qDebug() << "RP date : " << dateTime.toString();
    qDebug() << "RP method : " << (isOnline ? "NDDS" : "MNS");
    qDebug() << "RP type : " << getRpTypeString(type);
    if (nullptr != options) {
        qDebug() << "RP options : " << getOptionsString(options);
    }
    if (nullptr != filenames) {
        qDebug() << "RP filenames : " << getFilenamesString(filenames);
    }
    for (auto i = 0; i < 5; i++) {
        auto viaKey = QString("Via%1").arg(i+1);
        if (req.contains(viaKey)) {
            auto via = req[viaKey].value<RoutePosPtr>();
            auto x = via->tp.x;
            auto y = via->tp.y;
            if (x && y) {
                qDebug() << "RP" << "Via" << i+1 << " : " << x << ", " << y;
            }
        }
    }
    qDebug() << "RP result : " << errorCode;
    qDebug() << "------------------------------------------------------";
}

}//SKT
