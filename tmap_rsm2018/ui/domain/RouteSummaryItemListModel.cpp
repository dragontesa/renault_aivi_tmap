#include "RouteSummaryItemListModel.h"
#include "RouteSummaryItemVo.h"
#include "TCore.h"

namespace SKT {

RouteSummaryItemListModel::RouteSummaryItemListModel(QObject *parent) : QAbstractListModel(parent)
{
}

RouteSummaryItemListModel::RouteSummaryItemListModel(const RouteSummaryItemListModel &other, QObject *parent)
    : mList(other.mList)
    , QAbstractListModel(parent)
{
}

QHash<int, QByteArray> RouteSummaryItemListModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        { PosXRole, "X" },
        { PosYRole, "Y" },
        { TurnRole, "Turn" },
        { TurnRemainDistanceRole, "TurnRemainDistance" },
        { FromRole, "From" },
        { ToRole, "To" },
        { TrafficRole, "Traffic" },
        { SpeedRole, "Speed" },
        { RemainTimeRole, "RemainTime" }
    };

    return roles;
}

int RouteSummaryItemListModel::rowCount(const QModelIndex &parent) const
{
    return mList.count();
}

QVariant RouteSummaryItemListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
//    int col = index.column();

//    T_DEBUGV("RouteSummaryItemListModel::data(%d,%d)", row, col);

    if (row < 0 || row >= mList.count()) {
        T_DEBUGV("RouteSummaryItemListModel::data(%d) return QVariant()", row);
        return QVariant();
    }

    auto vo = mList[row];
    QVariant v;

    switch (role) {
    case PosXRole:
        v = vo->x();
        break;
    case PosYRole:
        v = vo->y();
        break;
    case TurnRole:
        v = vo->turn();
        break;
    case TurnRemainDistanceRole:
        v = vo->turnRemainDistance();
        break;
    case FromRole:
        v = vo->from();
        break;
    case ToRole:
        v = vo->to();
        break;
    case TrafficRole:
        v = vo->traffic();
        break;
    case SpeedRole:
        v = vo->speed();
        break;
    case RemainTimeRole:
        v = vo->remainTime();
        break;
    }

    return v;
}

void RouteSummaryItemListModel::append(const RouteSummaryItemVo &data)
{
    auto vo = t_new_shared<RouteSummaryItemVo>(data);
    mList.append(vo);
}

void RouteSummaryItemListModel::insert(int pos, const RouteSummaryItemVo &data)
{
    auto vo = t_new_shared<RouteSummaryItemVo>(data);
    mList.insert(pos, vo);
}

int RouteSummaryItemListModel::count() const
{
    return mList.count();
}

QVariant RouteSummaryItemListModel::get(int index) const
{
    auto vo = mList.at(index);
    return QVariant::fromValue(vo);
}

}//SKT
