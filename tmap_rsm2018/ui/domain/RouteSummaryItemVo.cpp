#include "RouteSummaryItemVo.h"

namespace SKT {

RouteSummaryItemVo::RouteSummaryItemVo()
{

}

RouteSummaryItemVo::RouteSummaryItemVo(int x,
                               int y,
                               int turn,
                               int turnRemainDistance,
                               const QString &from,
                               const QString &to,
                               int traffic,
                               int speed,
                               int remainTime)
                               : mX(x)
                               , mY(y)
                               , mTurnRemainDistance(turnRemainDistance)
                               , mFrom(from)
                               , mTo(to)
                               , mSpeed(speed)
                               , mRemainTime(remainTime)
{
    switch (traffic) {
    case 0: mTraffic = 0; break;
    case 48: mTraffic = 0; break;   //'0'
    case 49: mTraffic = 1; break;   //'1'
    case 50: mTraffic = 2; break;   //'2'
    case 52: mTraffic = 3; break;   //'4'
    default:
        T_DEBUGV("invalide traffic code : %d", traffic);
        break;
    }

#if (0) // turn code 변환을 VsmNaviProxy에서 진행.
    int type = 0;
    switch(turn)
    {
    case 0:
        type = 0;
        break;

    case 11:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 248:
        type = 1;
    break;
    case 13:
        type = 3;//2;
        break;
    case 12:
        type = 2;//3;
        break;
    case 18:
        type = 4;
        break;
    case 17:
        type = 5;
        break;
    case 14:
        type = 6;
        break;
    case 16:
        type = 7;
        break;
    case 19:
        type = 8;
        break;
    case 44:
        type = 9;
        break;
    case 43:
        type = 10;
        break;
    case 15:
        type = 11;
        break;
    case 103:
    case 113:
        type = 12;
        break;
    case 106:
    case 116:
        type = 13;
        break;
    case 102:
    case 112:
        type = 14;
        break;
    case 105:
    case 115:
        type = 15;
        break;
    case 101:
    case 111:
        type = 16;
        break;
    case 104:
    case 114:
        type = 17;
        break;
    case 7:
    case 118:
    case 75:
    case 76:
        type = 20;
        break;
    case 6:
    case 73:
    case 74:
        break;
    case 117:
        type = 10;//21;
        break;
    case 151:
    case 152:
        type = 22;
        break;
    case 185:
    case 186:
    case 187:
    case 200: //type = 24;
    case 201: //type = 23;
        type = turn;
        break;
    case 153:
    case 154:
    case 249:
        type = 28;
        break;
    case 121:
        type = 29;
        break;
    case 124:
        type = 31;
        break;
    case 120:
        type = 33;
        break;
    case 123:
        type = 34;
        break;
    case 119:
        type = 36;
        break;
    case 133:
        type = 37;
        break;
    case 131:
    case 132:
        type = 40;
        break;
    case 134:
    case 135:
        type = 42;
        break;
    case 139:
        type = 44;
        break;
    case 140:
    case 141:
        type = 45;
        break;
    case 137:
    case 138:
        type = 46;
        break;
    case 142:
        type = 47;
        break;

    default:
        T_DEBUGV("RouteSummaryItemVo.turn(%i) is invalid ###", turn);
        break;
    }
    mTurn = type;
#else
    mTurn = turn;
#endif
}

RouteSummaryItemVo::RouteSummaryItemVo(const RouteSummaryItemVo &other) :
    mX(other.mX),
    mY(other.mY),
    mTurn(other.mTurn),
    mTurnRemainDistance(other.mTurnRemainDistance),
    mFrom(other.mFrom),
    mTo(other.mTo),
    mTraffic(other.mTraffic),
    mSpeed(other.mSpeed),
    mRemainTime(other.mRemainTime)
{
}

int RouteSummaryItemVo::x() const
{
    return mX;
}

int RouteSummaryItemVo::y() const
{
    return mY;
}

int RouteSummaryItemVo::turn() const
{
    return mTurn;
}

int RouteSummaryItemVo::turnRemainDistance() const
{
    return mTurnRemainDistance;
}

QString RouteSummaryItemVo::from() const
{
    return mFrom;
}

QString RouteSummaryItemVo::to() const
{
    return mTo;
}

int RouteSummaryItemVo::traffic() const
{
    return mTraffic;
}

int RouteSummaryItemVo::speed() const
{
    return mSpeed;
}

int RouteSummaryItemVo::remainTime() const
{
    return mRemainTime;
}

void RouteSummaryItemVo::setFrom(const QString &from)
{
    mFrom = from;
}

void RouteSummaryItemVo::setTo(const QString &to)
{
    mTo = to;
}

void RouteSummaryItemVo::setTurnCode(int turnCode)
{
    mTurn = turnCode;
}

void RouteSummaryItemVo::setPos(int x, int y)
{
    mX = x;
    mY = y;
}

} //SKT
