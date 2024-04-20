#ifndef ROUTESUMMARYHELPER_H
#define ROUTESUMMARYHELPER_H

#include "rprequest.h"

struct VSM_stTmapSummaryList;

namespace SKT
{

enum RouteSummaryPoiCode {
    ROUTE_SUMMARY_POI_DEPARTURE = 23,
    ROUTE_SUMMARY_POI_ARRIVAL,
    ROUTE_SUMMARY_POI_VIA1,
    ROUTE_SUMMARY_POI_VIA2,
    ROUTE_SUMMARY_POI_VIA3
};

class RouteSummaryItemVo;

class RouteSummaryHelper
{
public:
    RouteSummaryHelper(const std::vector<VSM_stTmapSummaryList>&, const QStringList&, bool isLocal);

    QVariant model() const;

    int skipItemsCount() const;
    void setFooterItemCount(int skipItemsCount);

    static QString routeToString(TeRouteOption e);
    static TeRouteOption routeToEnum(const QString& s);
    static QStringList routeTypeStrings();

private:
    void parseHeader(const std::vector<VSM_stTmapSummaryList>&);
    QVariant parseBodyOld(const std::vector<VSM_stTmapSummaryList>&, const QStringList&);
    QVariant parseBody(const std::vector<VSM_stTmapSummaryList>&, const QStringList&);
    bool isArrivalTurnCode(int) const;
    bool isArrivalOrViaTurnCode(int) const;

#ifdef QT_DEBUG
    void printSummary(int index, int sx, int sy, int ex, int ey, int turnCode, int traffic, int distance, int speed, int time, const QString&) const;
#endif

private:
    QVariant mModel;
    bool mIsLocal;
    int mSkipItemCount;
    QStringList mAddresses;
};

}//SKT

#endif // ROUTESUMMARYHELPER_H
