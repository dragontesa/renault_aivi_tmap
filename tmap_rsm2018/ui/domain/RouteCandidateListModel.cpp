#include "RouteCandidateListModel.h"
#include "RouteSummaryVo.h"
#include "TCore.h"

namespace SKT {

RouteCandidateListModel::RouteCandidateListModel(QObject *parent) : QAbstractListModel(parent)
{
}

RouteCandidateListModel::RouteCandidateListModel(const RouteCandidateListModel &other, QObject *parent)
    : QAbstractListModel(parent)
    , mList(other.mList)
{
}

RouteCandidateListModel::~RouteCandidateListModel()
{
    clear();
}

QHash<int, QByteArray> RouteCandidateListModel::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        { TitleRole, "Title" },
        { RemainTimeRole, "RemainTimeValue" },
        { RemainDistanceRole, "RemainDistance" },
        { FeeRole, "Fee" },
        { MinPointRole, "MinPoint" },
        { MaxPointRole, "MaxPoint" },
        { StartNameRole, "Start" },
        { EndNameRole, "End" }
    };

    return roles;
}

int RouteCandidateListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return mList.count();
}

QVariant RouteCandidateListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    if (row < 0 || row >= mList.count())
        return QVariant();

    auto vo = mList[row];

    switch (role) {
    case TitleRole:
        return vo->type();

    case RemainDistanceRole:
        return vo->distance();

    case RemainTimeRole:
        return vo->remainTime();

    case FeeRole:
        return vo->fee();

    case MinPointRole:
        return vo->minPoint();

    case MaxPointRole:
        return vo->maxPoint();

    case StartNameRole:
        return vo->from();

    case EndNameRole:
        return vo->to();

    default:
        break;
    }

    return QVariant();
}

void RouteCandidateListModel::append(const RouteSummaryVoPtr &ptr)
{
    for (auto v : mList) {
        if (v->type() == ptr->type()) {
            v->setDistance(ptr->distance());
            v->setFee(ptr->fee());
            v->setFrom(ptr->from());
            v->setMaxPoint(ptr->maxPoint());
            v->setMinPoint(ptr->minPoint());
            v->setRemainTime(ptr->remainTime());
            v->setRoadInfo(ptr->roadInfo());
            v->setTo(ptr->to());
            v->setType(ptr->type());
            v->setWeather(ptr->weather());
            return;
        }
    }

    auto vo = new RouteSummaryVo;
    vo->setDistance(ptr->distance());
    vo->setFee(ptr->fee());
    vo->setFrom(ptr->from());
    vo->setMaxPoint(ptr->maxPoint());
    vo->setMinPoint(ptr->minPoint());
    vo->setRemainTime(ptr->remainTime());
    vo->setRoadInfo(ptr->roadInfo());
    vo->setTo(ptr->to());
    vo->setType(ptr->type());
    vo->setWeather(ptr->weather());

    mList.append(vo);
}

int RouteCandidateListModel::count() const
{
    return mList.count();
}

int RouteCandidateListModel::validDataCount() const
{
    int count = 0;
    for (auto vo : mList) {
        if (vo->hasData()) {
            ++count;
        }
    }
    return count;
}

void RouteCandidateListModel::clear()
{
    for (auto vo : mList) {
        delete vo;
    }
    mList.clear();
}

bool RouteCandidateListModel::hasData() const
{
    for (auto vo : mList) {
        if (vo->hasData())
            return true;
    }
    return false;
}

QVariant RouteCandidateListModel::get(quint32 index) const
{
    auto vo = mList.at(index);
    return QVariant::fromValue(vo);
}

RouteSummaryVo *RouteCandidateListModel::getData(quint32 index) const
{
    return mList.at(index);
}

}//SKT
