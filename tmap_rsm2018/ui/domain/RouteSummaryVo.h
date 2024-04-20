#ifndef ROUTESUMMARYVO_H
#define ROUTESUMMARYVO_H

//#include <QObject>
#include <QString>
#include "TCore.h"

namespace SKT {

T_SHAREABLE(RouteSummaryVo)

class RouteSummaryVo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QString to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(QString roadInfo READ roadInfo WRITE setRoadInfo NOTIFY roadInfoChanged)
    Q_PROPERTY(QPoint minPoint READ minPoint WRITE setMinPoint NOTIFY minPointChanged)
    Q_PROPERTY(QPoint maxPoint READ maxPoint WRITE setMaxPoint NOTIFY maxPointChanged)
    Q_PROPERTY(int distance READ distance WRITE setDistance NOTIFY distanceChanged)
    Q_PROPERTY(int weather READ weather WRITE setWeather NOTIFY weatherChanged)
    Q_PROPERTY(int remainTime READ remainTime WRITE setRemainTime NOTIFY remainTimeChanged)
    Q_PROPERTY(int fee READ fee WRITE setFee NOTIFY feeChanged)

public:
    RouteSummaryVo(QObject *parent = nullptr);

    RouteSummaryVo(
            const QString &type,
            const QString &from,
            const QString &to,
            const QString &roadInfo,
            const QPoint &min,
            const QPoint &max,
            int distance,
            int weather,
            int remainTime,
            int fee,
            QObject *parent = nullptr);

    RouteSummaryVo(const RouteSummaryVo &other);
    RouteSummaryVo &operator=(const RouteSummaryVo &other);
    ~RouteSummaryVo();

    Q_INVOKABLE void setType(const QString &type);
    Q_INVOKABLE void setFrom(const QString &from);
    Q_INVOKABLE void setTo(const QString &to);
    Q_INVOKABLE void setRoadInfo(const QString &roadInfo);
    Q_INVOKABLE void setMinPoint(const QPoint &pt);
    Q_INVOKABLE void setMaxPoint(const QPoint &pt);
    Q_INVOKABLE void setDistance(int distance);
    Q_INVOKABLE void setWeather(int weather);
    Q_INVOKABLE void setRemainTime(int time);
    Q_INVOKABLE void setFee(int fee);

    bool isDistanceOver(bool isOnline) const;
    bool hasData() const;

public slots:
    QString type() const;
    QString from() const;
    QString to() const;
    QString roadInfo() const;
    QPoint minPoint() const;
    QPoint maxPoint() const;
    int distance() const;
    int weather() const;
    int remainTime() const;
    int fee() const;

signals:
    void typeChanged();
    void fromChanged();
    void toChanged();
    void roadInfoChanged();
    void minPointChanged();
    void maxPointChanged();
    void distanceChanged();
    void weatherChanged();
    void remainTimeChanged();
    void feeChanged();

private:
    QString mType;
    QString mFrom;
    QString mTo;
    QString mRoadInfo;
    QPoint mMinPoint;
    QPoint mMaxPoint;
    int mDistance;
    int mWeather;
    int mRemainTime;
    int mFee;
};

}//SKT

Q_DECLARE_METATYPE(SKT::RouteSummaryVo)
Q_DECLARE_METATYPE(SKT::RouteSummaryVoPtr)

#endif // ROUTESUMMARYVO_H
