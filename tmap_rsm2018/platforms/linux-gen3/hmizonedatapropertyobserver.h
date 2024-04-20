#ifndef HMIZONEDATAPROPERTYOBSERVER_H
#define HMIZONEDATAPROPERTYOBSERVER_H

#include <QObject>
#include "hmiZonetype.h"

namespace SKT {

class HMIZoneDataPropertyObserver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool m_zone3PullUpListState READ zone3PullUpListState NOTIFY zone3PullUpListStateChanged)


public:
    explicit HMIZoneDataPropertyObserver(QObject *parent = 0);

    uint zone3PullUpListState()                        const { return m_zone3PullUpListState; }

signals:
    void zone3PullUpListStateChanged(uint zone3PullUpListState);

public slots:

private slots:
    void onPropertyChanged(const QString &propertyName, const QVariant &value);

private:
    uint m_zone3PullUpListState;
};

}

#endif // HMIZONEDATAPROPERTYOBSERVER_H
