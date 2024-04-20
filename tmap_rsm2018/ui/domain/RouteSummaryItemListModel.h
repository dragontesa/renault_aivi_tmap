#ifndef ROUTESUMMARYITEMLISTMODEL_H
#define ROUTESUMMARYITEMLISTMODEL_H

#include <QAbstractListModel>
#include "RouteSummaryItemVo.h"

namespace SKT {

T_SHAREABLE(RouteSummaryItemListModel)
T_SHAREABLE(RouteSummaryItemVo)

class RouteSummaryItemListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        PosXRole = Qt::UserRole + 1,
        PosYRole,
        TurnRole,
        TurnRemainDistanceRole,
        FromRole,
        ToRole,
        TrafficRole,
        SpeedRole,
        RemainTimeRole
    };

    RouteSummaryItemListModel(QObject *parent = 0);
    RouteSummaryItemListModel(const RouteSummaryItemListModel &other, QObject *parent = 0);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    void append(const RouteSummaryItemVo &data);
    void insert(int pos, const RouteSummaryItemVo &data);
    Q_INVOKABLE int count() const;

    Q_INVOKABLE QVariant get(int index) const;

private:
    QList<RouteSummaryItemVoPtr> mList;
};

}//SKT

Q_DECLARE_METATYPE(SKT::RouteSummaryItemListModel)
Q_DECLARE_METATYPE(SKT::RouteSummaryItemListModelPtr)

#endif // ROUTESUMMARYITEMLISTMODEL_H
