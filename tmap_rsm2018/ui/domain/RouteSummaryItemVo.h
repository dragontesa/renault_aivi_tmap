#ifndef ROUTESUMMARYITEMVO_H
#define ROUTESUMMARYITEMVO_H

#include <QString>
#include "TCore.h"

namespace SKT {

T_SHAREABLE(RouteSummaryItemVo)

class RouteSummaryItemVo
{
    Q_GADGET

    Q_PROPERTY(int distance READ turnRemainDistance CONSTANT)

public:
    RouteSummaryItemVo();

    RouteSummaryItemVo(
            int x,
            int y,
            int turn,
            int turnRemainDistance,
            const QString &from,
            const QString &to,
            int traffic,
            int speed,
            int remainTime);

    RouteSummaryItemVo(const RouteSummaryItemVo &other);

    int x() const;
    int y() const;
    int turn() const;
    Q_INVOKABLE int turnRemainDistance() const;
    QString from() const;
    QString to() const;
    int traffic() const;
    int speed() const;
    int remainTime() const;

    void setFrom(const QString &from);
    void setTo(const QString &to);
    void setTurnCode(int turnCode);
    void setPos(int x, int y);

private:
    int mX;
    int mY;
    int mTurn;
    int mTurnRemainDistance;
    QString mFrom;
    QString mTo;
    int mTraffic;
    int mSpeed;
    int mRemainTime;
};

}//SKT

Q_DECLARE_METATYPE(SKT::RouteSummaryItemVo)
Q_DECLARE_METATYPE(SKT::RouteSummaryItemVoPtr)

#endif // ROUTESUMMARYITEMVO_H
