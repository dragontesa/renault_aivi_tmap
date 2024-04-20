#ifndef RPSTATUS_H
#define RPSTATUS_H

#include "rprequest.h"

namespace SKT
{

class RpStatus
{
public:
    enum Status {
        Ready,
        Request,
        Done,
    };
    RpStatus();

    void init();
    void reset();
    void setOptions(QList<int> options);
    int option(int index) const;
    int optionIndex(TeRouteOption option) const;
    int optionCount() const;
    int routeIdByOption(TeRouteOption option) const;
    int routeIdByIndex(int index) const;
    int doneCount() const;
    void request(const QList<TeRouteOption>& options, bool isServer);
    void updateRpDone(QStringListPtr ptr);
    void updateRpDone(TeRouteOption option);
    QString toString(int status) const;
    QVariantList getTvasDataList(bool isOnline) const;
    QStringList getRouteColorList(bool isOnline) const;
    bool isValidRouteIndex(int routeId) const;
    QString getTvasFilename(int routeId, bool isOnline) const;
    bool isServerData(int index) const;
    bool hasRequest() const;
    bool isSwapOption(bool isOnline) const;
    void switchToLocal();

private:
    struct Data {
        Status status;
        int option;
        bool isServerData;
    };
    Data mData[3];
};

}//SKT
#endif // RPSTATUS_H
