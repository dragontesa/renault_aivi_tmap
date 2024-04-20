#ifndef ROUTECANDIDATELISTMODEL_H
#define ROUTECANDIDATELISTMODEL_H

#include <QAbstractListModel>
#include "RouteSummaryVo.h"

namespace SKT {

T_SHAREABLE(RouteCandidateListModel)

class RouteCandidateListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        RemainTimeRole,
        RemainDistanceRole,
        FeeRole,
        MinPointRole,
        MaxPointRole,
        StartNameRole,
        EndNameRole
    };

    RouteCandidateListModel(QObject *parent = 0);
    RouteCandidateListModel(const RouteCandidateListModel &other, QObject *parent = 0);
    virtual ~RouteCandidateListModel();

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;

    void append(const RouteSummaryVoPtr &data);
    int count() const;
    int validDataCount() const;
    void clear();
    bool hasData() const;

    Q_INVOKABLE QVariant get(quint32 index) const;
    RouteSummaryVo *getData(quint32 index) const;

private:
    QList<RouteSummaryVo*> mList;
};

}//SKT

Q_DECLARE_METATYPE(SKT::RouteCandidateListModel)
Q_DECLARE_METATYPE(SKT::RouteCandidateListModelPtr)

#endif // ROUTECANDIDATELISTMODEL_H
