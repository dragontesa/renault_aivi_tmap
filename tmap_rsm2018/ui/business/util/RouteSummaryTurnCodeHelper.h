#ifndef ROUTESUMMARYTURNCODEHELPER_H
#define ROUTESUMMARYTURNCODEHELPER_H

namespace SKT
{

class RouteSummaryTurnCodeHelper
{
public:
    static int fromRsdTurnCode(int rsd);
    static bool isSummaryCode(int rsd);
    static bool isNoRg(int rsd);
};

}

#endif // ROUTESUMMARYTURNCODEHELPER_H
