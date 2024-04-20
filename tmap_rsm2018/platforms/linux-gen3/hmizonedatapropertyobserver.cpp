#include "hmizonedatapropertyobserver.h"

#include <QDebug>

namespace SKT {

static const char* PropName_Zone3PullUpListState         = "zone3PullUpListState";


HMIZoneDataPropertyObserver::HMIZoneDataPropertyObserver(QObject *parent)
    : QObject(parent)
    , m_zone3PullUpListState(CLOSED)
{
    if (parent) {
        connect(parent, SIGNAL(propertyChanged(QString,QVariant)),
                this, SLOT(onPropertyChanged(QString,QVariant)));

        // set initial value
        m_zone3PullUpListState = qvariant_cast<uchar>(parent->property(PropName_Zone3PullUpListState)) == 1;

        qDebug() << "HMIZoneData property zone3PullUpListState: " << m_zone3PullUpListState;
    }
}

void HMIZoneDataPropertyObserver::onPropertyChanged(const QString &propertyName, const QVariant &value)
{
#if 0 // Too verbose
    qDebug() << Q_FUNC_INFO << "name:" << propertyName << "value:" << value;
#endif
    if (propertyName == QLatin1String(PropName_Zone3PullUpListState)) {
        bool pullUpListState = qvariant_cast<uchar>(value);
        qDebug() << "HMIZoneDataPropertyObserver::onPropertyChanged(" << pullUpListState << ")";

        if (m_zone3PullUpListState != pullUpListState) {
            m_zone3PullUpListState = pullUpListState;
            emit zone3PullUpListStateChanged(pullUpListState);
        }
    }
}

} // namespace SKT
