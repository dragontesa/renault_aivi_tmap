#ifndef HIGHWAYTBTVIEWMEDIATOR_H
#define HIGHWAYTBTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "RgDataVo.h"

namespace SKT {

class HighwayTbtViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated = 0,
        VisibleChanged,
        ListVisibleChanged,
        RegistCsw,
        RemoveCsw,
        SelectCsw
    };

    Q_INVOKABLE HighwayTbtViewMediator( QObject* viewComponent = nullptr );

    ~HighwayTbtViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );

    bool handleNotification( const mvc::INotificationPtr& note );

private:    
    static QVariantMap makeModel( const QString& type,
        bool noPrice,
        bool noData,
        const QString& name,
        const QString& popIcon,
        const QString& toll,
        const QVariantList& distance,
        const QString& progress,
        const QString& station,
        const QVariantList& oilAndPrice,
        bool isEvRapidCharger,
        bool isEvSlowCharger
        );

    void updateView( const RgDataVo& rgDataVo );

    QStringList mLastDistances;
    QString mShowSetting;
    bool mCrossImageVisible = false;
    int mCurrentOilType;
    int mTest = -1;
    bool mIsOnline = false;
};

}

#endif // HIGHWAYVIEWMEDIATOR_H
